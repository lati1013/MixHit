#ifndef _MAIN_COCKTAILMIXER_H_
#define _MAIN_COCKTAILMIXER_H_

#include "Arduino.h"
#include "Wire.h"
#include "../libs/MyString.h"
#include "../remote/cMenue.h"
#include "cCocktailMixer.h"
#include "SparkFunSX1509.h" 
#include "collections/cCocktailList.h"
#include "SPIFFS.h"
#include "../driver/FileOperations.h"
#include "../libs/MyMath.h"
#include "obj/cGlass.h"



void setup_CocktailMixer();	// Wird beim Start des ESP32 ausgefuehrt.
void loop_CocktailMixer();	// Wird in einer Dauerschleife zyklisch aufgerufen.
void loop_OLED();			// Wird in einer Dauerschleife zyklisch aufgerufen.

void create_Menue();						// Erstellt das Menue welches auf dem OLED-Dysplay angezeigt wird.
void WriteDefaultConfigFile();				// Schreibt die Daten in die /Default_Config.txt
void LoadConfigFile(String pFileName);		// Laed die angegebene Datei.
void refreshReservoirInfo();				// Laed die aktuelle Konfiguration, aendert die Parameter (m,b) der Vorratsbehaelter und speichert diese wieder ab.		

#endif