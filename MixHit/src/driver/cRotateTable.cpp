#include "cRotateTable.h"

cRotateTable::cRotateTable()
{
	mMagnetSensor = cMagnetSensor(PinMagnetTaster);
	mMotor = cMotor(PinMotor, PinDrehrichtung);
	mCurrentPosition = -1; // -1 bedeutet, dass die Position nicht bekannt ist.
	mTimeToNextSlot = 9516;
}
bool cRotateTable::goToFirstPosition()
{ // Position 1 Suchen (Slot mit der Nummer 1 ist unter dem Ausschank). 
	Serial.println("Position 1 suchen");
	gOLED.PrintFirstLine("Position 1 suchen");
	mMotor.MotorStartR();
	unsigned long lStartTime = millis(); // Startzeit speichern (damit im Fehlerfall nach einer gewissen Zeit abgebrochen werden kann).
	unsigned long lMaxSearchTime = 120000; // 120sec = 2min - Zeit, nach der Abgebrochen wird falls die Position nicht gefunden wird.

	unsigned long lMaxDeltaTime = 2000; // An der Position 1 sind zwei Magnete hintereinander. Falls also diese beiden Flanken weniger als 1sec voneinander Entfernt sind, ist die Position gefunden.
	bool lOldSignal = getMagnetSensorSignal(); // Beim Start der Funktion wird das vorhergehende Signal des Magnetsensors als das aktuelle angenommen.
	unsigned long lLastTimerisingEdge = lOldSignal == true ? millis() : 0; // Dort wird nachfolgend die Zeit gespeichert, an der ein Uebergang von 0 auf 1 stattfindet. Falls das lOldSignal bereits true ist, wird die aktuelle Zeit angenommen, ansonsten 0.
	#ifndef REGION_Position_0_Finden
	Serial.println(lStartTime);
	while (millis() - lStartTime <= lMaxSearchTime && (CheckNormalMode() || CheckInitMode())) // Solange die Maximale Suchzeit nicht ueberschritten wurde und sich die Maschiene im richtigen Zustand befindet.
	{ // Es wird maximal "lMaxSearchTime" millisekunden nach der Position 1 gesucht. Dauert es länger liegt ein Problem vor.
		if (risingEdge(getMagnetSensorSignal(), &lOldSignal)) // Falls ein Uebergang des Magnetsensors von 0 auf 1 vorliegt (positive Flanke)
		{
			Serial.print("MagnetSensorSignal erkannt. Zeitdifferenz zum letzten Signal: ");
			Serial.print(millis() - lLastTimerisingEdge);
			Serial.println("ms");
			if ((millis() - lLastTimerisingEdge) <= lMaxDeltaTime) // Falls die beiden Flanken nahe genug beisammen liegen, ist die 0-Position (da dort zwei Magnete hintereinander sind) gefunden worden.
			{ // Wenn innerhalb der "lMaxDeltaTime" zwei steigende Flanken auftreten ist Position 0 erreicht (bei Position 0 sind zwei Magnete? in kurzem Abstand hintereinander angebracht)
				Serial.println("Position 1 gefunden.");
				gOLED.PrintFirstLine("Position 1 gefunden");
				mMotor.MotorStartL(); // Zurueck drehen damit Glas frei steht
				delay(200);
				mMotor.MotorStop();
				mCurrentPosition = 1; // Aktuelle Position auf 1 setzen
				return true; // Funktion verlassen
			}
			lLastTimerisingEdge = millis(); // Zeit der Flanke speichern.
		}
		delay(10); // 10ms warten um den Prozessor zu entlasten
	}
	#endif
	// Falls die Position nicht gefunden wurde oder die Schleife aufgrund des Maschienenstatuses verlassen wurde.
	Serial.println("Position 1 nicht gefunden");
	gOLED.PrintFirstLine("Position 1 nicht gefunden");
	setMachineState(MachineState_ERROR_Position_1_NichtGefunden);
	mMotor.MotorStop();
	return false;

}
bool cRotateTable::goToPosition(int pPosition)
{
	if (pPosition > NumberOfSlotsRotateTable || pPosition <= 0)
	{ // Falls eine unerreichbare Position angegeben wird
		return false;
	}
	while (mCurrentPosition != pPosition) // Solange die gesuchte Position nciht erreicht wurde
	{
		if (!goToNextPosition()) // Naechste Position anfahren.
		{ // Falls die naechste Position nicht gefunden werden kann.
			return false; // Verlassen der Funktion
		}
	}
	return true; // Position wurde gefunden.
}	
bool cRotateTable::goToNextPosition()
{
	if (mCurrentPosition == -1)
	{ // Falls noch keine Position bekannt ist wird nach der Position 1 gesucht.
		if (!goToFirstPosition())
		{ // Falls die Position 1 nicht gefunden wird
			return false;
		}
		else
		{
			return true;
		}
	}
	Serial.println("GoToNextPosition");
	mMotor.MotorStartR();
	unsigned long lStartTime = millis(); // Startzeit speichern (damit im Fehlerfall nach einer gewissen Zeit abgebrochen werden kann).
	unsigned long lMaxSearchTime = 30000; // 30sec - Zeit, nach der Abgebrochen wird falls die Position nicht gefunden wird.
	bool lOldSignal = getMagnetSensorSignal(); // Beim Start der Funktion wird das vorhergehende Signal des Magnetsensors als das aktuelle angenommen.
	unsigned long lMinDeltaTime = 2000; // An der Position 1 sind zwei Magnete hintereinander. Falls also diese beiden Flanken weniger als 1sec voneinander Entfernt sind, ist es immernoch die Position 1 und nicht bereits die naechste.
	while (millis() - lStartTime <= lMaxSearchTime && (CheckNormalMode() || CheckInitMode())) // Solange die Maximale Suchzeit nicht ueberschritten wurde und sich die Maschiene im richtigen Zustand befindet.
	{ // Es wird maximal "lMaxSearchTime" ms nach der naechsten Position gesucht. Dauert es laenger liegt ein Problem vor.

		if (risingEdge(getMagnetSensorSignal(), &lOldSignal)) // Falls ein Uebergang des Magnetsensors von 0 auf 1 vorliegt (positive Flanke)
		{
			if (millis() - lStartTime > lMinDeltaTime) // Mindestsuchzeit abwarten
			{
				Serial.println("FoundNextPosition");
				mCurrentPosition = (mCurrentPosition % NumberOfSlotsRotateTable) + 1; // Position um eins erhoehen (Werte: 1 ... NumberOfSlotsRotateTable)
				delay(700); // 0,5sec weiter drehen, damit die Position stimmt.
				mMotor.MotorStartL(); // Zurueck drehen damit Glas frei steht
				delay(200);
				mMotor.MotorStop();
				return true;
			}
		}
		delay(10); // Warten
	}
	// Falls die Position nicht gefunden wurde oder die Schleife aufgrund des Maschienenstatuses verlassen wurde.
	gOLED.PrintFirstLine("Position nicht gefunden");
	setMachineState(MachineState_ERROR_NaechstePositionNichtGefunden);
	mMotor.MotorStop();
	return false;
}
int cRotateTable::getPosition()
{
	return mCurrentPosition;
}
//bool cRotateTable::risingEdge(bool pSignal, bool* pOldSignal)
//{
//	if (*pOldSignal == false && pSignal == true) // Prueft ob ein Uebergang von 0 auf 1 vorliegt.
//	{ // Positive Flanke liegt vor.
//		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
//		return true;
//	}
//	else
//	{ // Keine positive Flanke
//		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
//		return false;
//	}
//}
//bool cRotateTable::fallingEdge(bool pSignal, bool* pOldSignal)
//{
//	if (*pOldSignal == false && pSignal == true) // Prueft ob ein Uebergang von 0 auf 1 vorliegt.
//	{ // Positive Flanke liegt vor.
//		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
//		return true;
//	}
//	else
//	{ // Keine positive Flanke
//		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
//		return false;
//	}
//}
bool risingEdge(bool pSignal, bool* pOldSignal)
{
	if (*pOldSignal == false && pSignal == true) // Prueft ob ein Uebergang von 0 auf 1 vorliegt.
	{ // Positive Flanke liegt vor.
		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
		return true;
	}
	else
	{ // Keine positive Flanke
		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
		return false;
	}
}
bool fallingEdge(bool pSignal, bool* pOldSignal)
{
	if (*pOldSignal == true && pSignal == false) // Prueft ob ein Uebergang von 0 auf 1 vorliegt.
	{ // Positive Flanke liegt vor.
		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
		return true;
	}
	else
	{ // Keine positive Flanke
		*pOldSignal = pSignal; // Das alte Signal ueberschreiben, da das aktuelle Signal beim naechsten Aufruf das alte ist.
		return false;
	}
}
bool cRotateTable::getMagnetSensorSignal()
{
	return mMagnetSensor.getSignal();
}