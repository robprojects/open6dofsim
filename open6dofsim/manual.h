#pragma once
#include "qdialog.h"
#include "ui_configManual.h"
#include "geo6dof.h"
#include "algorithm.h"

class Manual : public QDialog, public Algorithm {
	Q_OBJECT

public:
	Manual(QWidget* parent);
	int compute2(double* faa, double* oaa, struct Geo6Dof::motion_pos* pos);
	void openConfigDialog();

public slots:
	void sliderMoved(int value);

	QString& name() {
		static QString n("Manual position");
		return n;
	}

	QString& getStatus() {
		static QString status("");
		return status;
	}
private:
	Ui::configManual ui;
	struct Geo6Dof::motion_pos m_pos;
};