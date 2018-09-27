#include "cServoMotor.h"

cServoMotor::cServoMotor(){}
cServoMotor::cServoMotor(int pPinNummber)
{
	mPinNumber = pPinNummber;
	mServo.attach(mPinNumber, 700, 2300);
}
void cServoMotor::goToPosition_Close()
{
	mServo.write(49); // Eigentlich ist hier die angabe in °. Allerdings hat der verwendete Servo eine andere Uebersetzung. 10° im Programmcode entsprechen tatsaechlich nur wenige Grad (<10°). 
}
void cServoMotor::goToPosition_Open()
{
	mServo.write(45); // Eigentlich ist hier die angabe in °. Allerdings hat der verwendete Servo eine andere Uebersetzung. 10° im Programmcode entsprechen tatsaechlich nur wenige Grad (<10°). 
}