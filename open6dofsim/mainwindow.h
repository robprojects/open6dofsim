#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QTimer>
#include "geo6dof.h"
#include "xplane.h"
#include "washout.h"
#include "actuator.h"
#include "input.h"
#include "qlinkedlist.h"
#include "selector.h"
#include "noinput.h"
#include "nooutput.h"
#include "manual.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget* parent = Q_NULLPTR);

    public slots:
        void frameTimer();
        void startButtonPress();
        void openPlatformConfig();
        void quitApp();

    private:
        Ui::MainWindow ui;
        QTimer *m_timer;
        Geo6Dof* m_geo6dof;
        struct Geo6Dof::display_pose m_pose;
        int m_stopped;
        int m_stop_height;
        Selector* m_input;
        Selector* m_output;
        Selector* m_alg;
        struct Geo6Dof::motion_pos m_pos;
};