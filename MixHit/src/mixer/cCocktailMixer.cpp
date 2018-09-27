#include "cCocktailMixer.h"
#include "../driver/cOLED.h"



cCocktailMixer::cCocktailMixer()
{ // Initialisieren aller Werte
	for (int i = 0; i < NumberOfSlotsRotateTable; i++)
	{
		mSlotOrderNumber[i] = 0;
		mSlotCocktailName[i] = "";
	}
	mAmountOfMixedCocktails = 0;
	mQueue[0] = cQueueOfOrders(50);
	mQueue[1] = cQueueOfOrders(20);
	mQueue[2] = cQueueOfOrders(5);
	mCurrentOrderNumber = 0;
	mNextOrderNumber = 0;
	mQueueSelectCounter = 0;
	mServo = cServoMotor(PinServoPWM);
	mScale = cScale(PinScaleData, PinScaleCLK);
}
int cCocktailMixer::addOrderToQueue(cOrder pBestellung)
{
	MyMutex_mQueue_lock();		// Bereich fuer andere Threads sperren
	int lOrderNumber = mNextOrderNumber + 1;	// int lOrderNumber = mNextOrderNumber + 100;
	pBestellung.setOrderNumber(lOrderNumber);
	mNextOrderNumber++;			// Naechste moegliche Bestellnummer berechnen.
	bool ReturnValue = mQueue[pBestellung.getPrio()].addOrder(pBestellung); // Bestellung ggf. der Warteschlange hinzufuegen (false, falls Liste bereits voll).
	MyMutex_mQueue_unlock();	// Bereich wieder freigeben
	if (ReturnValue)
	{
		return lOrderNumber;			// Rueckmelden der Bestellnummer
	}
	else
	{
		return -1; // Fehlermeldung
	}
}
int cCocktailMixer::getNumberOfOrdersInQueue(int pPrio)
{
	if (pPrio == 2) // Die Warteschlange mit hoechster Prio wird vor allen anderen abgearbeitet.
	{
		return mQueue[2].getNumberOfOrders(); // Die Anzahl an Cocktails vor einer neuen Bestellung sind nur die Cocktails welche bisher in der Lise der hoechsten Prio sind.
	}
	int lNumberOfOrdersBefore = 0;  // Anzahl an Bestellungen (in den bisherigen Warteschlangen) auf 0 initialisieren.
	if (pPrio == 1) // Mittlere Prio
	{
		lNumberOfOrdersBefore = mQueue[2].getNumberOfOrders() + mQueue[1].getNumberOfOrders(); // Hier kommen alle Cocktails der hoechsten Prio vorher sowie die bisherigen Cocktails in der mittleren Prio-Liste
		// Da sich die mittlere und niedrigste Prio abwechseln (3x mittlere, 1x niedrigste), kommen ggf. auch Cocktails aus der niedrigsten Liste vorher dran.
		if (mQueue[1].getNumberOfOrders() / 3 < mQueue[0].getNumberOfOrders())
		{
			// Beispiel: Liste_1: 7, Liste_0: 12
			// 7 / 3 = 2 (Nachkommastellen werden abgeschnitten).
			// 7 % 2 = 1
			// Es sind 12 Cocktails in der Liste_0,
			// es kommen davon aber nur 2 Cocktails vorher dran.
			// Ebenfalls wird berücksichtigt, wo der Counter aktuell steht. Jenchdem kommt noch ein weiterer Cocktail zuvor.
			int lCocktailsFrom0 = mQueue[1].getNumberOfOrders() / 3;
			lCocktailsFrom0 = lCocktailsFrom0 + (mQueue[1].getNumberOfOrders() % 3 < (3 - mQueueSelectCounter) ? 0 : 1);
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + lCocktailsFrom0;
		}
		else
		{
			// Beispiel: Liste_1: 11, Liste_0: 2
			// 11 / 3 = 3 (Nachkommastellen werden abgeschnitten).
			// Es wuerden 3 Cocktails vorher dran kommen, 
			// es sind allerdings nur 2 in der Liste.
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + mQueue[0].getNumberOfOrders();
		}
		return lNumberOfOrdersBefore;
	}
	if (pPrio == 0)
	{
		lNumberOfOrdersBefore = mQueue[2].getNumberOfOrders() + mQueue[0].getNumberOfOrders();
		// Da sich die mittlere und niedrigste Prio abwechseln (3x mittlere, 1x niedrigste), kommen ggf. auch Cocktails aus der niedrigsten Liste vorher dran.
		if ((mQueue[0].getNumberOfOrders() * 3 + (3 - mQueueSelectCounter)) < mQueue[1].getNumberOfOrders())
		{
			// Beispiel: Liste_1: 11, Liste_0: 2
			// 2 * 3 = 6
			// Es sind 11 Cocktails in der Liste_1,
			// davon kommen aber nur 6 vorher dran.
			// Ebenfalls wird berücksichtigt, wo der Counter aktuell steht. Jenchdem kommt noch bis zu drei weiterer Cocktails zuvor.
			int lCocktailsFrom1 = mQueue[0].getNumberOfOrders() * 3;
			lCocktailsFrom1 = lCocktailsFrom1 + (3 - mQueueSelectCounter);
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + lCocktailsFrom1;
		}
		else
		{
			// Beispiel: Liste_1: 2, Liste_0: 5
			// 5 * 3 = 15
			// Es wuerden 15 Cocktails aus der Liste_1 vorher kommen,
			// es sind aber nur 2 in der Liste_1.
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + mQueue[1].getNumberOfOrders();
		}
		return lNumberOfOrdersBefore;
	}
}
int cCocktailMixer::getNumberOfOrdersBeforeOrderNumber(int pOrderNumber)
{
	int lPrio;
	int lPlace = mQueue[2].getPlaceOfOrderNumber(pOrderNumber);
	if (lPlace >= 0)
	{
		lPrio = 2;
	}
	else
	{
		lPlace = mQueue[1].getPlaceOfOrderNumber(pOrderNumber);
		if (lPlace >= 0)
		{
			lPrio = 1;
		}
		else
		{
			lPlace = mQueue[0].getPlaceOfOrderNumber(pOrderNumber);
			if (lPlace >= 0)
			{
				lPrio = 0;
			}
		}
	}
	if (lPlace == -1)
	{
		return -1;
	}


	if (lPrio == 2) // Die Warteschlange mit hoechster Prio wird vor allen anderen abgearbeitet.
	{
		return lPlace; // Die Anzahl an Cocktails vor einer neuen Bestellung sind nur die Cocktails welche bisher in der Lise der hoechsten Prio sind.
	}

	int lNumberOfOrdersBefore = 0;  // Anzahl an Bestellungen (in den bisherigen Warteschlangen) auf 0 initialisieren.
	if (lPrio == 1) // Mittlere Prio
	{
		lNumberOfOrdersBefore = mQueue[2].getNumberOfOrders() + lPlace; // Hier kommen alle Cocktails der hoechsten Prio vorher sowie die bisherigen Cocktails in der mittleren Prio-Liste
		// Da sich die mittlere und niedrigste Prio abwechseln (3x mittlere, 1x niedrigste), kommen ggf. auch Cocktails aus der niedrigsten Liste vorher dran.
		if (lPlace / 3 < mQueue[0].getNumberOfOrders())
		{
			// Beispiel: Liste_1: 7, Liste_0: 12
			// 7 / 3 = 2 (Nachkommastellen werden abgeschnitten).
			// 7 % 3 = 1
			// Es sind 12 Cocktails in der Liste_0,
			// es kommen davon aber nur 2 Cocktails vorher dran.
			// Ebenfalls wird berücksichtigt, wo der Counter aktuell steht. Jenchdem kommt noch ein weiterer Cocktail zuvor.
			int lCocktailsFrom0 = lPlace / 3;
			lCocktailsFrom0 = lCocktailsFrom0 + (lPlace % 3 < (3 - mQueueSelectCounter) ? 0 : 1);
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + lCocktailsFrom0;
		}
		else
		{
			// Beispiel: Liste_1: 11, Liste_0: 2
			// 11 / 3 = 3 (Nachkommastellen werden abgeschnitten).
			// Es wuerden 3 Cocktails vorher dran kommen, 
			// es sind allerdings nur 2 in der Liste.
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + mQueue[0].getNumberOfOrders();
		}
		return lNumberOfOrdersBefore;
	}
	if (lPrio == 0)
	{
		lNumberOfOrdersBefore = mQueue[2].getNumberOfOrders() + lPlace;
		// Da sich die mittlere und niedrigste Prio abwechseln (3x mittlere, 1x niedrigste), kommen ggf. auch Cocktails aus der niedrigsten Liste vorher dran.
		if (lPlace * 3 + (3 - mQueueSelectCounter) < mQueue[1].getNumberOfOrders())
		{
			// Beispiel: Liste_1: 11, Liste_0: 2
			// 2 * 3 = 6
			// Es sind 11 Cocktails in der Liste_1,
			// davon kommen aber nur 6 vorher dran.
			// Ebenfalls wird berücksichtigt, wo der Counter aktuell steht. Jenchdem kommt noch bis zu drei weiterer Cocktails zuvor.
			int lCocktailsFrom1 = lPlace * 3;
			lCocktailsFrom1 = lCocktailsFrom1 + (3 - mQueueSelectCounter);
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + lCocktailsFrom1;
		}
		else
		{
			// Beispiel: Liste_1: 2, Liste_0: 5
			// 5 * 3 = 15
			// Es wuerden 15 Cocktails aus der Liste_1 vorher kommen,
			// es sind aber nur 2 in der Liste_1.
			lNumberOfOrdersBefore = lNumberOfOrdersBefore + mQueue[1].getNumberOfOrders();
		}
		return lNumberOfOrdersBefore;
	}
}
//int cCocktailMixer::getNumberOfOrdersInQueue(int pPrio)
//{
//	return getNumberOfOrdersBefore(pPrio, mQueue[pPrio].getNumberOfOrders() + 1);
//}
//int cCocktailMixer::getNumberOfOrdersBeforeOrderNumber(int pOrderNumber)
//{
//	int lPrio;
//	Serial.print("Prio: 2 ");
//	int lPlace = mQueue[2].getPlaceOfOrderNumber(pOrderNumber);
//	if (lPlace >= 0)
//	{
//		lPrio = 2;
//	}
//	else
//	{
//		Serial.print("Prio: 1 ");
//		lPlace = mQueue[1].getPlaceOfOrderNumber(pOrderNumber);
//		if (lPlace >= 0)
//		{
//			lPrio = 1; 
//		}
//		else
//		{
//			Serial.print("Prio: 0 ");
//			lPlace = mQueue[0].getPlaceOfOrderNumber(pOrderNumber);
//			if (lPlace >= 0)
//			{
//				lPrio = 0;
//			}
//		}
//	}
//	if (lPlace == -1)
//	{
//		return -1;
//	}
//	else
//	{
//		return getNumberOfOrdersBefore(lPrio, lPlace);
//	}
//}
//int cCocktailMixer::getNumberOfOrdersBefore(int pPrio, int pPlace)
//{
//	Serial.println("Prio: " + String(pPrio) + " Place: " + String(pPlace));
//	if (pPrio == 2)
//	{ // Falls die Prio 2 ist, sind nur die Bestellungen aus Prio 2 vorher.
//		return pPlace - 1;
//	}
//	int lQueue[] = { 0, 0, 0 };
//	lQueue[0] = mQueue[0].getNumberOfOrders(); // Werte lokal speichern
//	lQueue[1] = mQueue[1].getNumberOfOrders(); // Werte lokal speichern
//	lQueue[2] = mQueue[2].getNumberOfOrders(); // Werte lokal speichern
//	lQueue[pPrio] = pPlace; // Wert der Gesuchten Liste richtig ueberschreiben.
//	int lQueueSelectCounter = mQueueSelectCounter; // SelectCOunter lokal speichern.
//	Serial.println("SelectCounter: " + String(lQueueSelectCounter));
//	int lOrdersBefore = lQueue[2]; // Alle Bestellungen aus der Prio 2 kommen vorher dran.
//	while (lQueue[pPrio] > 0) // Listen abbbauen und Bestellungen zaehlen.
//	{
//		if (lQueue[0] > 0 && lQueue[1] > 0)
//		{
//			if (lQueueSelectCounter < 3)
//			{
//				lQueue[1]--;
//				lQueueSelectCounter++;
//				lOrdersBefore++;
//			}
//			else
//			{
//				lQueue[0]--;
//				lQueueSelectCounter = 0;
//				lOrdersBefore++;
//			}
//		}
//		else
//		{
//			lQueue[pPrio]--;
//			lOrdersBefore++;
//		}
//	}
//	return lOrdersBefore;
//}
int cCocktailMixer::getNextOrderNumber()
{
	//MyMutex_mQueue_lock();	// Bereich fuer andere Thraeds sperren
	int lNextOrderNumber = mNextOrderNumber; // Anzahl lokal speichern.
	//MyMutex_mQueue_unlock(); // Bereich freigeben.
	return lNextOrderNumber + 100; // Bestellnummer (+100) zurueckgeben (dmait alle dieselbe Laenge haben 100, 101, 102, ... 110, ... statt 0, 1, 2, ..., 10).
}
int cCocktailMixer::getCurrentOrderNumber()
{
	return mCurrentOrderNumber;
}
int cCocktailMixer::mixNextCocktail()
{
	if (CheckNormalMode()) // Pruefen ob sich sich die Maschiene im richtigen Betriebsmodus befindet.
	{
		gOLED.PrintFirstLine("Cocktail aus Warteschlange"); // Statusmeldung
		int lReturnValue = 0;
		cOrder lOrder;
		MyMutex_mQueue_lock(); // Bereich sperren
		if (mQueue[2].getOrder(&lOrder)) // Bestellung mit hoechster Prioritaet ggf. aus der Warteschlange holen.
		{ // Hoechste Prio
			MyMutex_mQueue_unlock(); // Bereich wieder freigeben.
			lReturnValue = mixCocktail(lOrder); // Bestellung ausfuehren und Rueckgabewert speichern.
		}
		else // Falls sich keine Bestellung in der Warteschlange mit der hoechsten Prioritaet befindet.
		{
			if (mQueue[1].getNumberOfOrders() > 0 && mQueue[0].getNumberOfOrders() > 0) // Falls sowohl in der Warteschlange mit mittlerer und mit niedrigster Prioritaet Bestellungen sind.
			{   // Hier wird die Warteschlange mit mittlerer Prioritaet 3x heufiger bedient als die mit niedriger Prioritaet.
				// Wenn lCounter 0, 1, 2 ist wird die Warteschlange mit mittlerer Prioritaet bedient.
				// Wenn lCounter 3 ist wird die mit niedriger Priotitaet bedient.
				//static int lCounter = 0; // Mit 0 wird nur beim aller ersten mal aufrufen initialisiert, danach behaelt die Variable ihren Wert den sie nachfolgend zugewiesen bekommt, auch nach Verlasen der Funktion (static).
				if (mQueueSelectCounter < 3) // 0, 1, 2
				{
					if (mQueue[1].getOrder(&lOrder)) // Bestellung mit mittlerer Prioritaet ggf. aus der Warteschlange holen.
					{ // Mittlere Prio
						mQueueSelectCounter++; // Counter um eins erhoehen
						MyMutex_mQueue_unlock(); // Bereich wieder freigeben
						lReturnValue = mixCocktail(lOrder); // Bestellung ausfuehren und Rueckgabewert speichern.
					}					
				}
				else // 3
				{
					if (mQueue[0].getOrder(&lOrder)) // Bestellung mit niedrigster Prioritaet ggf. aus der Warteschlange holen.
					{ // Normale Prio
						mQueueSelectCounter = 0; // Counter zuruecksetzen
						MyMutex_mQueue_unlock(); // Bereich wieder freigeben.
						lReturnValue = mixCocktail(lOrder); // Bestellung ausfuehren und Rueckgabewert speichern.
					}					
				}
			}
			else // Es sind nur in einer der beiden Warteschlangen Bestellungen (entweder mittlere oder niedrigste Prioritaet).
			{
				mQueueSelectCounter = 0; // Counter zuruecksetzen
				if (mQueue[1].getOrder(&lOrder)) // Bestellung mit mittlerer Prioritaet ggf. aus der Warteschlange holen.
				{ // Mittlere Prio
					MyMutex_mQueue_unlock(); // Bereich wieder freigeben.
					lReturnValue = mixCocktail(lOrder); // Bestellung ausfuehren und Rueckgabewert speichern.
				}
				else if (mQueue[0].getOrder(&lOrder)) // Bestellung mit niedrigster Prioritaet ggf. aus der Warteschlange holen.
				{ // Mittlere Prio
					MyMutex_mQueue_unlock(); // Bereich wieder freigeben.
					lReturnValue = mixCocktail(lOrder); // Bestellung ausfuehren und Rueckgabewert speichern.
				}
				else
				{
					MyMutex_mQueue_unlock(); // Bereich wieder freigeben
					lReturnValue = mixNextCocktail_OK_WarteschlangeLeer; // Kein Cocktail in der Warteschlange
				}
				
			}
		}
		MyMutex_mQueue_lock(); // Bereich fuer andere Threads sperren
		if (lReturnValue < 0) // Falls die Bestellung nicht ordentlich ausgefuehrt werden konnte.
		{ // Bei einem Fehler wird die Bestellung wieder eingereiht (wieder an vorderster Stelle).
			mQueue[lOrder.getPrio()].addOrder_FirstPlace(lOrder);
		}
		MyMutex_mQueue_unlock(); // Bereich wieder freigeben.
		return lReturnValue; // Rueckgabewert zurueckgeben.
	}
	else
	{
		return MachineState_ERROR_AnlagenStatus;
	}
}
int cCocktailMixer::mixCocktail(cOrder pBestellung)
{
	Serial.println("Mix: " + pBestellung.getCocktail().getCocktailName());
	gOLED.PrintFirstLine("Mix: " + pBestellung.getCocktail().getCocktailName());
	mCurrentOrderNumber = pBestellung.getOrderNumber(); // Letzte bearbeitete Bestellnummer aktuallisieren.
	int lSlotNumber;
	int lGlassIndex;
	if (findEmptyGlass(lSlotNumber, lGlassIndex) >= findEmptyGlass_OK) // Suche nach einem leeren Glas
	{ // Falls ein leeres Glas gefunden wurde
		gOLED.PrintFirstLine("Leeres Glas gefunden");
		cCocktail lCocktail = pBestellung.getCocktail(); // Cocktail aus der Bestellung lokal speichern.
		cGlass lGlass = mGlasses.getGlas(lGlassIndex); // Gefundenes Glas lokal speichern.
		int lNumberOfIngredients = lCocktail.getNumberOfIngredients(); // Anzahl an Zutaten des COcktsails ermitteln.
		int lReservoirIndices[MaxNumberOfReservoir]; // Liste fuer die benoetigten Vorratsbehaelter anlegen.
		long lTimes[MaxNumberOfReservoir]; // Liste fuer die benoetigten Ventiloeffnungszeiten anlegen.
		double lStartWeight = mScale.getWeight(); // Startgewicht ermitteln
		int lTotalAmount = pBestellung.getOrderedAmount() == ORDER_SMALL ? gCocktailMixer.mGlasses.getSmallAmount() : gCocktailMixer.mGlasses.getBigAmount(); // Wunschmaenge ermiteln (klein oder gross).
		int lSingleAmount = 0;
		int lAmount[] = { 0,0,0,0,0,0,0,0 };
		for (int i = 0; i < lNumberOfIngredients; i++)
		{
			cIngredient lIngredient = lCocktail.getIngredient(i); // Zutat lokal speichern
			lReservoirIndices[i] = mReservoir.getReservoirIndex(lIngredient.getName()); // Index des Benoetigten Vorratsbehaelter ermitteln.
			
			lSingleAmount = (double)lTotalAmount * pBestellung.getCocktail().getIngredient(i).getRelAmount(); // benoetigte Menge der Zutat (fuer diesen Cocktail mit der angegebenen Gesamtmaenge) ermitteln.
			lAmount[lReservoirIndices[i]] = (double)lTotalAmount * pBestellung.getCocktail().getIngredient(i).getRelAmount(); // benoetigte Menge der Zutat (fuer diesen Cocktail mit der angegebenen Gesamtmaenge) ermitteln.
			Serial.println("Amount in ml " + String(lSingleAmount));
			mReservoir.addToSum(lReservoirIndices[i], lSingleAmount); // Benoetigte Maenge in der Verbrauchsstatistik hinzufuegen.
			lTimes[i] = mReservoir.AmountToTime(lReservoirIndices[i], lSingleAmount); // Oeffnungszeit des Ventiles zur angegebenen Maenge ermitteln.
		}
		cCocktail myCocktail = pBestellung.getCocktail();
		String myCocktailName = myCocktail.getCocktailName();
		Serial.printf("$LOG;%s;%s;%s;%s;%s;%s;%s;%s;%s&\r\n", myCocktailName.c_str(), String(lAmount[0]).c_str(), String(lAmount[1]).c_str(), String(lAmount[2]).c_str(), String(lAmount[3]).c_str(), String(lAmount[4]).c_str(), String(lAmount[5]).c_str(), String(lAmount[6]).c_str(), String(lAmount[7]).c_str());


		gOLED.PrintFirstLine("Cocktail mixen");
		mServo.goToPosition_Open();
		gCocktailMixer.WaitMillis(1000);
		mValveControl.setValveState(lReservoirIndices, lTimes, lNumberOfIngredients); // Liste der Ventile und Zeiten an die Ventilsteuerung uebergeben.
		WaitMillis(2000);//DEFAULT 1000
		mServo.goToPosition_Close();
		mAmountOfMixedCocktails++; // Anzahl an gemixten Cocktails erhoehen (fuer die Statistik).
		double lEndWeight = mScale.getWeight(); // Endgewicht ermitteln
		Serial.println("Weight Difference: " + String(lEndWeight - lStartWeight));
		// Check Weight ... return 0 wenn Ok, -2 falls nicht OK (Vorratsbehaelter leer)
		mSlotOrderNumber[lSlotNumber - 1] = pBestellung.getOrderNumber(); // -1, da lSlotNumber bei 1 beginnt. Bestellnummer dem Slot zuordnen, in dem der Cocktail gemixt wurde.
		mSlotCocktailName[lSlotNumber - 1] = pBestellung.getCocktail().getCocktailName(); // -1, da lSlotNumber bei 1 beginnt. Cocktailname dem Slot zuordnen, in dem der Cocktail gemixt wurde.
		
		mRotateTable.goToNextPosition(); // Drehteller eine Position weiter rotieren.
		return mixCocktail_OK;
	}
	else
	{
		return ERROR_mixCocktail_KeinGlasGefunden;
	}
}
int cCocktailMixer::mixCocktail_TEST(cCocktail pCocktail)
{
	Serial.println(pCocktail.getCocktailName());
	int lNumberOfIngredients = pCocktail.getNumberOfIngredients();
	int lValveIndices[MaxNumberOfReservoir];
	long lTimes[MaxNumberOfReservoir];

	for (int i = 0; i < lNumberOfIngredients; i++)
	{
		cIngredient lIngredient = pCocktail.getIngredient(i);
		Serial.println(lIngredient.getName());
		lValveIndices[i] = mReservoir.getReservoirIndex(lIngredient.getName());
		int lAmount = 200; // Glas oder Wunschmaenge begränzt.
		mReservoir.addToSum(i, lAmount);
		lTimes[i] = mReservoir.AmountToTime(lValveIndices[i], lAmount);
		Serial.println(lTimes[i]);
	}
	if (CheckNormalMode())
	{
		mValveControl.setValveState(lValveIndices, lTimes, lNumberOfIngredients);
		// Check Weight ... return 0 wenn Ok, -2 falls nicht OK (Vorratsbehälter leer)
		return mixCocktail_OK;
	}
}
int cCocktailMixer::InitIngredients()
{
	gOLED.PrintFirstLine("Init. Vorratsbehaelter");
	Serial.println("Init_All");
	int lReturnValue = 0; // Anzahl an initialisierten Zutaten.
	for (int i = 0; i < mReservoir.getNumberOfReservoir(); i++)
	{
		gOLED.PrintFirstLine("Init.: " + String(i + 1));
		Serial.println("Init.: " + String(i + 1));
		if (InitIngredient(i) >= 0) // Prueft ob die Initialisierung in Ordnung war.
		{
			lReturnValue++;
		}
		else
		{
			Serial.println("Init_" + String(i + 1) + "Failed");
			break;
		}
	}
	return lReturnValue;
}
int cCocktailMixer::InitIngredient(int pIndex)
{
	int lSlotNumber;
	int lGlassIndex;
	if (findEmptyGlass(lSlotNumber, lGlassIndex) >= findEmptyGlass_OK) // Suche nach einem leeren Glas
	{
		Serial.println("FoundGlass");
		int lFillWeight = 200; //250 Fuellmenge welche zum Initialisieren gefuellt werden soll.
		// Nachfolgend werden mehrere Wertepaare (x-Achse = Zeit, y-Achse = Gewicht) aufgenommen.
		// Aus diesen Wertepaaren wird die Gausssche-Ausgleichsgerade ermittelt.
		int lDeltaTime = 100; // Alle 100ms wird ein Wertepaar aufgezeichnet.

		unsigned long lTempTime = 0;
		double lTempWeight = 0;

		vector<double> lWeights;
		vector<double> lTimes;


		double lStartWeight = mScale.getWeight();
		mServo.goToPosition_Open();
		WaitMillis(1000);
		unsigned long lStartTime = millis();
		unsigned long lMaxInitTime = 30000; // Maximale Zeit zum Initialisieren. Dauert die Initialisierung laenger, ist vermutlich der Vorratsbehaelter leer.
		
		gCocktailMixer.mValveControl.setValveState(pIndex, true);
		while (lTempWeight < lFillWeight && millis() - lStartTime <= lMaxInitTime)
		{
			if (CheckNormalMode() || CheckInitMode()) // Pruefen ob sich die Maschiene im richtigen Zustand befindet.
			{
				if (millis() - lTempTime >= lDeltaTime)
				{
					lTempTime = millis() - lStartTime;
					lTempWeight = mScale.getWeight(2) - lStartWeight;
					lWeights.push_back(lTempWeight);
					lTimes.push_back(lTempTime);
					Serial.println(String(lTempTime) + ";" + lTempWeight);
				}
				delay(1);
			}
			else
			{
				
				gCocktailMixer.mValveControl.setValveState(pIndex, false);
				delay(1000);
				mServo.goToPosition_Close();
				return MachineState_ERROR_AnlagenStatus;
			}
		}
		gCocktailMixer.mValveControl.setValveState(pIndex, false);
		WaitMillis(1000);
		mServo.goToPosition_Close();
		if (lTempWeight < lFillWeight) 
		{ // Falls die Zeit abgelaufen ist und das gewicht noch nicht erreicht wurde.
			setMachineState(MachineState_ERROR_VorratLeer);
			gOLED.PrintFirstLine("Vorratsbehaelter leer");
			return ERROR_INIT_VorratLeer;
		}
		WaitMillis(1000); // Abtropfen abwarten...
		lTempTime = millis() - lStartTime;
		lTempWeight = mScale.getWeight(2) - lStartWeight;
		Serial.println(String(lTempTime) + ";" + lTempWeight);

		double lSUMWeights = getSUM(&lWeights); // Summe aller Gewichte (Summe(lWeights[i]))
		double lSUMTimes = getSUM(&lTimes); // Summe aller Zeiten (Summe(lTimes[i]))
		double lSUMWeights2 = getSUM2(&lWeights); // Summe der einzelnen Quadrate der Gewichte (Summe(lWeights[i]*lWeights[i]))
		double lSUMTimesWeights = getSUM(&lTimes, &lWeights); // Summe aller Produkte aus Gewicht und Zeit (Summe(lWeights[i]*lTimes[i]))

		double lm = (lSUMTimesWeights - lSUMWeights * lSUMTimes / lTimes.size()) / (lSUMWeights2 - lSUMWeights*lSUMWeights / lTimes.size()); // Steigung des linearen Zusammenhangs
		double lb = (lSUMTimes - lm * lSUMWeights) / lTimes.size(); // Zeitlicher Offset des linearen Zusammenhangs
		lb = lb + 4 * lm;// Erhoehen des zeitlichen Offsets um 4g (Erfahrungswert).

		mReservoir.setMB(pIndex, lm, lb); // Speichern der Werte im Vorratsbehaelter

		Serial.print("Index: ");
		Serial.print(pIndex + 1);
		Serial.print(" m=");
		Serial.print(lm);
		Serial.print(" b=");
		Serial.println(lb);
		return INIT_Zutaten_OK;
	}
	else
	{
		return ERROR_INIT_KeinGlasGefunden;
	}
}
int cCocktailMixer::findEmptyGlass(int& pSlotNumber, int& pGlasIndex)
{
	Serial.println("FindEmptyGlass");
	gOLED.PrintFirstLine("Suche leeres Glas");
	int lWeight;
	int lGlassIndex;
	int lSlotNumber;
	int lSlotCounter = 0; // Anzahl an geprueften Slots.
	int lNumberOfSlotsToCheck = 6; // Anzahl an maximal zu pruefender Slots (bevor ein Fehler ausgegeben wird).
	while (lSlotCounter <= lNumberOfSlotsToCheck && (CheckNormalMode() || CheckInitMode())) // Solange die Anzahl an maximal zu pruefender Slots noch nicht ereicht wurde und sich die MAschiene im richtigen Zustand befindet.
	{
		lWeight = mScale.getWeight(); // Gewicht ermitteln.
		lGlassIndex = mGlasses.checkGlasses(lWeight); // Pruefen ob / welches Glas zum angegebenen Gewicht passt.
		lSlotNumber = mRotateTable.getPosition(); // Position des Drehtellers ermitteln.
		Serial.println("Gewicht: " + String(lWeight) + "g, GlasIndex: " + String(lGlassIndex) + ", Position: " + String(lSlotNumber));
		if (lGlassIndex >= 0 && lSlotNumber >= 1) // Pruefen ob ein Glas passt.
		{ // leeres Glas gefunden
			mSlotOrderNumber[lSlotNumber - 1] = 0; // -1, da lSlotNumber bei 1 beginnt. Wenn ein leeres Glas vorhanden ist, befindet sich kein gemixter Cocktail in dem Slot und der Index kann zurueckgesetzt werden.
			mSlotCocktailName[lSlotNumber - 1] = ""; // -1, da lSlotNumber bei 1 beginnt. Wenn ein leeres Glas vorhanden ist, befindet sich kein gemixter Cocktail in dem Slot und der Cocktailname kann zurueckgesetzt werden.

			pSlotNumber = lSlotNumber; // Uebergeben der Werte
			pGlasIndex = lGlassIndex; // Uebergeben der Werte
			return findEmptyGlass_OK;
		}
		else
		{
			if (lGlassIndex == ERROR_checkGlasses_KeinGlas && lSlotNumber >= 1)
			{
				mSlotOrderNumber[lSlotNumber - 1] = 0; // -1, da lSlotNumber bei 1 beginnt. Wenn ein leeres Glas vorhanden ist, befindet sich kein gemixter Cocktail in dem Slot und der Index kann zurueckgesetzt werden.
				mSlotCocktailName[lSlotNumber - 1] = ""; // -1, da lSlotNumber bei 1 beginnt. Wenn ein leeres Glas vorhanden ist, befindet sich kein gemixter Cocktail in dem Slot und der Cocktailname kann zurueckgesetzt werden.
			}
			lSlotCounter++;
		}

		if (!mRotateTable.goToNextPosition()) // Naechste Position anfahren.
		{
			return ERROR_findEmptyGlass_PositionNichtGefunden;
		}
	}
	gOLED.PrintFirstLine("Kein Glas gefunden");
	setMachineState(MachineState_ERROR_KeinLeeresGlasGefunden);
	// Falls kein leeres Glas gefunden wurde
	return ERROR_findEmptyGlass_KeinGlasGefunden;
}
int cCocktailMixer::getSlotnummer(int pOrderNumber)
{
	for (int i = 1; i <= NumberOfSlotsRotateTable; i++)
	{
		if (getSlotBestellnummer(i) == pOrderNumber)
		{
			return i;
		}
	}
	return -1;
}
int cCocktailMixer::getSlotBestellnummer(int pSlotNumber)
{
	if (pSlotNumber >= 1 && pSlotNumber <= NumberOfSlotsRotateTable)
	{
		return mSlotOrderNumber[pSlotNumber - 1];
	}
	else
	{
		return -1;
	}
}
String cCocktailMixer::getSlotCocktailName(int pSlotNumber)
{
	if (pSlotNumber >= 1 && pSlotNumber <= NumberOfSlotsRotateTable)
	{
		return mSlotCocktailName[pSlotNumber - 1];
	}
	else
	{
		return "";
	}
}
void cCocktailMixer::WaitMillis(unsigned long pTime)
{
	unsigned long lStartTime = millis();
	while (millis() - lStartTime < pTime && (CheckNormalMode() || CheckInitMode())) // Wartet die angegebene Zeit ab bzw. bricht auch dann ab, wenn sich die Maschiene nicht mehr im passenden Zustand befindet.
	{
		delay(1);
	}
}
int cCocktailMixer::getNumberOfServedCocktails()
{
	return mAmountOfMixedCocktails;
}
