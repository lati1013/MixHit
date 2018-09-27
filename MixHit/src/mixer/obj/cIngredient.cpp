#include "cIngredient.h"

cIngredient::cIngredient()
{
	mName = "";
	mAmount = 0;
	mRelAmount = 0;
}
cIngredient::cIngredient(String pName, int pAmount)
{
	mName = pName;
	mAmount = pAmount;
	mRelAmount = 0; // Relative Maenge wird vorerst mit 0 initialisiert und spaeter richtig pestimmt.
}
String cIngredient::getName()
{
	return mName;
}
int cIngredient::getAmount()
{
	return mAmount;
}
double cIngredient::getRelAmount()
{
	return mRelAmount;
}
void cIngredient::setRelAmount(int pTotalAmount)
{
	if(pTotalAmount != 0) // Division mit 0 vermeiden
		mRelAmount = ((double)mAmount) / pTotalAmount;
	else
		mRelAmount = 0;
}