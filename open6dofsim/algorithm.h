#pragma once
#include "qstring.h"
#include "selector.h"
#include "geo6dof.h"

class Algorithm : public Selectable {
public:
	Algorithm() {

	}
	virtual ~Algorithm() {

	}
	virtual int compute2(double* faa, double* oaa, struct Geo6Dof::motion_pos* pos) = 0;
	void openConfigDialog() {

	}
};