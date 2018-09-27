#ifndef _CMAGNETSENSOR_H_
#define _CMAGNETSENSOR_H_

#include "Arduino.h"
#include "../mixer/Configuration.h"
#include "I2C.h"


class cMagnetSensor
{
public:
	cMagnetSensor();					// Standartkonstruktor mit default_Pinnummer.
	cMagnetSensor(int pPinNumber);		// Konstruktor mit Pinnangabe.
	int getSignal();					// Gibt das Signal des Magnetsensors aus.
private:
	int mPinNumber;						// PinNummer an dem Der Magnetsensor angeschlossen ist.
};

#endif