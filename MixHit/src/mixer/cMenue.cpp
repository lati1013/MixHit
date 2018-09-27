#include "cMenue.h"

cMenue gMenue;

cMenue::cMenue()
{
	mMenueItem = new cMenueItem("HitMix_WS1718", NO_OPERATION);
	mMenueItem->mSelectedIndex = 0;
	mSelectedMenueItem = mMenueItem;
}
void cMenue::showMenue()
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		mSelectedMenueItem->drawDisplay();
	}
	else
	{
		Serial.println("ERROR_NULLPOINTER");
	}
}
bool cMenue::UP()
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		mSelectedMenueItem->UP();
		return true;
	}
	else
	{
		return false;
	}
}
bool cMenue::DOWN()
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		mSelectedMenueItem->DOWN();
		return true;
	}
	else
	{
		return false;
	}
}
bool cMenue::BACK()
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		mSelectedMenueItem->BACK();
		return true;
	}
	else
	{
		return false;
	}
}
bool cMenue::OK()
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		mSelectedMenueItem->OK();
		return true;
	}
	else
	{
		return false;
	}
}
bool cMenue::addSubMenueItemToSelectedMenueItem(cMenueItem* pMenueItem)
{
	if (mSelectedMenueItem != 0)
	{ // Fals ein MenueItem ausgewaehlt ist
		pMenueItem->mParent = mSelectedMenueItem; // Das Ausgeawehlte MenueItem wird zum ParentMenueItem des neuen MenueItems (da das neue MenueItem dem ausgewaehlten MenueItem hinzugefuegt wird)
		pMenueItem->mLayer = mSelectedMenueItem->mLayer + 1; // Der Layer des neuen MenueItems ist ein Layer hoeer als der des Aktuellen
		
		if (mSelectedMenueItem->mSubMenueItems_Count == 0) // Prueft, ob bereits SubMenueItems vorhanden sind
		{ //  keine SubMenueItems vorhanden
			pMenueItem->mIndex = 0;
			mSelectedMenueItem->mSubMenueItems = pMenueItem; // das Neue MenueItem wird zum ersten SubMenueItem des Ausgewaehlten MenueItems.
			mSelectedMenueItem->mSubMenueItems_Count++; // Anzahl an SubMenueItems um eins erhoehen.
		}
		else
		{
			pMenueItem->mIndex = mSelectedMenueItem->mSubMenueItems_Count; // Der Index des neuen MenueItems ist der welcher der aktuellen Anzahl an SubmenueItems entspricht.
			mSelectedMenueItem->mSubMenueItems->addMenueItem(pMenueItem); // Das neue MenueItem der Liste an SubMenueItems hinten anhaengen.
			mSelectedMenueItem->mSubMenueItems_Count++; // Anzahl an SubMenueItems um eins erhoehen.
		}
		return true;
	}
	else
	{
		return false;
	}
}
bool cMenue::selectMenueItem(int* pPath, int pNumberOfIndices)
{
	mSelectedMenueItem = mMenueItem->selectMenueItem(pPath, pNumberOfIndices,0); // Waehlt das MenueItem mit dem angegebenen Pfad aus.
	return mSelectedMenueItem == 0 ? false : true; // false, falls kein MenueItem mit dem angegeben Pfad existiert.
}