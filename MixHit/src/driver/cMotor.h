#ifndef _CMOTOR_H_
#define _CMOTOR_H_

#include "Arduino.h"
#include "esp32-hal-ledc.h" 
#include "../mixer/Configuration.h"

class cMotor
{
public:
	cMotor();											// Standartkonstruktor mit default_Pinnummer
	cMotor(int pPinPwm, int pPinDrehrichtung);			// Konstruktor mit Pinnummer angabe											
	void MotorStartR();									// Startet den Motor (konstante Drehzahl und konstante Drehrichtung)
	void MotorStartL();									// Startet den Motor (konstante Drehzahl und konstante Drehrichtung)
	void MotorStop();									// Stoppt den Motor.
private:
	int mPinDrehrichtung;								// Pinnummer des Motors ???
	int mPinPwm;										// Pinnummer zur auswahl der Drehrichtung???
};

#endif