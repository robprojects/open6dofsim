#pragma once

#include "motion.h"
#include "qvector3d.h"
#include "qdialog.h"
#include "qwidget.h"
#include "ui_configPlatform.h"



class Geo6Dof : public QDialog {
	Q_OBJECT
public:
	Geo6Dof(QWidget *parent);

	struct display_pose {
		QVector3D platform[6];
		QVector3D base[6];
		QVector3D p[6];
		QVector3D b[6];
	};

	struct motion_pos {
		double psi; // yaw
		double theta; // pitch
		double phi; // roll
		double T[3]; // translation vector x,y,z
	};

	void getPose(struct display_pose *pose) {
		int i;
		for (i = 0; i < 6; i++) {
			pose->platform[i] = getPlatformVector(i);
			pose->base[i] = getBaseVector(i);
			pose->p[i] = getActuatorVectorP(i);
			pose->b[i] = getActuatorVectorB(i);
		}
	}

	void getLengths(int* len);

	void resetPos();
	void setPos(struct motion_pos* pos);
	void setStoppedPos(int stop_height);
	void openConfigDialog();

public slots:
	void clickOK();

private:
	struct geo6dofconfig {
		double radius_base;
		double radius_platform;
		double mid_length;
		double min_length;
		double range;
		double sep_angle;
		double sep_angle_platform;
	};

	struct motion_state {
		double length[6];
		double p[6][3]; // transformed anchor point
		double platform[6][3]; // transformed platform 
		double ball[6];
		double cardan[6];
	};

	struct motion_geo {
		double p[6][3]; // upper anchor point x,y,z relative to platform origin
		double b[6][3]; // lower anchor point x,y,z relative to base origin
		double base[6][3]; // coordinates of simualtor base
		double platform[6][3]; // coordinates of simulator platform
		double mid_height;
		double min_height;
		double act_min;
		double act_range;
	};

	struct motion_geo m_geo;
	struct motion_pos m_pos;
	struct motion_state m_state;
	struct geo6dofconfig m_config;

	struct motion_pos m_stopped_pos;

	int init_geometry(struct motion_geo* geo, double radius_base, double radius_platform, double mid_length, double min_length, double range, double sep_angle, double sep_angle_platform);
	int inverse_kinematics(struct motion_geo* geo, struct motion_pos* pos, struct motion_state* st);

	QVector3D getPlatformVector(int i) {
		return QVector3D(m_state.platform[i][0], m_state.platform[i][2], m_state.platform[i][1]);
	}

	QVector3D getBaseVector(int i) {
		return QVector3D(m_geo.base[i][0], m_geo.base[i][2], m_geo.base[i][1]);
	}

	QVector3D getActuatorVectorP(int i) {
		return QVector3D(m_state.p[i][0], m_state.p[i][2], m_state.p[i][1]);
	}

	QVector3D getActuatorVectorB(int i) {
		return QVector3D(m_geo.b[i][0], m_geo.b[i][2], m_geo.b[i][1]);
	}

	Ui::configPlatform ui;

};



