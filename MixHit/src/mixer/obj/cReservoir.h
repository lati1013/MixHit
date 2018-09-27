#ifndef _CVORRATSBEHAELTER_H_
#define _CVORRATSBEHAELTER_H_

#include "Arduino.h"
#include "../Configuration.h"
#include "../../libs/MyString.h"

class cReservoir
{
public:
	cReservoir();												// Standartkonstruktor
	cReservoir(String pNames[MaxNumberOfReservoir], int pNumberOfUsedReservoir);	// Konstruktor mit Namen
	bool addReservoir(String pName);							// Fuegt ein Vorratsbehaelter hinzu (ohne Parameter zur Berechung von Maeche in Zeit).
	bool addReservoir(String pName, double pm, double pb);		// Fuegt ein Vorratsbehaelter hinzu (mit Parameter).
	void setParams(double pm[], double pb[]);					// Setzt alle Parameter fuer die umrechnung von Maenge in Zeit.
	String getReservoirName(int pIndex);						// Gibt den Namen des Vorrates an der gegebenen Stelle zurueck.
	int getReservoirIndex(String pName);						// Gibt den Index des Vorrates mit dem angegebenen Namen zurueck.
	int getNumberOfReservoir();									// Gibt die Anzahl an Verwendeten Vorratsgehaelter zurueck.
	int AmountToTime(int pIndex, int pml);						// Gibt die Oeffungszeit des Ventiles fuer die angegebene Maenge zurueck.
	void setMB(int pIndex, double pm, double pb);				// Set die Parameter fur die Umrechnung von Maenge in Zeit fuer den angegebenen Vorratsbehaelter.
	bool getInitState(int pIndex);								// Gibt an, ob der Vorratsbehaelter Initialisiert ist oder nicht.
	bool getInitStateAll();										// Gibt an, ob jeder Vorratsbehaelter initialisiert ist.
	void addToSum(int pIndex, int pAmount);						// Fuegt eine Maenge zur gesamten ausgeschenkten Maenge eines Vorrates seit Programmstart hinzu (Angabe in ml).
	double getSum(int pIndex);									// Gibt die gesamte ausgeschenkte Maenge des angegebenen Vorrates seit Programmstart zurueck (Angabe in ml).
	double getM(int pIndex);									// Gibt die Steigung des Lienearen zusammenhangs zwischen Maenge und Zeit zurueck.
	double getB(int pIndex);									// Gibt den Offset des Lienearen zusammenhangs zwischen Maenge und Zeit zurueck.
private:
	String mNames[MaxNumberOfReservoir];						// Namen der Vorratsbehaelter
	int mNumberOfReservoir;										// Anzahl an verwendeten Vorratsbehalter.
	//bool mInitOK[MaxNumberOfReservoir];							// Initialisierungszustaende der Vorratsbehaleter
	double mm[MaxNumberOfReservoir];							// Steigung; Für die umrechnung von cl bzw. g in die Öffungszeig der Valve.
	double mb[MaxNumberOfReservoir];							// Offset; Für die umrechnung von cl bzw. g in die Öffungszeig der Valve.
	int mSumServed[MaxNumberOfReservoir];						// Gesamte ausgeschenkte Maenge des angegebenen Vorrates seit Programmstart zurueck (Angabe in ml).
};

#endif
