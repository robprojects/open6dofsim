#pragma once
#include "qstring.h"
#include "selector.h"
class Output : public Selectable {
public:
	Output() {

	}
	virtual ~Output() {

	}
	virtual void send_serial_command(int* len) = 0;
	virtual QString& getStatus() = 0;
};
