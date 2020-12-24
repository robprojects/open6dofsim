#include "manual.h"

Manual::Manual(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	connect(ui.right, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
	connect(ui.high, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
	connect(ui.forward, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
	connect(ui.roll, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
	connect(ui.pitch, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
	connect(ui.yaw, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));

	m_pos.theta = 0.0;
	m_pos.phi = 0.0;
	m_pos.psi = 0.0;
	m_pos.T[0] = m_pos.T[1] = m_pos.T[2] = 0.0;
}

void Manual::sliderMoved(int value) {
	m_pos.T[0] = (((double)(ui.right->value()-50))/100.0) *0.5;
	m_pos.T[1] = (((double)(ui.forward->value() - 50)) / 100.0) * 0.5;
	m_pos.T[2] = (((double)(ui.high->value() - 50)) / 100.0) * 0.5;
	m_pos.theta = (((double)(ui.roll->value() - 50)) / 100.0) * M_PI / 4;
	m_pos.phi = (((double)(ui.pitch->value() - 50)) / 100.0) * M_PI / 4;
	m_pos.psi = (((double)(ui.yaw->value() - 50)) / 100.0) * M_PI / 4;
}

int Manual::compute2(double* faa, double* oaa, struct Geo6Dof::motion_pos* pos) {
	*pos = m_pos;

	return 0;
 }

void Manual::openConfigDialog() {
	show();
}

