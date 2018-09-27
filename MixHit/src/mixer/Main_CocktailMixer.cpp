// ###############################################################################
// ##																			##
// ##		#ifndef REGION_...													##
// ##		...																	##
// ##		#endif																##
// ##																			##
// ##		Dies dient dazu, um Bereiche zu benamen und hat keine Bedeutung		##
// ##		f�r den Programmcode. Dadurch wird der Code �bersichtlicher			##
// ##		und lesbarer gesteltet. In Visual-Studio k�nnen diese Bereiche		##
// ##		auch auf- und zugeklappt werden.									##
// ##																			##
// ###############################################################################

#include "Main_CocktailMixer.h"


cCocktailMixer gCocktailMixer;
bool LogActive = false;

void setup_CocktailMixer()
{
	#ifndef LoadFile
	String lConfigFileName = "";
	readFile(SPIFFS, "/Config_Select.txt", &lConfigFileName); // Liest ein, welche Config-Datei geladen werden soll.
	if (lConfigFileName == "")
	{ // Falls die Datei nicht vorhanden oder leer ist.
		lConfigFileName = "/Default_Config.txt"; // Default_Config soll geladen werden.
	}
	Serial.println("LoadFile: " + lConfigFileName);
	writeFile(SPIFFS, "/Config_Select.txt", "/Default_Config.txt");	// Zur�cksetzen auf Default (dass falls die aktuelle Config beschaedigt ist, nach einem Reset wieder die Default geladen wird - Damit der Controler nicht staendeig die fehlerhafte Datei laed und resetet).
	LoadConfigFile(lConfigFileName);
	writeFile(SPIFFS, "/Config_Select.txt", lConfigFileName); // Falls beim Laden alles in Ordnung war (und das Programm nicht abstuerzt) wird die Config_Select wieder auf den Urspruenglichen Wert gesetzt.
	#endif	
	#ifndef REGON_Menue
	create_Menue();
	Serial.println("Menue_OK");
	#endif
	#ifndef REGION_SparkFun
	Init_SparkFun();
	Serial.println("SparkFun_OK");
	#endif
	#ifndef REGION_PinModes
	AttacheInterrupts();
	AttacheInterrupts(); // Aktiviert die Interrupts und legt die PinModes fest.
	Serial.println("Interrupts_OK");
	#endif
	gCocktailMixer.mServo.goToPosition_Close();
}

