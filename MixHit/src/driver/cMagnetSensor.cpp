#include "cMagnetSensor.h"
cMagnetSensor::cMagnetSensor(){}
cMagnetSensor::cMagnetSensor(int pPinNumber)
{
	mPinNumber = pPinNumber;
	pinMode(mPinNumber, INPUT);
}
int cMagnetSensor::getSignal()
{
	return digitalRead(mPinNumber) != 0 ? 1 : 0; // Invertieren des Signals (1=Magnet vorhanden, 0=kein Magnet vorhanden)
}
