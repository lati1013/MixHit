#include "Configuration.h"

#include "../remote/cMenue.h"
#include "cCocktailMixer.h"
#include "../driver/cOLED.h"
#include "collections/cCocktailList.h"


#ifndef REGION_PIN_Belegung

// TODO: File Name Refactoring

const int PinSX1509Valve[MaxNumberOfReservoir] = { PinSX1509Valve_0,
PinSX1509Valve_1,
PinSX1509Valve_2,
PinSX1509Valve_3,
PinSX1509Valve_4,
PinSX1509Valve_5,
PinSX1509Valve_6,
PinSX1509Valve_7 };
#endif
#ifndef REGION_Interrupts
bool STOP_Mode = false;

void AttacheInterrupts()
{
	pinMode(PinOK, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PinOK), DO_OK, FALLING);
	pinMode(PinBACK, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PinBACK), DO_BACK, FALLING);
	pinMode(PinUP, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PinUP), DO_UP, FALLING);
	pinMode(PinDOWN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PinDOWN), DO_DOWN, FALLING);
	pinMode(PinONOFF, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PinONOFF), DO_Stop, RISING);
}

void DettacheInterrupts()
{
	detachInterrupt(PinOK);
	detachInterrupt(PinBACK);
	detachInterrupt(PinUP);
	detachInterrupt(PinDOWN);
	//detachInterrupt(PinONOFF);
}

#ifndef REGION_ISR

void DO_BACK()
{
	if (!STOP_Mode)
	{
		static unsigned long OldTime = 0; // static: Der Variable wird nur beim ersten aufrufen der Funktion die 0 zugewiesen. Anschliesend behaelt sie die Werte welche ihr zugewiesen werden (auch nach dem verlassen der Funktion).
		if ((millis() - OldTime) > Entprellzeit)
		{
			Serial.println("DO_BACK");
			OldTime = millis(); // letzte Schaltzeit merken  
			gMenue.BACK();
		}
	}
}
void DO_OK()
{
	if (!STOP_Mode)
	{
		static unsigned long OldTime = 0; // static: Der Variable wird nur beim ersten aufrufen der Funktion die 0 zugewiesen. Anschliesend behaelt sie die Werte welche ihr zugewiesen werden (auch nach dem verlassen der Funktion).
		if ((millis() - OldTime) > Entprellzeit)
		{
			Serial.println("DO_OK");
			OldTime = millis(); // letzte Schaltzeit merken   
			gMenue.OK();
		}
	}
}
void DO_DOWN()
{
	if (!STOP_Mode)
	{
		static unsigned long OldTime = 0; // static: Der Variable wird nur beim ersten aufrufen der Funktion die 0 zugewiesen. Anschliesend behaelt sie die Werte welche ihr zugewiesen werden (auch nach dem verlassen der Funktion).
		if ((millis() - OldTime) > Entprellzeit)
		{
			Serial.println("DO_DOWN");
			OldTime = millis(); // letzte Schaltzeit merken  
			gMenue.DOWN();
		}
	}
}
void DO_UP()
{
	if (!STOP_Mode)
	{
		static unsigned long OldTime = 0; // static: Der Variable wird nur beim ersten aufrufen der Funktion die 0 zugewiesen. Anschliesend behaelt sie die Werte welche ihr zugewiesen werden (auch nach dem verlassen der Funktion).
		if ((millis() - OldTime) > Entprellzeit)
		{
			Serial.println("DO_UP");
			OldTime = millis(); // letzte Schaltzeit merken  
			gMenue.UP();
		}
	}
}

void DO_Stop()
{
	static unsigned long OldTime = 0; // static: Der Variable wird nur beim ersten aufrufen der Funktion die 0 zugewiesen. Anschliesend behaelt sie die Werte welche ihr zugewiesen werden (auch nach dem verlassen der Funktion).
	if ((millis() - OldTime) > Entprellzeit)
	{
		Serial.println("DO_STOP");
		setMachineState_ForInterrupt(MachineState_ERROR_NotAus);
		gOLED.PrintFirstLine("!!! Stop !!!");
	}
}
void DO_Start()
{
	if (!STOP_Mode)
	{
		Serial.println("DO_Start");
		setMachineState_ForInterrupt(MachineState_OK);
		gOLED.PrintFirstLine("");
	}
}

