#include "cValveControl.h"

cValveControl::cValveControl()
{
	Init_SparkFun();
	for (int i = 0; i < MaxNumberOfReservoir; i++)
	{
		mValveStates[i] = false;
		mValvePinNumbers[i] = PinSX1509Valve[i];
		io.pinMode(mValvePinNumbers[i], OUTPUT);
		io.digitalWrite(mValvePinNumbers[i], LOW);
	}
	mFastMode = true; // Damit mehre Ventile gleichzeitig geoeffnet werden duerfen.
}
void cValveControl::setFastMode(bool pMode)
{
	mFastMode = pMode;
}
void cValveControl::setValveState(int pValveIndex, bool pState)
{
	MyMutex_I2C_lock(); // I2C fuer andere Threads sperren (da das OLED ebenfalls I2C verwendet).
	io.digitalWrite(mValvePinNumbers[pValveIndex], pState ? HIGH : LOW); // Ventil-Pin setzen / ruecksetzen.
	MyMutex_I2C_unlock(); // I2C freigeben
	mValveStates[pValveIndex] = pState; // Ventilstatus intern speichern.

}
void cValveControl::setValveState(int pValveIndices[], long pTime[], int pNumberOfIngredients)
{
	Serial.println(pNumberOfIngredients);
	int lNumberOfIngredientsDone = 0; // Anzahl an bereits erledigten Zutaten.
	int lNumberOfOpenedValves = 0; // Anzahl an gleichzeitig offenen Ventielen
	int lMaxlNumberOfOpenedValves = mFastMode ? MaxNumberOfOpenedValve_FastMode : 1; // Festlegen wieviele Ventiele maximal gleichzeitig offen sein duerfen
	int lIndex = 0; // Aktueller angefragter Index.
	int* lIngredientsDone = new int[MaxNumberOfReservoir]; // Array in dem Informationen bezueglich dem Vortschritt der Zutat hinterlegt ist. (0 = Noch nicht begonnen, 1 = gerade geoeffnet, 2 = fertig)
	unsigned long* StartTimes = new unsigned long[MaxNumberOfReservoir]; // Speichert die Zeiten, an denen die jeweiligen Ventiele geoeffnet wurden (damit man weiss, wann das Ventil wieder geschlossen werden muss).
	for (int i = 0; i < pNumberOfIngredients; i++)
	{
		lIngredientsDone[i] = 0; // Fortschritt aller Ventile auf 0 setzen.
		StartTimes[i] = 0; // Startzeiten auf 0 setzen
		if (pTime[i] < 0) // Pruefen ob faeleschlicherweise eine negative Zeit uebergeben wurde
		{ // Ist kleiner als 0
			pTime[i] = 0; // Zeit auf 0 setzen
		}

	}
	unsigned long LastTimeVentilOpened = 0; // Gibt an, zu welchem Zeitpunkt zuletzt ein Ventil geoeffnet wurde (damit die Ventile nicht zu schnell hintereinander oeffnen und keine zu hohen Stromspitzen entstehen).
	while (lNumberOfIngredientsDone < pNumberOfIngredients) // Solange bis alle benoetigen Zutaten erledigt sind.
	{
		if (!(CheckNormalMode() || CheckInitMode())) // Pruefen ob sich die Maschiene in einem falschen Zustand befindet.
		{
			// Bei einem falschen Zustand werden alle Ventiele geschlossen und die Funktion verlassen.
			for (int i = 0; i < pNumberOfIngredients; i++)
			{
				setValveState(i, false);
			}
			delay(1000);
			return; 
		}
		if (lIngredientsDone[lIndex] == 0 && lNumberOfOpenedValves < lMaxlNumberOfOpenedValves) 
		{ // Falls die Zutat noch nicht begonnen wurde und die Maximale anzahl an gleichzeitig offenen Ventielen noch nicht erreicht wurde.
			if (millis() - LastTimeVentilOpened > MinTimeBetwenVentilOpen) // Pruefen ob zwischen dem letztn Oeffnen und der jetzigen anfrage die minimale Zeitdifferenz vergangen ist. Damit die Stromausnahme nicht auf einmal zu groß wird.
			{
				setValveState(pValveIndices[lIndex], true); // Ventil oeffnen
				StartTimes[lIndex] = millis(); // Oeffnungszeit des Ventils merken
				LastTimeVentilOpened = StartTimes[lIndex]; // Letzte Oeffnungszeit eines Ventiles merken.
				lIngredientsDone[lIndex] = 1; // Zustand der Zutat auf 1 setztn (gerade geoeffnet)
				lNumberOfOpenedValves++; // Anzahl an aktuell geoeffneten Ventielen um eins erhoehen.
				
				Serial.print(String(lIndex) + ": Start: ");
				Serial.print(lIndex);
				Serial.print(" - Soll_Zeit: ");
				Serial.println(pTime[lIndex]);
			}
		}

		if (lIngredientsDone[lIndex] == 1 && (millis() - StartTimes[lIndex]) >= pTime[lIndex]) // Falls das Ventil geoeffnet ist und die benoetige Oeffnungszeit erreicht wurde
		{
			setValveState(pValveIndices[lIndex], false); // Ventil schliessen
			lIngredientsDone[lIndex] = 2; // Zustand auf 2 setzrn (Zutat erledigt)
			lNumberOfIngredientsDone++; // Anzahl an erledigten Zutaten um eins erhoehen.
			lNumberOfOpenedValves--; // Anzahl an derzeit offenen Ventielen um eins veringern.

			Serial.print(String(lIndex) + ": Ende: ");
			Serial.print(lIndex);
			Serial.print(" -  Ist_Zeit_Differenz: ");
			Serial.println(pTime[lIndex] - (millis() - StartTimes[lIndex] - 1));
		}
		delay(1);
		lIndex = (lIndex + 1) % pNumberOfIngredients; // lIndex um eins erhoehen (Falls lIndex >= pNumberOfIngredients --> lIndex = 0)
	}
}
