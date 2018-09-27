#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include "WString.h"
#include "Arduino.h"
#include "vector"
#include <stdlib.h>

using namespace std;

void split(String* pText, char pSeparator, vector<String>* pResult);
String getBlanks(int pNumber);
String getStringBetween(String &pContent, String pStartString, String pEndString);

#endif