void NO_OPERATION(cMenueItem& pSender)
{
	gOLED.PrintFirstLine("");
}
void ResetESP(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Reset);
}
void Config(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Einstellungen);
}
void Config_InitAll(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Einstellungen_InitAll);
}
void Config_DefaultConfig(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Einstellungen_Werkseinstellung);
}
void Config_InitSingle(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Einstellungen_InitSingle + pSender.mIndex - 1);
}
void TestMode(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_TestMode);
}
void TestMode_RotateR(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_RotateTableR)
	{
		setMachineState_ForInterrupt(MachineState_TestMode);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_RotateTableR);
	}
}
void TestMode_RotateL(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_RotateTableL)
	{
		setMachineState_ForInterrupt(MachineState_TestMode);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_RotateTableL);
	}
}
void TestMode_Valve(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_TestMode_Valve);
}
void TestMode_ValveOpen(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_ValveOpen)
	{
		setMachineState_ForInterrupt(MachineState_TestMode_Valve);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_ValveOpen);
	}
}
void TestMode_Scale(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_Scale)
	{
		setMachineState_ForInterrupt(MachineState_TestMode);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_Scale);
	}
}
void TestMode_Scale_Tare(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_TestMode_ScaleTare);
}
void TestMode_Servo0(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_Servo0)
	{
		setMachineState_ForInterrupt(MachineState_TestMode);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_Servo0);
	}
}
void TestMode_Servo1(cMenueItem& pSender)
{
	if (getMachineState_ForInterrupt() == MachineState_TestMode_Servo1)
	{
		setMachineState_ForInterrupt(MachineState_TestMode);
	}
	else
	{
		setMachineState_ForInterrupt(MachineState_TestMode_Servo1);
	}
}
void ModeSelect(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_Betriebsmodus);
}
void NormalMode(cMenueItem& pSender)
{
	if (!gCocktailMixer.mReservoir.getInitStateAll()) // Pruefen ob alle Vorratsbehaelter initialisiert sind. 
	{
		gOLED.PrintFirstLine("Init. Vorraete fehlt");
		gMenue.BACK();
		return;
	}
	if (gCocktailMixer.mGlasses.getNumberOfGlasses() <= 0)
	{
		gOLED.PrintFirstLine("Init. Glaeser fehlt");
		gMenue.BACK();
		return;
	}
	setMachineState_ForInterrupt(MachineState_NormalMode);
}
void NormalMode_OrderNumbern(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_NormalMode_Bestellnummern);
}
void NormalMode_Statistik(cMenueItem& pSender)
{
	setMachineState_ForInterrupt(MachineState_NormalMode_Statistik);
}
void NormalMode_OrderCocktailAlcohol(cMenueItem& pSender)
{
	cOrder lOrder(gAlcoholicList.getCocktail(pSender.mIndex), ORDER_BIG, 2);
	gCocktailMixer.addOrderToQueue(lOrder);
}
void NormalMode_OrderCocktailNonAlcohol(cMenueItem& pSender)
{
	cOrder lOrder(gNonAlcoholicList.getCocktail(pSender.mIndex), ORDER_BIG, 2);
	gCocktailMixer.addOrderToQueue(lOrder);
}
#endif
#endif
#ifndef REGION_MachineStates

volatile int gMachineState = MachineState_OK;
volatile int gOldMachineState = MachineState_OK;

void setMachineState(int pMachineState)
{
	MyMutex_gMachineState_lock();
	DettacheInterrupts();			// Disable Interrupts (Um den nachfolgenden Bereich zu Schützen)
	gOldMachineState = gMachineState;
	gMachineState = pMachineState;
	AttacheInterrupts();
	MyMutex_gMachineState_unlock();
}
void setMachineState_ForInterrupt(int pMachineState)
{
	gOldMachineState = gMachineState;
	gMachineState = pMachineState;
}
int getMachineState()
{
	int lMachineState;
	MyMutex_gMachineState_lock();
	DettacheInterrupts();						// Disable Interrupts (Um den nachfolgenden Bereich zu Schützen)
	lMachineState = gMachineState;				// Globale Variable local Speichern
	AttacheInterrupts();
	MyMutex_gMachineState_unlock();
	return lMachineState;
}
int getMachineState_ForInterrupt()
{
	return gMachineState;
}
int getOldMachineState()
{
	int lOldMachineState;
	MyMutex_gMachineState_lock();
	DettacheInterrupts();							// Disable Interrupts (Um den nachfolgenden Bereich zu Schützen)
	lOldMachineState = gOldMachineState;		// Globale Variable local Speichern
	AttacheInterrupts();
	MyMutex_gMachineState_unlock();
	return lOldMachineState;
}
int getOldMachineState_ForInterrupt()
{
	return gOldMachineState;
}
bool CheckNormalMode()
{
	int lMachineState = getMachineState();
	return  lMachineState == MachineState_NormalMode || 
			lMachineState == MachineState_NormalMode_Bestellnummern || 
			lMachineState == MachineState_NormalMode_Statistik || 
			lMachineState == MachineState_NormalMode_BestellungAufgeben;

}
bool CheckInitMode()
{
	int lMachineState = getMachineState();
	return  lMachineState == MachineState_Einstellungen_InitAll || 
		   (lMachineState >= MachineState_Einstellungen_InitSingle && 
			lMachineState < lMachineState < MachineState_Einstellungen_InitSingle + MaxNumberOfReservoir);

}
void CloseValvesStopMotor()
{
	gCocktailMixer.mRotateTable.mMotor.MotorStop();
	for (int i = 0; i < MaxNumberOfReservoir; i++)
	{
		gCocktailMixer.mValveControl.setValveState(i, false);
	}
	gCocktailMixer.mServo.goToPosition_Close();
}
#endif