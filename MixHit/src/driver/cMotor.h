#ifndef _CMOTOR_H_
#define _CMOTOR_H_

#include "Arduino.h"
#include "Wire.h"   
#include "../mixer/Configuration.h"

class cMotor
{
public:
	cMotor();											// Standartkonstruktor mit default_Pinnummer
	cMotor(int pPinNumberMotor, int pPinNumberRPM);		// Konstruktor mit Pinnummer angabe											
	void MotorStartR();									// Startet den Motor (konstante Drehzahl und konstante Drehrichtung)
	void MotorStartL();									// Startet den Motor (konstante Drehzahl und konstante Drehrichtung)
	void MotorStop();									// Stoppt den Motor.
private:
	int mPinNumberMotor;								// Pinnummer des Motors ???
	int mPinNumberRPM;									// Pinnummer zur auswahl der Drehrichtung???
};

#endif