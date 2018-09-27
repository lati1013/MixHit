#ifndef _CSERVOMOTOR_H_
#define _CSERVOMOTOR_H_

#include <ESP32_Servo.h>
#include "../mixer/Configuration.h"

class cServoMotor
{
public:
	cServoMotor();
	cServoMotor(int pPinNummber);
	void goToPosition_Close();
	void goToPosition_Open();
private:
	int mPinNumber;
	Servo mServo;
};

#endif