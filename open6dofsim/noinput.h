#pragma once
#include "input.h"
#include "selector.h"

class NoInput : public Input {
public:
	int ReadNext(double* Faa, double* Oaa) {
		Faa[0] = 0.0; Faa[1] = 0.0; Faa[2] = 9.8;
		Oaa[0] = 0.0; Oaa[1] = 0.0; Oaa[2] = 0.0;
		return 1;
	}
	QString& getStatus() {
		static QString status("");
		return status;
	}
	QString& name() {
		static QString n("None");
		return n;
	}
};