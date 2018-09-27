#ifndef _OLED_H_
#define _OLED_H_

#include "Arduino.h"
#include "../mixer/Configuration.h"
#include "Adafruit_GFX.h"		//OLED
#include "Adafruit_SSD1306.h"
#include "../libs/MyMutex.h"

class cOLED
{
public:
	cOLED();									// Standartkonstruktor
	bool ChangeLine(String pText, int pIndex);	// Aendert den Inhalt der angegebenen Zeile (zeigt diese aber noch nicht auf dem Display an).
	void DisplayLines();						// Zeigt die hinterlegten Zeilen auf dem Display an.
	void PrintFirstLine(String pString);		// Aendert den Inhalt der ersten Zeile (zeigt diese aber noch nicht auf dem Display an).
private:
	Adafruit_SSD1306 mdisplay;					// Display
	String mLinesToShow[OLED_MAX_ZEILEN];		// Zeilen welche angezeigt werden sollen.
	String mLinesToShow_Save[OLED_MAX_ZEILEN];	// Zeilen welche angezeigt werden (naehre Informationen in der .cpp).
	bool mNewValue;								// Gibt an, ob es neue Informatonen gibt welche angezeigt werden muessen.
};
extern cOLED gOLED;
#endif