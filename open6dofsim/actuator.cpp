#include <stdio.h>
#include <math.h>

//#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <termios.h>
#include <string.h>
//#include <pthread.h>
#include <stdint.h>
//#include "sys/time.h"

#include <qserialportinfo.h>
#include <qserialport.h>

#include "actuator.h"

#define fx_t int32_t
#define FX_FRAC 8
#define fx_make(a)  ((fx_t)((a) * ((int)1<<FX_FRAC)))


//int usb;
//pthread_t serial_recv_thread;
//FILE *logfile;




#define EXTENT ((250/5)*(1200/2))

//void *recv_serial_response_thread(void *);
//actuator_state act_state[6];

#define TIMEOUT 1000

char servo_state[16][32] = { "Servo_Init", "Servo_FindHome", "Servo_Run", "Servo_Error", "Servo_Stop", "Servo_Stop_NoHome" };


void Actuator::openConfigDialog()
{
	ui.serialport->clear();

	int index = 0, i = 0;
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo& info : infos) {
		qDebug("Port: %s", qUtf8Printable(info.portName()));
		qDebug("Loc: %s", qUtf8Printable(info.systemLocation()));
		qDebug("Desc: %s", qUtf8Printable(info.description()));
		qDebug("Man: %s", qUtf8Printable(info.manufacturer()));

		ui.serialport->addItem(info.portName());

		if (m_port_name != NULL && info.portName() == *m_port_name) {
			index = i;
		}
		i++;
	}
	ui.serialport->setCurrentIndex(index);

	// FDTI is manufacturer
	show();
}

void Actuator::clickOK() {
	if (m_port_name) delete m_port_name;
	m_port_name = new QString(ui.serialport->currentText());
	openSerialPort();
}

Actuator::Actuator(QWidget *parent) : QDialog(parent) {
	
	m_state = Failed;
	m_port_name = NULL;
	const auto infos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo& info : infos) {
		qDebug("Port: %s", qUtf8Printable(info.portName()));
		qDebug("Loc: %s", qUtf8Printable(info.systemLocation()));
		qDebug("Desc: %s", qUtf8Printable(info.description()));
		qDebug("Man: %s", qUtf8Printable(info.manufacturer()));

		if (info.manufacturer() == "FTDI") {
			m_port_name = new QString(info.portName());
		}
	}

	m_serial_port = new QSerialPort();
	//const QString portname("COM4");

	m_serial_port->setBaudRate(QSerialPort::Baud115200);
	m_serial_port->setDataBits(QSerialPort::Data8);
	m_serial_port->setParity(QSerialPort::NoParity);
	m_serial_port->setStopBits(QSerialPort::OneStop);
	m_serial_port->setFlowControl(QSerialPort::NoFlowControl);

	connect(m_serial_port, &QSerialPort::readyRead, this, &Actuator::handleReadyRead);
	connect(m_serial_port, &QSerialPort::errorOccurred, this, &Actuator::handleError);

	ui.setupUi(this);

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
	m_timer->setSingleShot(true);

	//m_timer->singleShot(TIMEOUT, this, &Actuator::handleTimeout);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(clickOK()));
}

void Actuator::setActive() {
	openSerialPort();
}

void Actuator::setInactive() {
	m_timer->stop();
	m_state = Failed;
	m_serial_port->close();
}

void Actuator::waitresponse(MachineState next) {
	for (int i = 0; i < 6; i++) {
		if (!m_act_state[i].num_responses) {
			qDebug("Waiting for actuator %d", i);
			return;
		}
	}
	qDebug("Transitioning to state %s\n", state_text[m_state]);
	for (int i = 0; i < 6; i++) m_act_state[i].num_responses = 0;
	m_state = next;
}

