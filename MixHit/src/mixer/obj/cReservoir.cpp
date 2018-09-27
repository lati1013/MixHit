#include "cReservoir.h"

cReservoir::cReservoir()
{
	mNumberOfReservoir = 0;
	for (int i = 0; i<MaxNumberOfReservoir; i++)
	{
		mNames[i] = "";
		mm[i] = 0;
		mb[i] = 0;
		mSumServed[i] = 0;
	}
}
cReservoir::cReservoir(String pNames[], int pNumberOfReservoir)
{
	mNumberOfReservoir = pNumberOfReservoir < MaxNumberOfReservoir ? pNumberOfReservoir : MaxNumberOfReservoir;
	for (int i = 0; i<mNumberOfReservoir; i++)
	{
		mNames[i] = pNames[i];
		mm[i] = 0;
		mb[i] = 0;
		mSumServed[i] = 0;
	}
}
bool cReservoir::addReservoir(String pName)
{
	if (mNumberOfReservoir <= MaxNumberOfReservoir) // Pruefen ob die Maximale Anzahl an Vorratsbehaelter bereits erreicht ist.
	{
		mNames[mNumberOfReservoir] = pName; // Name uebergeben
		mm[mNumberOfReservoir] = 0; // Parameter auf 0 setzen (entspricht nicht initielisiert)
		mb[mNumberOfReservoir] = 0; // Parameter auf 0 setzen (entspricht nicht initielisiert)
		mSumServed[mNumberOfReservoir] = 0; // Summe der ausgeschenkten Maenge auf 0ml setzen
		mNumberOfReservoir++; // Anzahl an verwendeten Vorratsbehaelter um eins erhoehen.
		return true;
	}
	else
	{
		return false;
	}	
}
bool cReservoir::addReservoir(String pName, double pm, double pb)
{
	if (mNumberOfReservoir <= MaxNumberOfReservoir) // Pruefen ob die Maximale Anzahl an Vorratsbehaelter bereits erreicht ist.
	{
		mNames[mNumberOfReservoir] = pName; // Name uebergeben
		mm[mNumberOfReservoir] = pm; // Parameter m festlegen
		mb[mNumberOfReservoir] = pb; // Parameter b festlegen
		mSumServed[mNumberOfReservoir] = 0; // Summe der ausgeschenkten Maenge auf 0ml setzen
		mNumberOfReservoir++; // Anzahl an verwendeten Vorratsbehaelter um eins erhoehen.
		return true;
	}
	else
	{
		return false;
	}
}
void cReservoir::setParams(double pm[], double pb[])
{
	for (int i = 0; i < mNumberOfReservoir; i++)
	{
		mm[i] = pm[i]; 
		mb[i] = pb[i];
	}
}
void cReservoir::setMB(int pIndex, double pm, double pb)
{
	mm[pIndex] = pm;
	mb[pIndex] = pb;
}
void cReservoir::addToSum(int pIndex, int pAmount)
{
	mSumServed[pIndex] = mSumServed[pIndex] + pAmount; // Gesamte ausgeschenkte Menge dieses Vorrates um die angegebene Maenge in ml (pAmount) erhoehen.
}
double cReservoir::getSum(int pIndex)
{
	return ((double)mSumServed[pIndex]) / 1000; // Gibt die gesamte ausgeschenkte Summe des Vorratsbehaelters (in L, daher division mit 1000) aus.
}
String cReservoir::getReservoirName(int pIndex)
{
	return mNames[pIndex];
}
int cReservoir::getReservoirIndex(String pName)
{
	for (int i = 0; i<mNumberOfReservoir; i++)
	{
		if(mNames[i] == pName) // Prueft, ob der Vorratsbehaelter an dieser Stelle den gesuchten Namen (pName) hat.
		{
			return i; // Falls dies der Fall ist, wird der Index dieses Vorratsbehaelters uebergeben.
		}
	}
	return -1; // falls keine Uebereinstimmung gefunden wurde.
}
int cReservoir::getNumberOfReservoir()
{
	return mNumberOfReservoir;
}
int cReservoir::AmountToTime(int pIndex, int pml)
{
	Serial.print("m=");
	Serial.print(mm[pIndex]);
	Serial.print(" b=");
	Serial.print(mb[pIndex]);
	Serial.print(" Time=");
	Serial.println((double)pml * mm[pIndex] + mb[pIndex]);
	return (double)pml * mm[pIndex] + mb[pIndex]; // Linearer Zusammenhang: Zeit (Einheit: ms) = Wunschmaenge (Einheit: ml) * Steigung (Einheit: ms / ml) + Offset (Einheit: ms); Wobei davon ausgegangen wird, dass ein Milliliter einem Gramm entsprincht.
}

bool cReservoir::getInitState(int pIndex)
{
	Serial.println("m=" + String(mm[pIndex]) + " b=" + String(mb[pIndex]));
	Serial.println("InitOK_" + String(pIndex) + " = " + String((mm[pIndex] != 0 || mb[pIndex] != 0)?"0":"1"));
	return mm[pIndex] != 0 || mb[pIndex] != 0;
}
bool cReservoir::getInitStateAll()
{
	for (int i = 0; i < mNumberOfReservoir; i++)
	{
		if(getInitState(i) == false)
		{ // Falls ein Vorratsbehaelter gefunden wird, der noch nicht initialisiert ist.
			return false; // Funktion verlassen und false zurueckgeben.
		}
	}
	// Falls alle Vorratsbehaelter initialisiert sind
	return true;
}
double cReservoir::getM(int pIndex)
{
	return mm[pIndex];
}
double cReservoir::getB(int pIndex)
{
	return mb[pIndex];
}