#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "SparkFunSX1509.h"
#include "../libs/MyMutex.h"

class cMenue;				// Bekanntmachen der Klassen
class cMenueItem;			// Bekanntmachen der Klassen
class cCocktailMixer;		// Bekanntmachen der Klassen
class cCocktailList;		// Bekanntmachen der Klassen
class cOLED;				// Bekanntmachen der Klassen

extern cMenue gMenue;						// Bekanntmachen der Objekte
extern cCocktailMixer gCocktailMixer;		// Bekanntmachen der Objekte
extern cOLED gOLED;							// Bekanntmachen der Objekte
extern cCocktailList gAlcoholicList;		// Bekanntmachen der Objekte
extern cCocktailList gNonAlcoholicList;		// Bekanntmachen der Objekte

#ifndef REGION_MixHit_Infos
#ifndef FillSIZE
#define ORDER_BIG 2
#define ORDER_SMALL 1
#endif


#define MaxNumberOfReservoir 8
#define MaxNumberOfOpenedValve_FastMode 3
#define MinTimeBetwenVentilOpen 100 // in ms

#define NumberOfSlotsRotateTable 5

#define MaxMenueLayers 10

#define WeightEmptyScale 10 // Minimalgewicht, darunter wird angenommen, dass nichts auf der Waage steht.
#define GlassTollerance 30
#define MaxNumberOfDifferentGlasses 1	// Aktuell ist nur eine Sorte Glaeser erlaubt. Um weitere hinzuzufuegen wird zusaetzliche Sensorik benoetigt!
										// Beispiel: Glas_1 hat 400g, Glas_2 hat 280g. Steht nun ein Glas_2 auf der Waage mit einem kleinen Cocktail (120g) auf der Waage, sind dies ebenfalls 400g. Nun kann nicht richtig zwischen den Glaesern unterschieden werden.

#define OLED_MAX_SPALTEN 21
#define OLED_MAX_ZEILEN 8
#endif
#ifndef REGION_PIN_Belegung
#define PinOK 16	// 3
#define PinBACK 17	// 4
#define PinDOWN 18	// 2
#define PinUP 19	// 1
#define PinONOFF 27 // 5



#define PinScaleData 25
#define PinScaleCLK 26

#define PinPwm 14
#define PinDrehrichtung 12
#define PinMagnetTaster 23
#define DelayAnlauframpe 500							// Angabe in µs

#define PWM_FREQUENCY 5000								// PWM-Frequenz
#define CHANNEL 7										// Channel
#define RESOLUTION 8									// Resolution

#define PinServoPWM 32
#define ServoPosAktiv 20
#define ServoPosDeaktiv 110

#define PinSX1509Valve_0 7 
#define PinSX1509Valve_1 0
#define PinSX1509Valve_2 6
#define PinSX1509Valve_3 1
#define PinSX1509Valve_4 5
#define PinSX1509Valve_5 2
#define PinSX1509Valve_6 4
#define PinSX1509Valve_7 3
extern const int PinSX1509Valve[MaxNumberOfReservoir];

#define I2C_SX1509address 0x3E
#define I2C_OLEDaddress 0x3C

#endif
#ifndef REGION_MachineStates
#define MachineState_OK 0
#define MachineState_Reset 1

#define MachineState_Einstellungen 10
#define MachineState_Einstellungen_InitAll 11
#define MachineState_Einstellungen_InitSingle 12 // 12,13,14,15,16,17,18,19

#define MachineState_Betriebsmodus 20

#define MachineState_NormalMode 30
#define MachineState_NormalMode_Bestellnummern 31
#define MachineState_NormalMode_Statistik 32
#define MachineState_NormalMode_BestellungAufgeben 33

#define MachineState_TestMode 40
#define MachineState_TestMode_RotateTableR 41
#define MachineState_TestMode_RotateTableL 42
#define MachineState_TestMode_Valve 43
#define MachineState_TestMode_ValveOpen 44
#define MachineState_TestMode_Scale 45
#define MachineState_TestMode_ScaleTare 46
#define MachineState_TestMode_Servo0 47
#define MachineState_TestMode_Servo1 48