void loop_CocktailMixer()
{
	/*String serialrec = Serial.readString();
	if (serialrec == "$INITLOG\r")
	{
		Serial.println("$ACCLOG\r");
		LogActive = true;
	}*/
	#ifndef REGION_SwitchMachineStates
	#ifndef Global_To_Local_EVA_Prinzip
	static int lMachineState = MachineState_OK;
	static int lOldMachineState = MachineState_OK;
	lMachineState = getMachineState(); // globale Variable lokal Speichern, damit diese sich lokal waehrend diesem Durchlauf nicht mehr aendert.
	lOldMachineState = getOldMachineState(); // globale Variable lokal Speichern, damit diese sich lokal waehrend diesem Durchlauf nicht mehr aendert.
	#ifndef REGION_StartStoppTaster
	if (digitalRead(PinONOFF) == 1)
	{ // Falls der Stop-Taster offen ist, bzw. ein Drahtbruch vorliegt, liegt am Pin ein High-Signal an (wegen internem PullUp). In diesem Fall liegt ein Fehler vor.
		
		STOP_Mode = true; // Falls der Stop-Taster auf Stopp steht, kann die Maschine nicht weiter bedient werden.
		setMachineState(MachineState_ERROR_NotAus);
		lMachineState = MachineState_ERROR_NotAus;
		gOLED.PrintFirstLine("!!! Stop !!!");
	}
	else
	{ // Falls der Stop-Taster nicht auf Stopp steht
		if (lMachineState == MachineState_ERROR_NotAus)
		{ // Falls die Maschine aktuell im Stopp-Modus ist, wird diese nun wieder in den Zustand OK gesetzt, damit diese nun wieder bedient werden kann.
			
			lMachineState = MachineState_OK;
			setMachineState(MachineState_OK);
			gOLED.PrintFirstLine("");
		}
		STOP_Mode = false;
	}
	#endif
	#endif
	if (lMachineState >= 0) // Prueft ob ein Fehler vorliegt oder nicht. (< 0 sind Fehler).
	{
		if (lOldMachineState < 0)
		{ // Fals der letzte Zustand ein Fehlerzustand war, wird nun die Fehlermeldung auf dem Display aufgehoben.
			gOLED.PrintFirstLine("");
		}
		#ifndef REGION_NormalMode
		if (CheckNormalMode())
		{
			gCocktailMixer.mixNextCocktail();
		}
		#endif
		#ifndef REGION_ResetESP
		if (lMachineState == MachineState_Reset)
		{
			ESP.restart();
		}
		#endif
		#ifndef REGION_Werkseinstellungen
		if (lMachineState == MachineState_Einstellungen_Werkseinstellung)
		{
			gOLED.PrintFirstLine("Werkseinstellung laden...");
			//String lFiles = "";
			//listDir(SPIFFS, "/", 0, &lFiles);
			//vector<String> lFileNames;
			//split(&lFiles, ';', &lFileNames);
			//for (int i = 0; i < lFileNames.size(); i++)
			//{
			//	deleteFile(SPIFFS, lFileNames[i]);
			//}
			writeFile(SPIFFS, "/Werkseinstellungen.xtxt", "yes");
			//WriteBootstrap();
			//WriteDefaultWiFiConfig();
			//WriteDefaultConfigFile(); // DefaultConfig schreiben.
			//writeFile(SPIFFS, "/Config_Select.txt", "/Default_Config.txt"); // Default_Config als zu ladende Datei festlegen
			//writeFile(SPIFFS, "/Config_Select.wtxt", "/Default_Config.wtxt"); // Default_Config als zu ladende Datei festlegen
			ESP.restart(); // Neustart
		}
		#endif
		#ifndef REGION_Betriebsmodus
		if (lMachineState == MachineState_Betriebsmodus)
		{
			gOLED.PrintFirstLine("");
			delay(100);
		}
		#endif
		#ifndef REGION_InitAll
		if (lMachineState == MachineState_Einstellungen_InitAll)
		{
			if (gCocktailMixer.InitIngredients() == gCocktailMixer.mReservoir.getNumberOfReservoir())
			{ // Nur wenn alle Initialisierungen in Ordnung waren wird die Datei ueberschieben
				refreshReservoirInfo();
			}
			setMachineState(MachineState_Einstellungen);
		}
		#endif
		#ifndef REGION_InitSingle
		if (lMachineState >= MachineState_Einstellungen_InitSingle && lMachineState < MachineState_Einstellungen_InitSingle + MaxNumberOfReservoir)
		{
			if (gCocktailMixer.InitIngredient(lMachineState - MachineState_Einstellungen_InitSingle) >= 0)
			{ // Nur wenn die Initialisierung in Ordnung war wird die Datei ueberschrieben
				refreshReservoirInfo();
			}
			setMachineState(MachineState_Einstellungen);
		}
		#endif
		#ifndef REGION_TestMode_RotateTableR
		if (lMachineState == MachineState_TestMode_RotateTableR)
		{
			gCocktailMixer.mRotateTable.mMotor.MotorStartR();
			static unsigned long lTimeRisingEdge = 0;
			static bool lOldSignalRR = false;
			static bool lOldSignalRF = false;
			bool lSignal = gCocktailMixer.mRotateTable.mMagnetSensor.getSignal() == 1 ? true : false;
			if (lSignal != lOldSignalRR);
			{
				gOLED.PrintFirstLine("Magnetsensor: " + String(lSignal ? "1" : "0")); // Anzeigen des ZSignals des Magnetsensors in der ersten Zeile des Displays
			}
			if (risingEdge(lSignal, &lOldSignalRR))
			{
				Serial.println("RisingEdge");
				lTimeRisingEdge = millis();
			}
			if (fallingEdge(lSignal, &lOldSignalRF))
			{
				Serial.println("FalingEdge: " + String(millis() - lTimeRisingEdge) + "ms High-Time");
				lTimeRisingEdge = millis();
			}
		}
		else if (lOldMachineState == MachineState_TestMode_RotateTableR)
		{ // Falls ein anderer Modus gewaehlt wurde und zuvor der Testmodus fuer den Drehteller aktiv war.
			gCocktailMixer.mRotateTable.mMotor.MotorStop();
			if (getMachineState() == lMachineState) // Pruefen, ob sich inzwischen keine Aenderung ergeben hat.
				setMachineState(lMachineState); // Um den Alten Maschienenstatus zu ueberschreiben.
		}
		#endif
		#ifndef REGION_TestMode_RotateTableL
		if (lMachineState == MachineState_TestMode_RotateTableL)
		{
			gCocktailMixer.mRotateTable.mMotor.MotorStartL();
			static unsigned long lTimeRisingEdge = 0;
			static bool lOldSignalLR = false;
			static bool lOldSignalLF = false;
			bool lSignal = gCocktailMixer.mRotateTable.mMagnetSensor.getSignal() == 1 ? true : false;
			if (lSignal != lOldSignalLR);
			{
				gOLED.PrintFirstLine("Magnetsensor: " + String(lSignal ? "1" : "0")); // Anzeigen des ZSignals des Magnetsensors in der ersten Zeile des Displays
			}
			if (risingEdge(lSignal, &lOldSignalLR))
			{
				Serial.println("RisingEdge");
				lTimeRisingEdge = millis();
			}
			if (fallingEdge(lSignal, &lOldSignalLF))
			{
				Serial.println("FalingEdge: " + String(millis() - lTimeRisingEdge) + "ms High-Time");
				lTimeRisingEdge = millis();
			}
		}
		else if (lOldMachineState == MachineState_TestMode_RotateTableL)
		{ // Falls ein anderer Modus gewaehlt wurde und zuvor der Testmodus fuer den Drehteller aktiv war.
			gCocktailMixer.mRotateTable.mMotor.MotorStop();
			if (getMachineState() == lMachineState) // Pruefen, ob sich inzwischen keine Aenderung ergeben hat.
				setMachineState(lMachineState); // Um den Alten Maschienenstatus zu ueberschreiben.
		}
		#endif
		#ifndef REGION_TestMode_Valve
		if (lMachineState == MachineState_TestMode_Valve)
		{ // Falls die Ventile getestet werden sollen.
			for (int i = 0; i < MaxNumberOfReservoir; i++)
			{
				gCocktailMixer.mValveControl.setValveState(i, false); // Falls im Menue das aktuelle Ventil ausgewaehlt ist, wird das Ventil geoeffnet, andernfalls geschlossen
			}
			gCocktailMixer.mServo.goToPosition_Open();
			gOLED.PrintFirstLine("Geschlossen");
			delay(100);
		}
		#endif
		#ifndef REGION_TestMode_ValveOpen
		if (lMachineState == MachineState_TestMode_ValveOpen)
		{ // Falls die Ventile getestet werden sollen.
			for (int i = 0; i < MaxNumberOfReservoir; i++)
			{
				gCocktailMixer.mValveControl.setValveState(i, i == gMenue.mSelectedMenueItem->mSelectedIndex); // Falls im Menue das aktuelle Ventil ausgewaehlt ist, wird das Ventil geoeffnet, andernfalls geschlossen
			}
			gOLED.PrintFirstLine("Offen");
			delay(100);
		}
		else if (lOldMachineState == MachineState_TestMode_ValveOpen)
		{ // Falls ein anderer Modus gewaehlt wurde und zuvor der Testmodus fuer die Ventile aktiv war.
			// Alle Ventiele schlie�en
			for (int i = 0; i < MaxNumberOfReservoir; i++)
			{
				gCocktailMixer.mValveControl.setValveState(gMenue.mSelectedMenueItem->mSelectedIndex, false);
			}
			if (getMachineState() == lMachineState) // Pruefen, ob sich inzwischen keine Aenderung ergeben hat.
				setMachineState(lMachineState); // Um den Alten Maschienenstatus zu ueberschreiben.
		}
		#endif
		#ifndef REGION_TestMode_Scale_Tare
		if (lMachineState == MachineState_TestMode_ScaleTare)
		{
			gCocktailMixer.mScale.Tare();
			double Scale_Weight = gCocktailMixer.mScale.getWeight(); // Speichern des Gewichtswertes
			Serial.println(Scale_Weight);
			gOLED.PrintFirstLine("Gewicht: " + String(Scale_Weight) + "g"); // Anzeigen des Gewichtes in der ersten Zeile des Displays
			setMachineState(MachineState_TestMode_Scale);
			delay(100);
		}
		#endif
		#ifndef REGION_TestMode_Scale
		if (lMachineState == MachineState_TestMode_Scale)
		{
			Serial.println("Scale_Now");
			double Scale_Weight = gCocktailMixer.mScale.getWeight(); // Speichern des Gewichtswertes
			Serial.println(Scale_Weight);
			gOLED.PrintFirstLine("Gewicht: " + String(Scale_Weight) + "g"); // Anzeigen des Gewichtes in der ersten Zeile des Displays
			delay(100);
		}
		#endif
		#ifndef REGION_TestMode_Servo0
		if (lMachineState == MachineState_TestMode_Servo0)
		{
			gCocktailMixer.mServo.goToPosition_Close();
		}
		#endif
		#ifndef REGION_TestMode_RotateTableL
		if (lMachineState == MachineState_TestMode_Servo1)
		{
			gCocktailMixer.mServo.goToPosition_Open();
		}
		#endif
		#ifndef REGION_TestMode
		if (lMachineState == MachineState_TestMode)
		{ // Falls der TestModus aktiv ist, aber nichts ausgewaehlt wurde.
			gOLED.PrintFirstLine("Test_Mode");
			CloseValvesStopMotor(); // Alle Ventile schlie�en udn Motor stoppen.
			delay(100);
		}
		#endif
	}
	else
	{ // Falls ein Fehler vorliegt.
		CloseValvesStopMotor(); // Alle Ventile schlie�en und Motor stoppen.
		gOLED.PrintFirstLine("!!! Stop !!!");
		if (lOldMachineState >= 0)
		{ // Falls zuvor kein Fehler vorlag wird der erste Menuepunkt ausgewaehlt und angezeigt.
			Serial.println("lOldMachineState: " + String(lOldMachineState) + " NewMachineState: " + String(lMachineState));
			int lSelectedPath_0[] = { 0 };  // MixHit_0
			MyMutex_MenueItems_lock(); // Bereich fuer anderen Threads sperren
			gMenue.selectMenueItem(lSelectedPath_0, 1);
			gMenue.mSelectedMenueItem->mSelectedIndex = 0; // mSelectedIndex auf 0 setzen.
			MyMutex_MenueItems_unlock(); // Bereich wieder freigeben.
			
			if (getMachineState() == lMachineState) // Pruefen, ob sich inzwischen keine Aenderung ergeben hat.
				setMachineState(lMachineState); // Um den Alten Maschienenstatus zu ueberschreiben.
		}
		delay(100);
	}
	delay(100);
	#endif
}
void loop_OLED()
{
	#ifndef REGION_Serielle_Menue_Navigation
	// Falls keine Taster zum Steuern fuer das Menue vorhanden sind, kann das Menue ueber die Serielle Schnittstelle gesteuert werden.
	if (Serial.available() > 0)
	{
		String InString = Serial.readString();
		Serial.println(InString);
		if (InString == "U")
		{
			gMenue.UP();
		}
		else if (InString == "D")
		{
			gMenue.DOWN();
		}
		else if (InString == "OK")
		{
			gMenue.OK();
		}
		else if (InString == "BACK")
		{
			gMenue.BACK();
		}
	}
	#endif

	int lMachineState = getMachineState(); // globale Variable lokal Speichern, damit diese sich lokal waehrend diesem Durchlauf nicht mehr aendert.
	#ifndef REGION_Bestellnummern
	if (lMachineState == MachineState_NormalMode_Bestellnummern)
	{ // 
		DettacheInterrupts(); // Interrupts verhindern
		MyMutex_MenueItems_lock(); // Bereich fuer andere Threads sperren
		gMenue.mSelectedMenueItem->getSubmenueItem(0)->mName = "Warteschange:  " + String(gCocktailMixer.getNumberOfOrdersInQueue(0));
		for (int i = 1; i <= NumberOfSlotsRotateTable; i++)
		{
			if (gCocktailMixer.getSlotBestellnummer(i) != 0)
			{
				gMenue.mSelectedMenueItem->getSubmenueItem(i)->mName = String(i) + ": " + String(gCocktailMixer.getSlotBestellnummer(i)) + " " + gCocktailMixer.getSlotCocktailName(i);
			}
			else
			{
				gMenue.mSelectedMenueItem->getSubmenueItem(i)->mName = String(i) + ": ";
			}
		}
		MyMutex_MenueItems_unlock(); // Bereich freigeben
		AttacheInterrupts(); // Interrupts aktivieren
		delay(100);
	}
	#endif
	#ifndef REGION_Statistik
	if (lMachineState >= 0)
	{
		if (lMachineState == MachineState_NormalMode_Statistik)
		{
			DettacheInterrupts(); // Interrupts verhindern
			MyMutex_MenueItems_lock(); // Bereich fuer andere Threads sperren
			gMenue.mSelectedMenueItem->getSubmenueItem(0)->mName = "Cocktails: " + String(gCocktailMixer.getNumberOfServedCocktails());
			int lNumberOfMaxSigns = 9;
			for (int i = 0; i < gCocktailMixer.mReservoir.getNumberOfReservoir(); i++)
			{
				String lName = gCocktailMixer.mReservoir.getReservoirName(i);
				String lSum = String(gCocktailMixer.mReservoir.getSum(i));
				if (gCocktailMixer.mReservoir.getReservoirName(i).length() > lNumberOfMaxSigns)
				{
					gMenue.mSelectedMenueItem->getSubmenueItem(i + 1)->mName = lName.substring(0, lNumberOfMaxSigns - 1) + ": " + getBlanks(6 - lSum.length()) + lSum + " L";
				}
				else
				{
					gMenue.mSelectedMenueItem->getSubmenueItem(i + 1)->mName = lName + ": " + getBlanks(lNumberOfMaxSigns + 6 - lName.length() - lSum.length()) + lSum + " L";
				}
			}
			MyMutex_MenueItems_unlock(); // Bereich freigeben
			AttacheInterrupts(); // Interrupts aktivieren
		}
	}
	else
	{
		MyMutex_MenueItems_lock(); // Bereich fuer anderen Threads sperren
		int lSelectedPath_0[] = { 0 };  // MixHit_0
		gMenue.selectMenueItem(lSelectedPath_0, 1);
		gMenue.mSelectedMenueItem->mSelectedIndex = 0; // mSelectedIndex auf 0 setzen.
		MyMutex_MenueItems_unlock(); // Bereich wieder freigeben.
	}
	#endif

	gMenue.showMenue(); // Informationen des Menues an Display uebergeben
	gOLED.DisplayLines(); // Informationen anzeigen.
	delay(100);
}



