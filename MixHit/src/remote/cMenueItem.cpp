#include "cMenueItem.h"
#include "cMenue.h"

cMenueItem::cMenueItem()
{
	mName = "";
	mOK_Function = &NO_OPERATION;
	mSubMenueItems_Count = 0;
	mSelectedIndex = -1;
	mLayer = 0;
	mIndex = 0;
}
cMenueItem::cMenueItem(String pName, OK_Function pOK_Function)
{
	mName = pName;
	mOK_Function = pOK_Function;
	mSubMenueItems_Count = 0;
	mSelectedIndex = -1;
	mLayer = 0;
	mIndex = 0;
}

void cMenueItem::UP()
{
	mSelectedIndex = mSelectedIndex - 1 < 0 ? mSubMenueItems_Count - 1 : mSelectedIndex - 1; // mSelectedIndex um eins erniedrigen (falls < 0 --> maximaler Index)
	drawDisplay(); // Neu Zeichnen des Menues
}
void cMenueItem::DOWN()
{
	mSelectedIndex = (mSelectedIndex + 1) % mSubMenueItems_Count; // mSelectedIndex um eins erhoehen (falls > maximaler Index --> 0)
	drawDisplay(); // Neu Zeichnen des Menues
}
void cMenueItem::BACK()
{
	if (mLayer > 0) // Pruefen ob der mLayer > 0 ist
	{
		mSelectedIndex = -1; // Den mSelectedIndex dieses MenueItems aus -1 zuruecksezen (da dies anschliessend nicht mehr aktiv ist)
		gMenue.mSelectedMenueItem = mParent; // Das uebergeordnete MenueItem ist nun das ausgewaehlte MenueItem.
		gMenue.mSelectedMenueItem->mOK_Function(*gMenue.mSelectedMenueItem); // Ausfuehren der mOK-Funktion des neue ausgewaehlten MenueItems
		mParent->drawDisplay(); // Neu Zeichnen des Menues
	}
}
void cMenueItem::OK()
{
	if (getSubmenueItem(mSelectedIndex)->mSubMenueItems_Count > 0) // Pruefen ob SubMenueItems vorhanden sind
	{ // SubMenueItems vorhanden
		gMenue.mSelectedMenueItem = getSubmenueItem(mSelectedIndex); // Das SubMenueItem mit dem mSelectedIndex als neues ausgewaehltes MenueItem festlegen.
		gMenue.mSelectedMenueItem->mSelectedIndex = 0; // Den mSelectedIndex des neuen ausgewaehlten MenueItems auf 0 setzen.
		gMenue.mSelectedMenueItem->drawDisplay(); // Neu Zeichnen des Menues
	}
	getSubmenueItem(mSelectedIndex)->mOK_Function(*getSubmenueItem(mSelectedIndex)); // mOK_Funktion des neuen ausgewaehlten MenueItems ausfuehren.
}

