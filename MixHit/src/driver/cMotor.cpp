#include "cMotor.h"
cMotor::cMotor(){}
cMotor::cMotor(int pPinNumberMotor, int pPinNumberRPM)
{
	mPinNumberMotor = pPinNumberMotor;
	mPinNumberRPM = pPinNumberRPM;
	pinMode(mPinNumberMotor, OUTPUT);
	pinMode(mPinNumberRPM, OUTPUT);
	
	MotorStop();
}
void cMotor::MotorStartR()
{
	digitalWrite(mPinNumberRPM, LOW);
	digitalWrite(mPinNumberMotor, HIGH);
}
void cMotor::MotorStartL()
{
	digitalWrite(mPinNumberRPM, HIGH);
	digitalWrite(mPinNumberMotor, HIGH);
}
void cMotor::MotorStop()
{
	digitalWrite(mPinNumberRPM, LOW);
	digitalWrite(mPinNumberMotor, LOW);
}
