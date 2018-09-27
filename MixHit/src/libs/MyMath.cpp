// ###############################################################################
// ##																			##
// ##		#ifndef REGION_...													##
// ##		...																	##
// ##		#endif																##
// ##																			##
// ##		Dies dient dazu, um Bereiche zu benamen und hat keine Bedeutung		##
// ##		für den Programmcode. Dadurch wird der Code übersichtlicher			##
// ##		und lesbarer gesteltet. In Visual-Studio können diese Bereiche		##
// ##		auch auf- und zugeklappt werden.									##
// ##																			##
// ###############################################################################

#include "MyMath.h"



double getSUM(double pArray[], int pLenght)
{
	double lReturnValue = 0;
	for (int i = 0; i < pLenght; i++)
	{
		lReturnValue = lReturnValue + pArray[i];
	}
	return lReturnValue;
}
double getSUM2(double pArray[], int pLenght)
{
	double lReturnValue = 0;
	for (int i = 0; i < pLenght; i++)
	{
		lReturnValue = lReturnValue + pArray[i] * pArray[i];
	}
	return lReturnValue;
}
double getSUM(double pArray1[], double pArray2[], int pLenght)
{
	double lReturnValue = 0;
	for (int i = 0; i < pLenght; i++)
	{
		lReturnValue = lReturnValue + pArray1[i] * pArray2[i];
	}
	return lReturnValue;
}

double getSUM(vector<double> *pArray)
{
	double lReturnValue = 0;
	for (int i = 0; i < (*pArray).size(); i++)
	{
		lReturnValue = lReturnValue + (*pArray)[i];
	}
	return lReturnValue;
}
double getSUM2(vector<double> *pArray)
{
	double lReturnValue = 0;
	for (int i = 0; i < (*pArray).size(); i++)
	{
		lReturnValue = lReturnValue + (*pArray)[i] * (*pArray)[i];
	}
	return lReturnValue;
}
double getSUM(vector<double> *pArray1, vector<double> *pArray2)
{
	double lReturnValue = 0;
	for (int i = 0; i < (*pArray1).size(); i++)
	{
		lReturnValue = lReturnValue + (*pArray1)[i] * (*pArray2)[i];
	}
	return lReturnValue;
}

double double_abs(double value) // Berechnet den Betrag einer Variable vom Typ double.
{
	return value >= 0 ? value : -value;
}
bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}
int CharToInt(char c)
{
	if (isDigit(c))
		return c - '0';
}
int StringToInt(String s)
{
	int ReturnValue = 0;
	int VZ = s[0] == '-' ? -1 : 1;
	int StartIndex = VZ == 1 ? 0 : 1;
	for (int i = StartIndex; i < s.length(); i++)
	{
		if (isDigit(s[i]))
		{
			ReturnValue += CharToInt(s[i]) * pow(10, s.length() - 1 - i);
		}
		else
		{
			return -999999;
		}
	}
	return ReturnValue * VZ;
}