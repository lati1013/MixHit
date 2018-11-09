#include "cMotor.h"
cMotor::cMotor()
{

}

cMotor::cMotor(int pPinPwm, int pPinDrehrichtung)
{
	mPinDrehrichtung = pPinDrehrichtung;
	mPinPwm = pPinPwm;

	pinMode(mPinDrehrichtung, OUTPUT);
	pinMode(mPinPwm, OUTPUT);

	ledcSetup(CHANNEL, PWM_FREQUENCY, RESOLUTION);    //PWM Setup
	ledcAttachPin(mPinPwm, CHANNEL);                //Channelzuweisung zu PWM Pin

	MotorStop();
}

void cMotor::MotorStartR()
{
	digitalWrite(mPinDrehrichtung, HIGH);                   //Drehrichtung bestimmen
	for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)   //Motor Anfahrrampe rechtslauf
	{
		ledcWrite(CHANNEL, dutyCycle);
		delayMicroseconds(DelayAnlauframpe);
	}

}

void cMotor::MotorStartL()
{
	digitalWrite(mPinDrehrichtung, LOW);                    //Drehrichtung bestimmen
	for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)   //Motor Anfahrrampe linkslauf
	{
		ledcWrite(CHANNEL, dutyCycle);
		delayMicroseconds(DelayAnlauframpe);
	}
}

void cMotor::MotorStop()
{
	for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)   //Motor Abbremsrampe
	{
		ledcWrite(CHANNEL, dutyCycle);
		delayMicroseconds(DelayAnlauframpe);
	}
}