void Actuator::statemachine() {
	qDebug("Entering state machine state=%s", state_text[m_state]);
	int needs_home;
	switch (m_state) {
	case Wait_Pong:
		waitresponse(Wait_PWR);
		send_constant(K_PWR, 300);
		m_timer->start(TIMEOUT);
		break;
	case Wait_PWR:
		waitresponse(Wait_P);
		send_constant_PID(K_P, 0.20);
		m_timer->start(TIMEOUT);
		break;
	case Wait_P:
		waitresponse(Wait_I);
		send_constant_PID(K_I, 0.002);
		m_timer->start(TIMEOUT);
		break;
	case Wait_I:
		waitresponse(Wait_D);
		send_constant_PID(K_D, 0.1);
		m_timer->start(TIMEOUT);
		break;
	case Wait_D:
		needs_home = 0;
		for (int i = 0; i < 6; i++) {
			if (!m_act_state[i].num_responses) return;
			if (FLAGS_STATE(m_act_state[i].last_flags) != 4 /* Servo_Stop */) needs_home = 1;
		}
		for (int i = 0; i < 6; i++) m_act_state[i].num_responses = 0;
		if (needs_home) {
			m_state = WaitHome;
			m_home_count = 0;
			send_home();
			m_timer->start(TIMEOUT);
		} else {
			m_state = Done;
		}
		break;
	case WaitHome:
		needs_home = 0;
		for (int i = 0; i < 6; i++) {
			if (FLAGS_STATE(m_act_state[i].last_flags) != 4 /* Servo_Stop */) needs_home = 1;
		}
		if (!needs_home) {
			qDebug("Done!");
			m_state = Done;
		} else {
			send_ping();
		}
		break;
	case Done:
		// check status of flags
		for (int i = 0; i < 6; i++) {
			if (FLAGS_STATE(m_act_state[i].last_flags) == 3 /* Servo_Error */) {
				// actuator failed
				m_state = Failed;
			}
		}
		break;
	}
}

void Actuator::handleReadyRead() {
	QByteArray bytes = m_serial_port->readAll();
	QByteArray::iterator ptr;

	for (ptr = bytes.begin(); ptr < bytes.end(); ptr++) {
		//qDebug("got: %c", *ptr);
		parser_recv(*ptr);
	}
}

void Actuator::handleTimeout() {
	//qDebug("Timeout!\n");
	qDebug("Timeout state=%s", state_text[m_state]);
	switch (m_state) {
	case WaitHome:
		if (m_home_count <= 20) {
			m_home_count++;
			m_timer->start(TIMEOUT);
			statemachine();
		} else {
			m_timer->start(TIMEOUT);
			m_state = Failed;
		}
		break;
	case Done:
		break;
	case Failed:
		// retry
		m_serial_port->close();
		openSerialPort();
		break;
	default:
		m_state = Failed;
		m_timer->start(TIMEOUT);
	}
}

void Actuator::handleError(QSerialPort::SerialPortError error) {
	if (error == QSerialPort::NoError) return;
	qDebug("Error state=%s error %d", state_text[m_state], error);
	m_state = Failed;
	m_timer->start(TIMEOUT);
}

void Actuator::openSerialPort()
{

	if (m_port_name == NULL) return;

	m_serial_port->setPortName(*m_port_name);
	// reset parser state
	m_parser = Parser_P;
	m_state = Wait_Pong;

	for (int i = 0; i < 6; i++) {
		m_act_state[i].active = 0;
		m_act_state[i].num_responses = 0;
	}

	qDebug("Opening serial port %s", qUtf8Printable(*m_port_name));
	m_serial_port->open(QIODevice::ReadWrite);

	send_ping();
	m_timer->start(TIMEOUT);

	m_home_count = 0;
}

#if 0

