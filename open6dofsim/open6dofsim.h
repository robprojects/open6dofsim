#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_open6dofsim.h"

class open6dofsim : public QMainWindow
{
    Q_OBJECT

public:
    open6dofsim(QWidget *parent = Q_NULLPTR);

private:
    Ui::open6dofsimClass ui;
};