void WriteDefaultConfigFile()
{
	String pFileName = "/Default_Config.txt";
	writeFile(SPIFFS, pFileName, "Vorrarsbehaelter\n\r");
	appendFile(SPIFFS, pFileName, "Nr;Zutatenname;m;b (Wird ein Behaelter nicht benoetigt, wird an dieser Stelle \"leer;1;1\" als Platzhalter eingetragen werden.) (m,b - Parameter zur Umrechnung von ml -> ms - Koennen im Programm ermittelr werden)\n\r");
	appendFile(SPIFFS, pFileName, "Zitrone;22.92;43.15\n\r");
	appendFile(SPIFFS, pFileName, "Grenadine;21.17;100.07\n\r");
	appendFile(SPIFFS, pFileName, "Wodka;18.97;23.92\n\r");
	appendFile(SPIFFS, pFileName, "Orange;25.34;116.12\n\r"); //bissle runter
	appendFile(SPIFFS, pFileName, "Maracuja;25.44;33.35\n\r"); // bissle runter
	appendFile(SPIFFS, pFileName, "Malibu;19.28;34.01\n\r");
	appendFile(SPIFFS, pFileName, "Ananas;20.18;14.41\n\r");
	appendFile(SPIFFS, pFileName, "Banane;20.65;61.31\n\r");
	appendFile(SPIFFS, pFileName, "Mengenangaben\n\r");
	appendFile(SPIFFS, pFileName, "Menge kleines Glas;Menge grosses Glas\n\r");
	appendFile(SPIFFS, pFileName, "120;200\n\r");//250
	appendFile(SPIFFS, pFileName, "Glaeser\n\r");
	appendFile(SPIFFS, pFileName, "Leergewicht - Hier wird nur das erste angegebene Glas eingelesen.\n\r");
	appendFile(SPIFFS, pFileName, "330\n\r");
	appendFile(SPIFFS, pFileName, "Cocktails_Alkohol\n\r");
	appendFile(SPIFFS, pFileName, "Cocktailname;Zutat_1;Menge_1;Zutat_2;Menge_2;...\n\r");
	appendFile(SPIFFS, pFileName, "Wodka-Maracuja;Maracuja;120;Wodka;40\n\r");
	appendFile(SPIFFS, pFileName, "Malibu-Maracuja;Maracuja;100;Malibu;50\n\r");
	appendFile(SPIFFS, pFileName, "Malibu-Orange;Orange;120;Malibu;40\n\r");
	appendFile(SPIFFS, pFileName, "Malibu-Sunrise;Orange;120;Malibu;40;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Screwdriver;Orange;100;Wodka;40\n\r");
	appendFile(SPIFFS, pFileName, "Tropic-Thunder;Maracuja;120;Malibu;40;Wodka;20\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Mimi;Ananas;40;Wodka;40;Malibu;10\n\r");
	appendFile(SPIFFS, pFileName, "Pink-Pussycat;Ananas;120;Wodka;40;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Safari;Wodka;20;Zitrone;20\n\r");
	appendFile(SPIFFS, pFileName, "San-Francisco;Ananas;50;Maracuja;50;Wodka;40;Zitrone;30;Grenadine;20;Orange;10;Banane;10\n\r");
	appendFile(SPIFFS, pFileName, "The-Waikiki;Ananas;120;Wodka;4\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Brassmonkey;Orange;150;Wodka;30\n\r");
	appendFile(SPIFFS, pFileName, "Cocktails_Alkoholfrei\n\r");
	appendFile(SPIFFS, pFileName, "Cocktailname;Zutat_1;Menge_1;Zutat_2;Menge_2;...\n\r");
	appendFile(SPIFFS, pFileName, "Maracuja;Maracuja;100\n\r");
	appendFile(SPIFFS, pFileName, "Orange;Orange;100\n\r");
	appendFile(SPIFFS, pFileName, "Taifruitpunch;Maracuja;50;Orange;50;Ananas;40;Zitrone;10;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Tropic-Sunrise;Banane;40;Ananas;20;Maracuja;20;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Lemon-Cocktail;Zitrone;80;Orange;40\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Sixteen;Orange;80;Ananas;40;Maracuja;20;Zitrone;20;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Redone;Orange;50;Ananas;50;Zitrone;10;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Planterswonder;Orange;60;Ananas;60;Maracuja;60;Zitrone;20;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Targa;Maracuja;60;Orange;60;Ananas;4;Zitrone;30;Grenadine;30\n\r");
	appendFile(SPIFFS, pFileName, "Bananajack;Banane;8;Orange;8;Zitrone;20;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Bora-Bora;Ananas;10;Maracuja;60;Zitrone;10;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Drachenblut;Kirsche;80;Ananas;80;Zitrone;20;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Kiba;Banane;100;Kirsche;100\n\r");
	appendFile(SPIFFS, pFileName, "Luckydriver;Orange;80;Maracuja;40;Zitrone;20;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Ananas-Orange;Ananas;80;Orange;60\n\r");
	appendFile(SPIFFS, pFileName, "Babouin;Kirsche;50;Maracuja;40;Ananas;40;Orange;20;Zitrone;10\n\r");
	appendFile(SPIFFS, pFileName, "Ballerina;Ananas;40;Zitrone;40;Orange;40\n\r");
	appendFile(SPIFFS, pFileName, "Cinderella;Orange;70;Ananas;70;Zitrone;60\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Sweetvictory;Kirsche;110;Ananas;3;Zitrone;10;Grenadine;10\n\r");
	appendFile(SPIFFS, pFileName, "Mix-Sweety;Ananas;80;Orange;80;Grenadine;20\n\r");
	appendFile(SPIFFS, pFileName, "Tropic-Star;Orange;60;Maracuja;60;Ananas;60\n\r");
	appendFile(SPIFFS, pFileName, "Tropical-Orange;Orange;100;Ananas;100;Zitrone;20;Grenadine;20\n\r");
}
void LoadConfigFile(String pFileName)
{
	String lFileContent = "";
	readFile(SPIFFS, pFileName, &lFileContent); // Inhalt der angegebenen Datei lesen.
	if (lFileContent == "")
	{ // Falls der Inhalt der Datei leer ist.
		Serial.println("Datei ist leer! - Default_Config wird geladen");
		WriteDefaultConfigFile(); // Defauult_Config schreiben
		readFile(SPIFFS, "/Default_Config.txt", &lFileContent); // Default_Config einlesen
	}
	Serial.println(lFileContent);

	lFileContent.replace("\r", ""); // Ein Zeilenumbruch besteht meist aus \n\r. Damit dieser auf ein Zeichen reduziert wird, wird \r ggf. entfernt.

	int SelectIndex = -1;
	int Counter = 0;
	vector<String> Lines;
	split(&lFileContent, '\n', &Lines); // Filecontent an den Stellen \n trennen und in ein vektor (vergl. Array mit variabler Groesse) schreiben. (BSP: "A\nB\nC" -> Lines[0] = "A", Lines[1] = "B", Lines[2] = "C", Lines.size() = 3))
	Serial.println("############ SPLIT DONE ##############");
	Serial.print("Lines_Lenght: ");
	Serial.println(Lines.size());
	vector<String> Line;
	for (int i = 0; i < Lines.size(); i++)
	{
		Line = vector<String>(); // Line leeren. (damit keine Eintraege mehr vorhanden sind)
		Counter++;
		#ifndef REGION_Select
		// Hier wird geprueft, an welcher Stelle in der Datei man sich befindet, damit man weiss, was mit dem Inhalt anzufangen ist.
		if (Lines[i] == "Vorrarsbehaelter")
		{
			SelectIndex = 0;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2; // -2: "Vorrarsbehaelter", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		else if (Lines[i] == "Mengenangaben")
		{
			SelectIndex = 1;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2; // -2: "Mengenangaben", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		else if (Lines[i] == "Glaeser")
		{
			SelectIndex = 2;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2; // -2: "Glaeser", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		else if (Lines[i] == "Cocktails_Alkohol")
		{
			SelectIndex = 3;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2; // -2: "Cocktails_Alkohol", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		else if (Lines[i] == "Cocktails_Alkoholfrei")
		{
			SelectIndex = 4;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2; // -2: "Cocktails_Alkoholfrei", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		#endif

		if (SelectIndex == 0 && Counter >= 0)
		{ // Vorratsbehaelter
			split(&Lines[i], ';', &Line); // Zeileninhalt trennen
			if (Line.size() == 1) // Pruefen ob nur der Name des Vorratsbehaelters enthalten ist
			{
				gCocktailMixer.mReservoir.addReservoir(Line[0]);
			}
			else if (Line.size() == 3) // Pruefen ob der Name des Vorratsbehaelters sowie die Parameter m und b enthalten sind
			{
				gCocktailMixer.mReservoir.addReservoir(Line[0], atof(Line[1].c_str()), atof(Line[2].c_str()));
			}
		}
		if (SelectIndex == 1 && Counter >= 0)
		{ // Mengenangaben
			split(&Lines[i], ';', &Line); // Zeileninhalt trennen

			if (Line.size() == 2) // Pruefen ob die Anzahl an Informationen stimmt (Leergewicht und Fuellmaenge).
			{
				gCocktailMixer.mGlasses.setSmallAmount(atof(Line[0].c_str()));
				gCocktailMixer.mGlasses.setBigAmount(atof(Line[1].c_str()));
			}
		}
		if (SelectIndex == 2 && Counter >= 0)
		{ // Glaeser
			split(&Lines[i], ';', &Line); // Zeileninhalt trennen

			if (Line.size() == 1) // Pruefen ob die Anzahl an Informationen stimmt (Leergewicht und Fuellmaenge).
			{
				gCocktailMixer.mGlasses.addGlas(new cGlass(Line[0].toInt(), GlassTollerance));
			}
		}
		if (SelectIndex == 3 && Counter >= 0)
		{ // Cocktails alkoholisch
			// Zeilenaufbau: Cocktailname; Zutat_1; Menge_1; Zutat_2; Menge_2; ...
			split(&Lines[i], ';', &Line); // Zeileninhalt trennen
			
			vector<String> lNames;
			vector<int> lAmount;
			for (int i = 0; i < (Line.size() - 1) / 2; i++)
			{
				lNames.push_back(Line[1 + i * 2]);
				lAmount.push_back(Line[2 + i * 2].toInt());
			}
			bool AddCocktail = true;
			for (int i = 0; i < lNames.size(); i++)
			{
				if (gCocktailMixer.mReservoir.getReservoirIndex(lNames[i]) < 0)
				{
					AddCocktail = false;
					break;
				}
			}
			if (AddCocktail)
			{
				gAlcoholicList.addCocktail(new cCocktail(Line[0], lNames, lAmount));
			}
		}
		if (SelectIndex == 4 && Counter >= 0)
		{ // Cocktails alkoholfrei
			// Zeilenaufbau: Cocktailname; Zutat_1; Menge_1; Zutat_2; Menge_2; ...
			split(&Lines[i], ';', &Line);
			vector<String> lNames;
			vector<int> lAmount;
			for (int i = 0; i < (Line.size() - 1) / 2; i++)
			{
				lNames.push_back(Line[1 + i * 2]);
				lAmount.push_back(Line[2 + i * 2].toInt());
			}
			bool AddCocktail = true;
			for (int i = 0; i < lNames.size(); i++)
			{
				if (gCocktailMixer.mReservoir.getReservoirIndex(lNames[i]) < 0)
				{
					AddCocktail = false;
					break;
				}
			}
			if (AddCocktail)
			{
				gNonAlcoholicList.addCocktail(new cCocktail(Line[0], lNames, lAmount));
			}
		}
	}
}
void create_Menue()
{
	Serial.println("MixHit_0");
	int lSelectedPath_0[] = { 0 };  // MixHit_0
	gMenue.selectMenueItem(lSelectedPath_0, 1);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Einstellungen", Config));

	Serial.println("MixHit_0 - Einstellungen_0");
	int lSelectedPath_00[] = { 0, 0 }; // MixHit_0 - Einstellungen_0
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Vorratsbehaelter", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Reservoir_0");
	int lSelectedPath_000[] = { 0, 0, 0 }; // MixHit_0 - Einstellungen_0 - Reservoir_0
	gMenue.selectMenueItem(lSelectedPath_000, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Anzeigen", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Reservoir_0 - Anzeigen_0");
	int lSelectedPath_0000[] = { 0, 0, 0, 0 }; // MixHit_0 - Einstellungen_0 - Reservoir_0 - Anzeigen_0
	gMenue.selectMenueItem(lSelectedPath_0000, 4);
	for (int i = 0; i < gCocktailMixer.mReservoir.getNumberOfReservoir(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(String(i + 1) + " " + gCocktailMixer.mReservoir.getReservoirName(i), NO_OPERATION));
	}
	Serial.println("MixHit_0 - Einstellungen_0 - Reservoir_0");
	gMenue.selectMenueItem(lSelectedPath_000, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Initialisieren", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Reservoir_0 - Initialisieren_1");
	int lSelectedPath_0001[] = { 0, 0, 0, 1 }; // MixHit_0 - Einstellungen_0 - Reservoir_0 - Initialisieren_1
	gMenue.selectMenueItem(lSelectedPath_0001, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Alle Initialisieren", Config_InitAll));

	for (int i = 0; i < gCocktailMixer.mReservoir.getNumberOfReservoir(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(String(i + 1) + " " + gCocktailMixer.mReservoir.getReservoirName(i), Config_InitSingle));
	}

	Serial.println("MixHit_0 - Einstellungen_0");
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Fuellmaengen", NO_OPERATION));
	
	Serial.println("MixHit_0 - Einstellungen_0 - Fuellmaengen_1");
	int lSelectedPath_001[] = { 0, 0, 1 }; // MixHit_0 - Einstellungen_0 - Fuellmaengen_1
	gMenue.selectMenueItem(lSelectedPath_001, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("klein: " + String(gCocktailMixer.mGlasses.getSmallAmount()), NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("gross: " + String(gCocktailMixer.mGlasses.getBigAmount()), NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0");
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Glaeser", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Glasses_2");
	int lSelectedPath_002[] = { 0, 0, 2 }; // MixHit_0 - Einstellungen_0 - Glasses_2
	gMenue.selectMenueItem(lSelectedPath_002, 3);
	for (int i = 0; i < gCocktailMixer.mGlasses.getNumberOfGlasses(); i++)
	{
		String lText = "Glas_" + String(i + 1) + " " + gCocktailMixer.mGlasses.getGlas(i).getTare() + "g";
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(lText, NO_OPERATION));
	}

	Serial.println("MixHit_0 - Einstellungen_0");
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Cocktails", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Cocktails_3");
	int lSelectedPath_003[] = { 0, 0, 3 }; // MixHit_0 - Einstellungen_0 - Cocktails_3
	gMenue.selectMenueItem(lSelectedPath_003, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Alkohol", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Cocktails_3 - Alkohol_0");
	int lSelectedPath_0030[] = { 0, 0, 3, 0 }; // MixHit_0 - Einstellungen_0 - Cocktails_3 - Alkohol_0
	gMenue.selectMenueItem(lSelectedPath_0030, 4);
	for (int i = 0; i < gAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(gAlcoholicList.getCocktail(i).getCocktailName(), NO_OPERATION));
	}

	Serial.println("MixHit_0 - Einstellungen_0 - Cocktails_3");
	gMenue.selectMenueItem(lSelectedPath_003, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Alkoholfrei", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - Cocktails_3 - Alkoholfrei_1");
	int lSelectedPath_0031[] = { 0, 0, 3, 1 }; // MixHit_0 - Einstellungen_0 - Cocktails_3 - Alkoholfrei_1
	gMenue.selectMenueItem(lSelectedPath_0031, 4);
	for (int i = 0; i < gNonAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(gNonAlcoholicList.getCocktail(i).getCocktailName(), NO_OPERATION));
	}

	Serial.println("MixHit_0 - Einstellungen_0");
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("WiFi", NO_OPERATION));

	Serial.println("MixHit_0 - Einstellungen_0 - WiFi_4");
	int lSelectedPath_004[] = { 0, 0, 4 }; // MixHit_0 - Einstellungen_0 - WiFi_4
	gMenue.selectMenueItem(lSelectedPath_004, 3);
	/*
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("SSID:", NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(USED_SSID, NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("IP:", NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(MY_IP, NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("MAC:", NO_OPERATION));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(MY_MAC, NO_OPERATION));*/


	Serial.println("MixHit_0 - Einstellungen_0");
	gMenue.selectMenueItem(lSelectedPath_00, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Werkseinstellungen", Config_DefaultConfig));

	Serial.println("MixHit_0");
	gMenue.selectMenueItem(lSelectedPath_0, 1);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Betriebsmodus", ModeSelect));

	Serial.println("MixHit_0 - Betriebsmodus_1");
	int lSelectedPath_01[] = { 0, 1 }; // MixHit_0 - Betriebsmodus_1
	gMenue.selectMenueItem(lSelectedPath_01, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Normalbetrieb", NormalMode));
	
	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0");
	int lSelectedPath_010[] = { 0, 1, 0 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0
	gMenue.selectMenueItem(lSelectedPath_010, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Bestellnummern", NormalMode_OrderNumbern));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellnummern_0");
	int lSelectedPath_0100[] = { 0, 1, 0, 0 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellnummern_0
	gMenue.selectMenueItem(lSelectedPath_0100, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Warteschlange:", NO_OPERATION));
	for (int i = 0; i < NumberOfSlotsRotateTable; i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Slot" + String(i), NO_OPERATION));
	}

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0");
	gMenue.selectMenueItem(lSelectedPath_010, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Statistik", NormalMode_Statistik));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Statistik_1");
	int lSelectedPath_0101[] = { 0, 1, 0, 1 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Statistik_1
	gMenue.selectMenueItem(lSelectedPath_0101, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Mixed Cocktails: ", NO_OPERATION));
	for (int i = 0; i < gCocktailMixer.mReservoir.getNumberOfReservoir(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(gCocktailMixer.mReservoir.getReservoirName(i) + ": ", NO_OPERATION));
	}

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0");
	gMenue.selectMenueItem(lSelectedPath_010, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Bestellung aufgeben", NO_OPERATION));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2");
	int lSelectedPath_0102[] = { 0, 1, 0, 2 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2
	gMenue.selectMenueItem(lSelectedPath_0102, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Alkohol", NO_OPERATION));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2 - Alkohol_0");
	int lSelectedPath_01020[] = { 0, 1, 0, 2, 0 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2 - Alkohol_0
	gMenue.selectMenueItem(lSelectedPath_01020, 5);
	for (int i = 0; i < gAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(gAlcoholicList.getCocktail(i).getCocktailName(), NormalMode_OrderCocktailAlcohol));
	}

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2");
	gMenue.selectMenueItem(lSelectedPath_0102, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Alkoholfrei", NO_OPERATION));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2 - Alkoholfrei_1");
	int lSelectedPath_01021[] = { 0, 1, 0, 2, 1 }; // MixHit_0 - Betriebsmodus_1 - Normalbetrieb_0 - Bestellung aufgeben_2 - Alkoholfrei_1
	gMenue.selectMenueItem(lSelectedPath_01021, 5);
	for (int i = 0; i < gNonAlcoholicList.getAmountOfCocktailsInList(); i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem(gNonAlcoholicList.getCocktail(i).getCocktailName(), NormalMode_OrderCocktailNonAlcohol));
	}

	Serial.println("MixHit_0 - Betriebsmodus_1");
	gMenue.selectMenueItem(lSelectedPath_01, 2);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Komponententest", TestMode));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1");
	int lSelectedPath_011[] = { 0, 1, 1 }; // MixHit_0 - Betriebsmodus_1 - Komponententest_1
	gMenue.selectMenueItem(lSelectedPath_011, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Drehteller", TestMode));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Drehteller_1");
	int lSelectedPath_0110[] = { 0, 1, 1, 0 }; // MixHit_0 - Betriebsmodus_1 - Komponententest_1
	gMenue.selectMenueItem(lSelectedPath_0110, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Rechtslauf", TestMode_RotateR));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Linkslauf", TestMode_RotateL));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1");
	gMenue.selectMenueItem(lSelectedPath_011, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Ventile", TestMode_Valve));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Valve_1");
	int lSelectedPath_0111[] = { 0, 1, 1, 1 }; // MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Valve_1
	gMenue.selectMenueItem(lSelectedPath_0111, 4);
	for (int i = 0; i < 8; i++)
	{
		gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Ventil_" + String(i), TestMode_ValveOpen));
	}

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1");
	gMenue.selectMenueItem(lSelectedPath_011, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Waage", TestMode_Scale));
	
	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Waage_2");
	int lSelectedPath_0112[] = { 0, 1, 1, 2 }; // MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Valve_1
	gMenue.selectMenueItem(lSelectedPath_0112, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Tare", TestMode_Scale_Tare));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1");
	gMenue.selectMenueItem(lSelectedPath_011, 3);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Abtropfschutz", NO_OPERATION));

	Serial.println("MixHit_0 - Betriebsmodus_1 - Komponententest_1 - Abtropfschutz_3");
	int lSelectedPath_0113[] = { 0, 1, 1, 3 }; // MixHit_0 - Betriebsmodus_1 - Komponententest_1
	gMenue.selectMenueItem(lSelectedPath_0113, 4);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Position_Darunter", TestMode_Servo0));
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Position_Weg", TestMode_Servo1));

	Serial.println("MixHit_0");
	gMenue.selectMenueItem(lSelectedPath_0, 1);
	gMenue.addSubMenueItemToSelectedMenueItem(new cMenueItem("Reset", ResetESP));
}
void refreshReservoirInfo()
{
	String lConfigFileName;
	readFile(SPIFFS, "/Config_Select.txt", &lConfigFileName); // Ermitteln der aktuellen Config_Datei
	String lFileContent;
	readFile(SPIFFS, lConfigFileName, &lFileContent); // Inhalt der aktuellen Config lesen
	Serial.println(lFileContent);

	lFileContent.replace("\r", ""); // Ein Zeilenumbruch besteht meist aus \n\r. Damit dieser auf ein Zeichen reduziert wird, wird \r ggf. entfernt.

	int SelectIndex = -1;
	int Counter = 0;
	vector<String> Lines;
	split(&lFileContent, '\n', &Lines);
	for (int i = 0; i < Lines.size(); i++)
	{
		Counter++;
		#ifndef REGION_Select
		// Hier wird geprueft, an welcher Stelle in der Datei man sich befindet, damit man weiss, was mit dem Inhalt anzufangen ist.
		if (Lines[i] == "Vorrarsbehaelter")
		{
			SelectIndex = 0;
			Serial.print("SelectIndex: ");
			Serial.println(SelectIndex);
			Counter = -2;  // -2: "Vorrarsbehaelter", -1: Erleuterungen; 0: Erster Eintrag welcher zu verwerten ist.
		}
		else if (Lines[i] == "Glaeser")
		{ // Sobald der Inhalt Glaeser ist, sind alle Vorratsbehaelter vorbei und es kann abgebrochen werden.
			break;
		}
		#endif

		if (SelectIndex == 0 && Counter >= 0)
		{ // Vorratsbehaelter
			Lines[i] = gCocktailMixer.mReservoir.getReservoirName(Counter) + ";" + String(gCocktailMixer.mReservoir.getM(Counter)) + ";" + String(gCocktailMixer.mReservoir.getB(Counter)); // Parameter m und b in die aktuelle Zeile uebernehmen.
		}
	}

	if (lConfigFileName == "/Default_Config.txt") // Default_Config darf nicht �berschrieben werden!
	{
		lConfigFileName = "/User_Config.txt";
	}
	Serial.println(Lines[0] + "\n\r");
	writeFile(SPIFFS, lConfigFileName, Lines[0] + "\n\r"); // Erste Zeile neu schreiben.
	for (int i = 1; i < Lines.size(); i++)
	{
		Serial.println(Lines[i] + "\n\r");
		appendFile(SPIFFS, lConfigFileName, Lines[i] + "\n\r"); // Alle weiteren Zeilen anhaengen.
	}
	writeFile(SPIFFS, "/Config_Select.txt", lConfigFileName); // Damit beim naechsten Start die angepasste Config geladen wird.
}