void setup_serial(void) {
	// serial port
	usb = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK | O_NDELAY);
	struct termios tty;

	memset (&tty, 0, sizeof tty);

	//memset (fs, sizeof(struct f_state) * 6 * 3, 0);
	
	cfsetospeed(&tty, (speed_t)B115200);
	cfsetispeed(&tty, (speed_t)B115200);
	tty.c_cflag &= ~PARENB & ~CSTOPB & ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 5;
	tty.c_cflag |= CREAD | CLOCAL;

	cfmakeraw(&tty);
	tcflush(usb, TCIFLUSH);
	tcsetattr(usb, TCSANOW, &tty);

	pthread_create(&serial_recv_thread, NULL, recv_serial_response_thread, (void *) NULL);

	logfile = fopen("log.csv", "w");

	memset(act_state, 0, sizeof(act_state));

	// init actuators
	int nr[6];
	int i;
	for (i=0; i<6; i++) nr[i] = act_state[i].num_responses;
	send_ping();
	int act_found = 0;
	for (i=0; i<1; i++) { // FIXME!
	//for (;;) {
		int j;
		act_found = 0;
		for (j=0; j<6; j++) if (nr[j] != act_state[j].num_responses) act_found++;
		if (act_found == 6) {printf("Found all 6!\n"); break; }
		usleep(1000);
		send_ping();
	}
	sleep(1);
	//exit(1);
	
	printf("Actuators found %d\n", act_found);
	for (i=0; i<6; i++) {
		if (nr[i] != act_state[i].num_responses) {
			printf("Actuator %d :\tState=%s\tPos=%d\n", i, servo_state[FLAGS_STATE(act_state[i].last_flags)], act_state[i].actual_pos); 
			act_state[i].active = 1;
		} else {
			printf("Actuator %d :\tNOT FOUND\n", i);
			act_state[i].active = 0;
		}
	}

	//exit(1);
	
	sleep(1);

	printf("Setting constants...\n");
	send_constant(K_PWR, 300);	

	sleep(1);

	send_constant_PID(K_P, 0.20);

	sleep(1);

	send_constant_PID(K_D, 0.1);

	sleep(1);

	send_constant_PID(K_I, 0.002);

	sleep(1);


	//if (act_found != 6) exit(0);	
	
	//exit(0);

	int j;
	int ready = 1;
	for (j=0; j<6; j++) ready &= ((!act_state[j].active) || (FLAGS_STATE(act_state[j].last_flags)==4 /*Servo_Stop*/)) ;
	if (ready) {
		printf("No need to home...\n");	
		return;
	}
	// Need to find home


	printf("Homing...\n");
	send_home();
	// wait till all are home (20 sec)
	for (i=0; i<20; i++) {
		int j;
		int ready = 1;
		for (j=0; j<6; j++) ready &= ((!act_state[j].active) || (FLAGS_STATE(act_state[j].last_flags)==4 /*Servo_Stop*/)) ;
		if (i>4 && ready) break;
		send_ping();
		sleep(1);
	}
	for (i=0; i<6; i++) {
		if (act_state[i].active) {
			printf("Actuator %d :\tState=%s\tPos=%d\n", i, servo_state[FLAGS_STATE(act_state[i].last_flags)], act_state[i].actual_pos); 
		} else {
			printf("Actuator %d :\tNOT FOUND\n", i);
		}
	}
}

#endif

void Actuator::parser_recv(char c) {
	if (c == 'P') {
		m_parser = Parser_A;
	}
	
	switch (m_parser) {
		case Parser_P:
		break;
		case Parser_A:
		if (c=='A') m_parser = Parser_A_val;
		break;
		case Parser_A_val:
		if (isdigit(c)) {
			m_parser_axis = c - '0';
			m_parser = Parser_D;
		} else m_parser = Parser_A;
		case Parser_D:
		if (c=='D') {
			m_parser = Parser_D_val;
			m_parser_axis_value = 0;
		}
		break;
		case Parser_D_val:
		if (isdigit(c)) {
			m_parser_axis_value*=10;
			m_parser_axis_value += c - '0';
		} else {
			if (c=='A')
				m_parser = Parser_A_val;
			else if (c=='F') {
				m_parser = Parser_F_val;
				m_parser_axis_flags = 0;
			} else
				m_parser = Parser_A;
		}
		break;
		case Parser_F_val:
		if (isdigit(c)) {
			m_parser_axis_flags *=10;
			m_parser_axis_flags += c - '0';
		} else {
			if (c=='A')
				m_parser = Parser_A_val;
			else
				m_parser = Parser_A;

			qDebug("RECEIVED: Axis=%d value=%d State=%s\n", m_parser_axis, m_parser_axis_value, servo_state[FLAGS_STATE(m_parser_axis_flags)]);
			//if (parser_axis_flags & FLAG_BRAKE_LIMIT) printf("Axis %d reached braking limit!\n", parser_axis);

			if (m_parser_axis >=0 && m_parser_axis < 6) {
				m_act_state[m_parser_axis].actual_pos = (int)((float)m_parser_axis_value * ((float)((1<<16)-1)/(float)EXTENT));
				//gettimeofday(&(act_state[parser_axis].last_response), NULL);
				m_act_state[m_parser_axis].last_flags = m_parser_axis_flags;
				m_act_state[m_parser_axis].num_responses++;
				statemachine();
			}
			//fprintf(logfile, "%d\t%d\n", act_state[2].actual_pos, act_state[2].cmd_pos);
			//fflush(logfile);
		}
		break;
	}
}

