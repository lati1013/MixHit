#include "MyString.h"

void split(String* pText, char pSeparator, vector<String>* pResult)
{
	(*pResult).clear();
	int lIndex = (*pText).indexOf(pSeparator);
	int Zeile = 0;
	while (lIndex >= 0)
	{
		//Serial.print("Zeile: ");
		//Serial.println(Zeile);
		//Serial.println((*pText).substring(0, lIndex));
		(*pResult).push_back((*pText).substring(0, lIndex));
		(*pText).remove(0, lIndex + 1);
		Zeile++;
		lIndex = (*pText).indexOf(pSeparator);
	}
	//Serial.print("Rest_Lenght: ");
	//Serial.println((*pText).length());
	if ((*pText).length() > 0)
	{
		//Serial.print("Zeile: ");
		//Serial.println(Zeile);
		//Serial.println((*pText));
		(*pResult).push_back((*pText));
		(*pText).remove(0,(*pText).length());
		Zeile++;
	}
}
String getBlanks(int pNumber)
{
	String ReturnString = "";
	for (int i = 0; i < pNumber; i++)
	{
		ReturnString = ReturnString + " ";
	}
	return ReturnString;
}
String getStringBetween(String &pContent, String pStartString, String pEndString)
{
	int lStartIndex = pContent.indexOf(pStartString);
	int lEndIndex = pContent.indexOf(pEndString, lStartIndex + pStartString.length());
	if (lStartIndex > 0 && lEndIndex > lStartIndex)
	{
		return pContent.substring(lStartIndex + pStartString.length(), lEndIndex);
	}
	else
	{
		return "";
	}
}