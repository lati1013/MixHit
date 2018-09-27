#ifndef _CVENTILSTEUERUNG_H_
#define _CVENTILSTEUERUNG_H_

#include "../mixer/Configuration.h"
#include "I2C.h"

class cValveControl
{
public:
	cValveControl();										// Standartkonstruktor				
	void setFastMode(bool pMode);							// Setzt den Modus (eines / mehrere Vebtiele gleichzeitig offen).
	void setValveState(int pValveIndex, bool pState);		// Oeffnet / Schliesst das ngegebene Ventiel.
	void setValveState(int pValveIndices[], long pTime[], int pNumberOfIngredients);	// Offner / Schliesst die angegebenen Ventiele.
private:
	int mValvePinNumbers[MaxNumberOfReservoir];				// Pinnummern der Ventiele
	bool mValveStates[MaxNumberOfReservoir];				// Zustaende der Ventiele
	bool mFastMode;											// Modus (eines / mehrere Vebtiele gleichzeitig offen)
};

#endif
