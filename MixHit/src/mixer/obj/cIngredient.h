#ifndef _CZUTATEN_H_
#define _CZUTATEN_H_

#include "Arduino.h"
#include "../Configuration.h"
#include "../../libs/MyString.h"

class cIngredient
{
public:
	cIngredient();
	cIngredient(String pName, int pAmount);
	String getName();
	int getAmount();
	double getRelAmount();
	void setRelAmount(int pTotalAmount);
private:
	String mName;
	int mAmount;
	double mRelAmount;
};


#endif