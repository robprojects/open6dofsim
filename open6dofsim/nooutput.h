#pragma once
#include "output.h"
#include "selector.h"

class NoOutput : public Output {
public:
	void send_serial_command(int* len) {

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