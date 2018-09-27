#include "cCocktail.h"



cCocktail::cCocktail()
{
	mCocktailName = "";
}
cCocktail::cCocktail(String pCocktailName, vector<String> pNames, vector<int> pAmounts)
{
	mCocktailName = pCocktailName;
	int lTotalAmount = 0; // Hier werden die Maengen aller Zutaten aufsummiert.					
	for (int i = 0; i<pNames.size(); i++)
	{
		mIngredients.push_back(cIngredient(pNames[i], pAmounts[i])); // Zutat der Liste hinzufuegen.
		lTotalAmount = lTotalAmount + pAmounts[i]; // Maenge der Zutat in die Gesamtmaenge hinzufuegen.
	}
	for (int i = 0; i<mIngredients.size(); i++)
	{
		mIngredients[i].setRelAmount(lTotalAmount); // Relative Maenge der Zutaten berechnen (Einzelmaenge / Gesamtmaenge).
	}
}
String cCocktail::getCocktailName()
{
	return mCocktailName;
}
int cCocktail::getNumberOfIngredients()
{
	return mIngredients.size();
}
cIngredient cCocktail::getIngredient(int pIndex)
{
	return mIngredients[pIndex];
}