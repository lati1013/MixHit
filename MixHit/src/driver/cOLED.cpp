#include "cOLED.h"
cOLED gOLED;

cOLED::cOLED()
{
	mdisplay.begin(SSD1306_SWITCHCAPVCC, I2C_OLEDaddress);
	mdisplay.display();
	delay(2000);
	mdisplay.clearDisplay();
	mdisplay.setTextSize(1);
	mdisplay.setTextColor(INVERSE);

	//OLED GEHT senden
	mdisplay.clearDisplay();
	mdisplay.setCursor(0, 0);
	mdisplay.println("Booting...");
	mdisplay.display();
	mNewValue = false;
}
void cOLED::DisplayLines()
{
	MyMutex_mLinesToShow_lock(); // Bereich fuer andere Threads sperren
	if (mNewValue) // Pruefen ob neue Werte vorhanden sind
	{
		for (int i = 0; i < OLED_MAX_ZEILEN; i++) // Alle Werte in das "Save" Array uebernehmen um den Bereich moeglichst schnell wieder freizugeben.
		{
			mLinesToShow_Save[i] = mLinesToShow[i];
		}
		mNewValue = false; // Flag fuer neue Werte zuruecksetzen.
		MyMutex_mLinesToShow_unlock(); // Bereich wieder freigeben

		MyMutex_I2C_lock(); // I2C Bus sperren (der IO-Expander verwendet ebenfalls I2C).
		mdisplay.clearDisplay(); // Display leeren.
		mdisplay.setCursor(0, 0); // Cursor an die erste Stelle setzen.
		for (int i = 0; i < OLED_MAX_ZEILEN; i++)
		{
			mdisplay.println(mLinesToShow_Save[i]); // Zeilen auf das Display schreiben
		}
		mdisplay.display(); // Werte anzeigen.
		MyMutex_I2C_unlock(); // I2C freigeben
	}
	else
	{
		MyMutex_mLinesToShow_unlock(); // Bereich freigeben.
	}
}
bool cOLED::ChangeLine(String pText, int pIndex)
{
	MyMutex_mLinesToShow_lock(); // Bereich fuer andere Threads sperren
	if (pIndex < OLED_MAX_ZEILEN)
	{
		if (pText.length() <= OLED_MAX_SPALTEN) // Pruefen ob der Inhalt des neuen Textes in eine Zeile des Displays passt.
		{ // Passt in eine Zeile
			if (mLinesToShow[pIndex] != pText) // Pruefen ob der neue Text dem alten entspricht
			{ // Entspricht nicht dem alten
				mNewValue = true; // Flag fuer neue Werte setzen.
			}
			mLinesToShow[pIndex] = pText; // Text in die entspraechende Zeile schreiben.
		}
		else
		{ // passt nicht in eine Zeile
			String lText = pText.substring(0, OLED_MAX_SPALTEN - 1); // Abschneiden des neuen Textes
			if (mLinesToShow[pIndex] != lText) // Pruefen ob der neue Text dem alten entspricht
			{ // Entspricht nicht dem alten
				mNewValue = true; // Flag fuer neue Werte setzen.
			}
			mLinesToShow[pIndex] = lText; // Text in die entspraechende Zeile schreiben.
		}
		MyMutex_mLinesToShow_unlock(); // Bereich wieder freigeben
		return true;
	}
	else
	{
		MyMutex_mLinesToShow_unlock(); // Bereich wieder freigeben
		return false;
	}
}
void cOLED::PrintFirstLine(String pString)
{
	ChangeLine(pString, 0);
}

