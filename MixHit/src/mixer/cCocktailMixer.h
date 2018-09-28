
#ifndef _CCOCKTAILMIXER_H_
#define _CCOCKTAILMIXER_H_
#include "obj/cIngredient.h"
#include "Configuration.h"
#include "../libs/cScale.h"
#include "../driver/cRotateTable.h"
#include "collections/cGlasses.h"
#include "obj/cGlass.h"
#include "obj/cCocktail.h"
#include "collections/cQueueOfOrders.h"
#include "../driver/cValveControl.h"
#include "obj/cReservoir.h"
#include "obj/cOrder.h"
#include "../libs/MyMath.h"
#include "../driver/cServoMotor.h"


#define mixNextCocktail_OK 0
#define mixNextCocktail_OK_WarteschlangeLeer 1

#define mixCocktail_OK 0

#define ERROR_mixCocktail_KeinGlasGefunden -2

#define INIT_Zutaten_OK 0
#define ERROR_INIT_PositionNichtGefunden -1
#define ERROR_INIT_KeinGlasGefunden -2
#define ERROR_INIT_VorratLeer -3

#define findEmptyGlass_OK 0
#define ERROR_findEmptyGlass_PositionNichtGefunden -1
#define ERROR_findEmptyGlass_KeinGlasGefunden -2

class cCocktailMixer
{
public:
	cCocktailMixer();											// Standartkonstruktor
	int addOrderToQueue(cOrder pBestellung);					// Fügt eine Bestellung der entspraecheden Warteschlange hinzu.
	int getNumberOfOrdersInQueue(int pPrio);					// Gibt die Anzahl an Cocktails aus, welche aktuell vor einer Bestellung mit angegebener Prio gemixt werden würden.
	int getNumberOfOrdersBeforeOrderNumber(int pOrderNumber);	// Gibt die ANzahl an COcktails aus, welche vor der angegebenen Bestellnummer ausgeschenkt werden.
	int mixNextCocktail();										// Holt den naechsten Cocktail aus der Warteschlange (unter beruecksichtigung der Prioritaeten) und gibt ihn an "mixCocktail(...);
	int mixCocktail(cOrder pOrder);								// Mixt einen Cocktail (Prüft nach ob ein leeres Glas vorhanden ist).
	int mixCocktail_TEST(cCocktail pCocktail);					// Mixt einen Cocktail ungeachtet aller Sensorwerte (z.B. auch wenn kein Glas unter dem Ausschank steht.
	int InitIngredients();										// Initialisiert alle Zutaten (siehe InitIngredient(...);
	int InitIngredient(int pIndex);								// Initielisiert den angegebenen Zutatenvorrat (fuer die Werte zur Umrechnugn von Menge zu Zeit)
	int findEmptyGlass(int& pSlotNumber, int& pGlasIndex);		// Sucht nach einem Leeren Glas.
	int getSlotnummer(int pOrderNumber);					// Gibt die aktuelle Bestellnummer zurueck welche sich in dem Slot befindet.
	int getSlotBestellnummer(int pSlotNumber);					// Gibt die aktuelle Bestellnummer zurueck welche sich in dem Slot befindet.
	String getSlotCocktailName(int pSlotNumber);					// Gibt den aktuellen Cocktailnamen zurueck welcher sich in dem Slot befindet.
	int getNumberOfServedCocktails();							// Gibt die anzahl an gemixten COcktails seit Programmstart zurueck.
	int getNextOrderNumber();									// Gibt die naechste moegliche freie Bestellnummer zurueck.
	int getCurrentOrderNumber();								// Gibt die Bestellnummer zurueck, welche zuletzt gemixt wurde.
	cValveControl mValveControl;								// Steuerung der Ventile.
	cRotateTable mRotateTable;									// Drehteller zum rotieren der Glaeser.
	cReservoir mReservoir;										// Informationen zu den Vorraetigen Zutaten.
	cGlasses mGlasses;											// Liste an bekannten Glaeser.
	cScale mScale;												// Waage fuer die Glaserkennung
	cServoMotor mServo;											// Servomotor fuer den Abtropfschutz
private:
	//int getNumberOfOrdersBefore(int pPrio, int pPlace);
	int mSlotOrderNumber[NumberOfSlotsRotateTable];				// Zuweisen einer Bestellnummer zu einem Slot (Damit man weis, welcher bestellte Cocktail in welchem Slot im Drehteller steht).
	String mSlotCocktailName[NumberOfSlotsRotateTable ];		// Zuweisen einer Bestellnummer zu einem Slot (Damit man weis, welcher bestellte Cocktail in welchem Slot im Drehteller steht).
	int mAmountOfMixedCocktails;								// Anzahl an gemixten COcktails (seit dem letzten Neustart).
	byte mQueueSelectCounter;									// Wird benoetigt, um zu entscheiden aus welcher Warteschlange eine Bestellung geholt wird (zwischen Prio 0 und Prio 1, da diese sich ggf. abwechseln (Prio 1 3x oefters als Prio 0).
	cQueueOfOrders mQueue[3];									// Drei Warteschlangen fuer drei Prioritaeten (Prio 2 > Prio 1 > Prio 0)
	int mNextOrderNumber;										// Naechste moegliche freie Bestellnummer.
	int mCurrentOrderNumber;									// Zuletzt gemixte Bestellnummer.
	void WaitMillis(unsigned long pTime);						// Wartet die angegebene Zeit in Millisekunden. Bricht ab, sobald der Betriebsmodus verlassen wird.
};
extern cCocktailMixer gCocktailMixer;

#endif