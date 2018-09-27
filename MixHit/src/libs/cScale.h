#ifndef _CWAAGE_H_
#define _CWAAGE_H_

#include "HX711.h"
#include "../mixer/Configuration.h"

class cScale
{
public:
	cScale();						// Standartkonstruktor
	cScale(byte pPinScaleData, byte pPinScaleCLK); // Konstruktor mit angabe der Pins.
	double getWeight();				// Gibt das Gewicht zurueck (in g).
	double getWeight(int pPoints);	// pPoints ist die ANzahl an Messwerten wlche zur Mittelwertbildung verwendet wird. Gibt das Gewicht zurueck (in g).				
	void Tare();
private:
	HX711 mHX711Scale;				// Waage
	double mTare;
};

#endif
