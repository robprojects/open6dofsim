#pragma once
#include "qserialport.h"
#include <QtCore/qtimer.h>
#include "ui_configActuator.h"
#include "output.h"
#include "selector.h"



#define FLAG_SETCONSTANT (1<<5)
#define FLAG_PONG (1<<6)
#define FLAGS_STATE(X) ((X)&0xf)
#define FLAG_BRAKE_LIMIT (1<<7)

class Actuator :  public QDialog, public Output {
	Q_OBJECT
public:
	explicit Actuator(QWidget *parent = nullptr);
	~Actuator();
	void send_serial_command(int* len);
	QString& getStatus();
	QString& name() {
		static QString n("Open6DOF");
		return n;
	}
	void openConfigDialog();

	void setActive();
	void setInactive();

public slots:
	void handleReadyRead();
	void handleTimeout();
	void handleError(QSerialPort::SerialPortError error);
	void clickOK();

private:
	typedef enum
	{
		Parser_P = 0,
		Parser_A = 1,
		Parser_A_val = 2,
		Parser_D = 3,
		Parser_D_val = 4,
		Parser_F_val = 5
	} ParserState;

	typedef enum
	{
		Wait_Pong = 0,
		Wait_PWR = 1,
		Wait_P = 2,
		Wait_I = 3,
		Wait_D = 4,
		WaitHome = 5,
		Done = 6,
		Failed = 7
	} MachineState;

	char state_text[8][20] = {
		"Wait_Pong",
		"Wait_PWR",
		"Wait_P",
		"Wait_I",
		"Wait_D",
		"WaitHome",
		"Done",
		"Failed"
	};

	typedef struct {
		int cmd_pos;
		int actual_pos;
		//struct timeval last_response;
		int last_flags;
		int num_responses;
		int active;
	} actuator_state;

	typedef enum {
		K_P = 0,
		K_D = 1,
		K_I = 2,
		K_PWR = 3,
		K_STALL_THOLD_HOME = 4,
		K_STALL_THOLD_RUN = 5,
		K_IDLE_TIMEOUT = 6,
		K_I_LIMIT = 7
	} act_constant_t;

	typedef enum { SERVO_INIT = 0, SERVO_FINDHOME = 1, SERVO_RUN = 2, SERVO_ERROR = 3, SERVO_STOP = 4, SERVO_STOP_NOHOME = 5 } servo_state_t;

	void openSerialPort();
	//void setup_serial();
	QSerialPort* m_serial_port;
	void send_home(void);
	void send_ping(void);
	void send_constant_PID(act_constant_t c, float k);
	void send_constant(act_constant_t c, int k);
	void parser_recv(char c);
	void statemachine();
	void waitresponse(MachineState next);

	MachineState m_state;
	actuator_state m_act_state[6];
	ParserState m_parser;
	int m_parser_axis;
	int m_parser_axis_value;
	int m_parser_axis_flags;

	int m_home_count;

	QTimer *m_timer;
	QString *m_port_name;
	Ui::configActuator ui;

};