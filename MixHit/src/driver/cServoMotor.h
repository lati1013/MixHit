#ifndef _CSERVOMOTOR_H_
#define _CSERVOMOTOR_H_

#include <ESP32_Servo.h>
#include "../mixer/Configuration.h"

class cServoMotor
{
public:
	cServoMotor();
	cServoMotor(int pServoPin);
	void Aktivieren();
	void Deaktivieren();
private:
	int mPinNummer;
	Servo mServo;
};

#endif