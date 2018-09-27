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

#ifndef _MYMATH_H_
#define _MYMATH_H_

#include <math.h>
#include "Wire.h"
#include "MyString.h"

using namespace std;

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

double getSUM(double pArray[], int pLenght);
double getSUM2(double pArray[], int pLenght);
double getSUM(double pArray1[], double pArray2[], int pLenght);

double getSUM(vector<double> *pArray);
double getSUM2(vector<double> *pArray);
double getSUM(vector<double> *pArray1, vector<double> *pArray2);

double double_abs(double value); // Berechnet den Betrag einer Variable vom Typ double.
bool isDigit(char c);
int CharToInt(char c);
int StringToInt(String s);
#endif