#define MachineState_Einstellungen_Werkseinstellung 50


#define MachineState_ERROR_AnlagenStatus -1
#define MachineState_ERROR_NotAus -10
#define MachineState_ERROR_VorratLeer -20
#define MachineState_ERROR_KeinLeeresGlasGefunden -30
#define MachineState_ERROR_Position_1_NichtGefunden - 40
#define MachineState_ERROR_NaechstePositionNichtGefunden -41

#endif

#ifndef REGION_Interrupts

#define Entprellzeit 200 // ms
extern bool STOP_Mode;
void AttacheInterrupts();		// Aktiviert die Interrupts
void DettacheInterrupts();		// Deaktiviert die Interrupts

#ifndef REGION_ISR
void DO_OK();			// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.
void DO_DOWN();			// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.
void DO_UP();			// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.
void DO_BACK();			// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.
void DO_Stop();			// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.
void DO_Start();		// Einstiegspunkt der Interruptroutiene beim druecken des Buttons.

void NO_OPERATION(cMenueItem& pSender);				// Funktionen fuer die mOK_Funktionen der MenueItems.
void ResetESP(cMenueItem& pSender);					// Funktionen fuer die mOK_Funktionen der MenueItems.
void Config(cMenueItem& pSender);					// Funktionen fuer die mOK_Funktionen der MenueItems.
void Config_InitAll(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void Config_InitSingle(cMenueItem& pSender);		// Funktionen fuer die mOK_Funktionen der MenueItems.
void Config_DefaultConfig(cMenueItem& pSender);		// Funktionen fuer die mOK_Funktionen der MenueItems.

void TestMode(cMenueItem& pSender);					// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_RotateR(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_RotateL(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_Valve(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_ValveOpen(cMenueItem& pSender);		// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_Scale(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_Scale_Tare(cMenueItem& pSender);		// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_Servo0(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.
void TestMode_Servo1(cMenueItem& pSender);			// Funktionen fuer die mOK_Funktionen der MenueItems.

void ModeSelect(cMenueItem& pSender);							// Funktionen fuer die mOK_Funktionen der MenueItems.

void NormalMode(cMenueItem& pSender);							// Funktionen fuer die mOK_Funktionen der MenueItems.
void NormalMode_OrderNumbern(cMenueItem& pSender);				// Funktionen fuer die mOK_Funktionen der MenueItems.
void NormalMode_Statistik(cMenueItem& pSender);					// Funktionen fuer die mOK_Funktionen der MenueItems.
void NormalMode_OrderCocktailAlcohol(cMenueItem& pSender);		// Funktionen fuer die mOK_Funktionen der MenueItems.
void NormalMode_OrderCocktailNonAlcohol(cMenueItem& pSender);	// Funktionen fuer die mOK_Funktionen der MenueItems.

#endif

#endif
#ifndef REGION_MachineStates
void setMachineState(int pMachineState);					// Setzt den Maschienenstatus (mit Mutex)
void setMachineState_ForInterrupt(int pMachineState);		// Setzt den Maschienenstatus (ohne Mutex)
int getMachineState();										// Gibt den Maschienenstatus zurueck (mit Mutex)
int getMachineState_ForInterrupt();							// Gibt den Maschienenstatus zurueck (ohne Mutex)
int getOldMachineState();									// Gibt den vorhergehenden Maschienenstatus zurueck (mit Mutex)
int getOldMachineState_ForInterrupt();						// Gibt den vorhergehenden Maschienenstatus zurueck (ohne Mutex)
bool CheckNormalMode();										// Prueft nach, ob sich die Maschiene im Normalbetrieb (oder Unterpunkte davon) befindet.
bool CheckInitMode();										// Prueft nach, ob sich die Maschiene im Initialisierungsmodus befindet.
void CloseValvesStopMotor();								// Schlieﬂt die Ventile und stoppt den Motor.
#endif

#endif
