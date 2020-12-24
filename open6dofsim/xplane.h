#pragma once

#include "qcoreapplication.h"
#include <QUdpSocket>
#include <QTimer>
#include "input.h"
#include "selector.h"

#define BUFLEN 1024



class XPlane : public QObject, public Input {
	Q_OBJECT

public:
	XPlane(QObject *parent);
	int ReadNext(double* Faa, double* Oaa);
	QString& getStatus();
	~XPlane();
	QString& name() {
		static QString n("XPlane UDP");
		return n;
	}
	void setActive();
	void setInactive();

public slots:
	void readyRead();
	void handleTimeout();

private:
	typedef enum { PAUSED, WAITING, RECEIVING } state_t;

	QTimer *m_timer;
	int register_all();
	int register_dataref(char* name, int nr, int freq);
	int calculate_accel();
	int m_paused;
	float m_theta;
	float m_psi;
	float m_phi;
	float m_a_side;
	float m_a_nrml;
	float m_a_axil;
	int m_run;
	QUdpSocket *m_s;
	char* m_server;
	int m_sample;
	char m_buf[BUFLEN];
	state_t m_state;
	bool m_enabled;
};