#if 0
void *recv_serial_response_thread(void * param) {
	ssize_t br;
	char cmd[256];
	while (1) {
		bzero(cmd, 256);
		br = read(usb, cmd, sizeof(cmd));
		if (br>0) {
			//printf("READ FROM SERIAL: %s\n", cmd);
			int i;	
			// parse response
			for (i=0; i<br; i++) {
				parser_recv(cmd[i]);
			}
		}
	}
}
#endif 

Actuator::~Actuator() {
	m_serial_port->deleteLater();
	m_timer->deleteLater();
	if (m_port_name) delete m_port_name;
}

void Actuator::send_serial_command(int *len) {
	int i;
	char cmd[64];

	if (m_state != Done) return;

	for (i=0; i<6; i++) {
		if (len[i]<8000) len[i] = 8000;
		if (len[i]>((1<<16)-8000)) len[i] = ((1<<16)-8000);
		m_act_state[i].cmd_pos = len[i];
		
		if (FLAGS_STATE(m_act_state[i].last_flags) != 2 && FLAGS_STATE(m_act_state[i].last_flags) != 4) {
			// Error
			qDebug("Error for actuator %d", i);
			m_state = Failed;
			m_timer->start(TIMEOUT);
		}

		//printf("Commanding %d to %d\n", i, len[i]);
	}

	sprintf(cmd, "PA0D%dA1D%dA2D%dA3D%dA4D%dA5D%dA", len[0], len[1], len[2], len[3], len[4], len[5]); // FIXME! order
	//sprintf(cmd, "PA1D%dA", len[1]);
        //printf("sending %s\n", cmd);
	size_t remain = strlen(cmd);
	size_t start = 0;

	while (remain>0) {
		start = m_serial_port->write(cmd + start, remain);
		remain -= start;
	}
}

QString& Actuator::getStatus()
{
	static QString stats[] = {
		QString("Waiting Ping response..."),
		QString("Setting PWR..."),
		QString("Setting P..."),
		QString("Setting I..."),
		QString("Setting D..."),
		QString("Homing..."),
		QString("Running..."),
		QString("Error")
	};
	return stats[m_state];
}



void Actuator::send_home(void) {
	char cmd[64];
	sprintf(cmd, "H");
	//write(usb, cmd, strlen(cmd));
	m_serial_port->write(cmd, strlen(cmd));
}

void Actuator::send_ping(void) {
	char cmd[64];
	sprintf(cmd, "I");
	//printf("ping...\n");
	//write(usb, cmd, strlen(cmd));
	m_serial_port->write(cmd, strlen(cmd));
}

void Actuator::send_constant_PID(act_constant_t c, float k) {
	char cmd[64];
	if (c<K_P || c>K_I) return;
	fx_t val;
	val = fx_make(k);
	sprintf(cmd, "PK%dD%dA\n", c, val);
	//write(usb, cmd, strlen(cmd));
	m_serial_port->write(cmd, strlen(cmd));
}

void Actuator::send_constant(act_constant_t c, int k) {
	char cmd[64];
	if (c<K_PWR || c>K_I_LIMIT) return;
	sprintf(cmd, "PK%dD%dA\n", c, k);
	//write(usb, cmd, strlen(cmd));
	m_serial_port->write(cmd, strlen(cmd));
}