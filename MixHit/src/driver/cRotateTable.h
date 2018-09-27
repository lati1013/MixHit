#ifndef _CDREHTELLER_H_
#define _CDREHTELLER_H_

#include "Arduino.h"
#include "../mixer/Configuration.h"
#include "cMagnetSensor.h"
#include "cMotor.h"
#include "cOLED.h"

class cRotateTable
{
public:
	cRotateTable();										// Standartkonstruktor
	bool goToFirstPosition();							// Sucht die Position 0. (AN der Position 0 sind zwei Magnete hintereinander angebracht).
	bool goToPosition(int pPosition);					// Geht an die angegebene Position.
	bool goToNextPosition();							// Dreht den Drehteller an die naechste Position.
	int getPosition();									// Gibt die aktuelle Position des Drehtellers zurueck.
	//bool risingEdge(bool pSignal, bool* pOldSignal);	// Gibt true bei einem Wechsel von false nach true zurück.
	//bool fallingEdge(bool pSignal, bool* pOldSignal);	// Gibt true bei einem Wechsel von false nach true zurück.
	cMotor mMotor;										// Motor welcher den Drehteller bewegt.
	cMagnetSensor mMagnetSensor;						// Magnetsensor welcher zur Positionserkennung verwendet wird.
private:
	bool getMagnetSensorSignal();						// Gibt das Signal des Magnetsensors aus.
	int mCurrentPosition;								// Aktuelle Position des Drehtellers.	
	unsigned long mTimeToNextSlot;						// Zeit in ms welche der Drehteller fuer eine ganze runde braeuchte (im Dauerlauf ohne Unterbrechung)
};
bool risingEdge(bool pSignal, bool* pOldSignal);	// Gibt true bei einem Wechsel von false nach true zurück.
bool fallingEdge(bool pSignal, bool* pOldSignal);	// Gibt true bei einem Wechsel von false nach true zurück.

#endif