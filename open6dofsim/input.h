#pragma once
#include "selector.h"
class Input : public Selectable {
public:
	Input() {

	}
	virtual ~Input() {

	}
	virtual int ReadNext(double* Faa, double* Oaa) = 0;
	virtual QString& getStatus() = 0;
};