String cMenueItem::getSubmenueItemName(int pIndex)
{
	return mSubMenueItems->getNextMenueItemName(pIndex);
}
String cMenueItem::getNextMenueItemName(int pIndex)
{
	if (mIndex == pIndex)
	{ // Das MenueItem ist das gesuchte MenueItem.
		return mName;
	}
	else
	{
		if (mIndex < mParent->mSubMenueItems_Count) // Pruefen ob ein nachfolgendes MenueItem vorhanden ist.
		{ // Ein nachfolgendes MenueItem ist vorhanen.
			return mNextMenueItem->getNextMenueItemName(pIndex);
		}
		else
		{ // keines vorhanden
			return ""; // Keinen Namen ausgeben
		}
	}
}
cMenueItem* cMenueItem::getSubmenueItem(int pIndex)
{
	return mSubMenueItems->getNextMenueItem(pIndex);
}
cMenueItem* cMenueItem::getNextMenueItem(int pIndex)
{
	if (mIndex == pIndex)
	{ // Das MenueItem ist das gesuchte MenueItem.
		return this;
	}
	else
	{
		if (mIndex < mParent->mSubMenueItems_Count) // Pruefen ob ein nachfolgendes MenueItem vorhanden ist.
		{ // Ein nachfolgendes MenueItem ist vorhanen.
			return mNextMenueItem->getNextMenueItem(pIndex);
		}
		else
		{ // keines vorhanden
			return 0; // Adrsse 0 (Fehler) zurueckgeben.
		}
	}
}
void cMenueItem::drawDisplay()
{
	int StartIndex = 0;
	if (mSelectedIndex > OLED_MAX_ZEILEN - 2) // Pruefen ob das SubMenueItem mit dem mSelectedIndex im sichtbaren Bereich des Displays ist.
	{ // Auﬂerhalb des Bereiches
		StartIndex = mSelectedIndex - (OLED_MAX_ZEILEN - 2); // StartIndex so anpassen, dass das SubMenueItem mit dem mSelectedIndex noch im sichtbaren Bereich ist (letze sichtbare Zeile).
		StartIndex = StartIndex < 0 ? 0 : StartIndex; // Sicherstellen dass der Startindex nicht keiner als 0 wird.
	}
	else
	{ // im Sichtbaren Bereich
		StartIndex = 0; // StartIndex kann bei 0 beginnen.
	}
	for (int i = 0; i < OLED_MAX_ZEILEN - 1; i++) // Alle sichtbaren Zeilen des Displays durchgehen
	{
		if (i < mSubMenueItems_Count)
		{ // Es sind mehr SubMenueItems vorhanden als der aktuelle Index
			MyMutex_MenueItems_lock(); // Bereich fuer andere Threads sperren
			if (mSelectedIndex != i + StartIndex) // Pruefen ob der Index der mSelectedIndex ist
			{ // Nicht der mSelectedIndex
				gOLED.ChangeLine(" " + getSubmenueItemName(i + StartIndex), i + 1);
			}
			else
			{ // Ist der mSelecedIndex
				gOLED.ChangeLine(">" + getSubmenueItemName(i + StartIndex), i + 1); // Markieren der Zeile mit einem Pfeil
			}
			MyMutex_MenueItems_unlock(); // Bereich freigeben
		}
		else
		{ // Der Index ist groesser als der maximale Index der SubMenueItems
			gOLED.ChangeLine(" ", i + 1); // leeren Platzhalter anzeigen.
		}
	}
}
void cMenueItem::addSubMenueItem(cMenueItem* pMenueItem)
{
	pMenueItem->mParent = this; // Dieses MenueItem ist das ParentMenueItem des als SubMenueItem hinzuzufuegende MenueItem.
	pMenueItem->mLayer = mLayer + 1; // Der Layer des neuen SubMenueItems ist eins hoeher als der dieses MenueItems.
	pMenueItem->mIndex = 0; // Der Index des neuen MenueItems wird vorerst mit 0 initialisiert (dieser wird nachfolgend ggf. noch angepasst.
	if(mSubMenueItems_Count == 0) // Pruefen ob bereits SubMenueItems vorhanden sind.
	{ // keine vorhanden
		mSubMenueItems = pMenueItem; // Das neue MenueItem ist das erste SubMenueItem dieses MenueItems.
		mSubMenueItems_Count++; // Anzahl an SubMenueItems dieses MenueItems um eins erhoehen.
	}
	else
	{ // bereit SubMenueItems vorhanden
		mSubMenueItems->addMenueItem(pMenueItem); // Das neue MenueItem hinten an die Liste der bereits vorhandenen SubMenueItems anhaengen.
		mSubMenueItems_Count++; // Anzahl an SubMenueItems dieses MenueItems um eins erhoehen.
	}
}
void cMenueItem::addMenueItem(cMenueItem* pMenueItem)
{
	if (mIndex == mParent->mSubMenueItems_Count - 1) // Pruefen ob der Index dieses MenueItems der momentan letzte Index des ParentMenueItems ist.
	{ // Dieses MenueItem ist das derzeit letzte.
		// pMenueItem->mIndex = mIndex + 1; // ???
		mNextMenueItem = pMenueItem; // Das neue MeueItem wird nun hinten angehaengt.
	}
	else
	{
		mNextMenueItem->addMenueItem(pMenueItem);
	}
}
cMenueItem* cMenueItem::selectMenueItem(int* pPath, int pNumberOfIndices, int pLayer)
{
	int lSelectedIndex = pPath[pLayer]; // Ermitteln des gewuenschten Index in dieser Ebene
	if (pLayer < pNumberOfIndices) // Pruefen ob der gewuenschte Layer noch gueltig ist
	{ // ist noch gueltig
		if (mLayer == pLayer) // Prueft ob der aktuelle Layer dem angegebenen Layer entspricht
		{
			if (lSelectedIndex == mIndex) // Prueft nach, ob der aktuelle Index dem in dieser Ebene gewuenschte Index entspricht.
			{
				if (pLayer == pNumberOfIndices - 1) // Prueft ob der gewuenschte Layer der letzte gewuenschte Layer ist
				{
					return this; // Ziel MenueItem erreicht
				}
				else
				{
					return mSubMenueItems->selectMenueItem(pPath, pNumberOfIndices, pLayer + 1); // In der nachfolgenden Ebene weiter suchen.
				}
			}
			else
			{
				return mNextMenueItem->selectMenueItem(pPath, pNumberOfIndices, pLayer); // In der aktuellen Ebene weiter suchen.
			}
		}
	}
	else
	{ // ist nicht mehr gueltig
		return 0;
	}
}

