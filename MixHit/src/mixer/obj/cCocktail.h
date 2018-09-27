#ifndef _CCOCKTAIL_H_
#define _CCOCKTAIL_H_

#include "cIngredient.h"
#include "../Configuration.h"

class cCocktail
{
public:
	cCocktail();									// Standatkonstruktor
	cCocktail(String pCocktailName, vector<String> pNames, vector<int> pAmounts); // Konstruktor mit Informationen
	String getCocktailName();						// Gibt den Namen des Cocktails aus.
	int getNumberOfIngredients();					// Gibt die Anzahl an verwendeten Zutaten aus.
	cIngredient getIngredient(int pIndex);			// Gibt die Zutat mit dem entsprechenden Index aus.
private:
	vector<cIngredient> mIngredients;				// Verwendete Zutaten.
	String mCocktailName;							// Name des Cocktails
};
#endif