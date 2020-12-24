#include "mainwindow.h"
#include <QTimer>
#include <QDebug>
#include "ui_configWashout.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_pos.phi = m_pos.theta = m_pos.psi = 0.0;
    m_geo6dof = new Geo6Dof(this);

    ui.setupUi(this);

    // populate inputs
    m_input = new Selector(ui.inputSelect, ui.inputConfig);
    m_input->addOption(new XPlane(this));
    m_input->addOption(new NoInput(), true);

    m_output = new Selector(ui.outputSelect, ui.outputConfig);
    m_output->addOption(new Actuator(this));
    m_output->addOption(new NoOutput(), true);

    m_alg = new Selector(ui.algSelect, ui.algConfig);
    m_alg->addOption(new Washout(this), true);
    m_alg->addOption(new Manual(this));

    //m_washout = new Washout();

    m_geo6dof->getPose(&m_pose);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(frameTimer()));

    m_timer->start(1000 / SAMPLE);

    ui.renderwindow->updateDisplay(&m_pose);

    qDebug("START");

    m_stopped = 1;
    m_stop_height = 0;

    connect(ui.startButton, SIGNAL(clicked()), this, SLOT(startButtonPress()));
 
    //connect(ui.algConfig, SIGNAL(clicked()), this, SLOT(openAlgConfig()));
    //connect(ui.algSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(algSelect(int)));

    connect(ui.actionPlatform_setup, SIGNAL(triggered()), this, SLOT(openPlatformConfig()));
    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quitApp()));

    // populate algorithms
    //ui.algSelect->addItem(QString("Manual position"));
    //ui.algSelect->addItem(QString("Classical Washout"));
}

void MainWindow::frameTimer() {

    // main loop
    //qDebug("Timer");
    // get data from source
    double Faa[3], Oaa[3];
    int valid = dynamic_cast<Input*>(m_input->getCurrent())->ReadNext(Faa, Oaa);

    //qDebug("Faa %f %f %f", Faa[0], Faa[1], Faa[2]);
    //qDebug("Oaa %f %f %f", Oaa[0], Oaa[1], Oaa[2]);

    // washout filter

    dynamic_cast<Algorithm*>(m_alg->getCurrent())->compute2(Faa, Oaa, &m_pos);
    //pos.T[2] += geo.mid_height;

    // update 6dof
    if ((!m_stopped) && (valid)) {
        if (m_stop_height < 500) {
            m_stop_height += 100 / SAMPLE;
            m_geo6dof->setStoppedPos(m_stop_height);
        }
        else {
            m_geo6dof->setPos(&m_pos);
        }
    } else {
        if (m_stop_height > 0) m_stop_height -= 100 / SAMPLE;
        m_geo6dof->setStoppedPos(m_stop_height);
    }

    m_geo6dof->getPose(&m_pose);

    // send commands to actuators
    int len[6];
    m_geo6dof->getLengths(len);

    dynamic_cast<Output*>(m_output->getCurrent())->send_serial_command(len);
   
    // update display
    ui.renderwindow->updateDisplay(&m_pose);

    ui.inputStatus->setText(m_input->getStatus());
    ui.outputStatus->setText(m_output->getStatus());
}

void MainWindow::startButtonPress()
{
    if (m_stopped) {
        m_stopped = 0;
        ui.startButton->setText("Stop");
        ui.inputSelect->setDisabled(true);
        ui.outputSelect->setDisabled(true);
        ui.algSelect->setDisabled(true);
        ui.actionPlatform_setup->setEnabled(false);
    } else {
        m_stopped = 1;
        ui.startButton->setText("Start");
        ui.inputSelect->setDisabled(false);
        ui.outputSelect->setDisabled(false);
        ui.algSelect->setDisabled(false);
        ui.actionPlatform_setup->setEnabled(true);
    }
}

void MainWindow::openPlatformConfig()
{
    qDebug("open platform config");
    m_geo6dof->openConfigDialog();
}

void MainWindow::quitApp() {
    qApp->exit();
}
