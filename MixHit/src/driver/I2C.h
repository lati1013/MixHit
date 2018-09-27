#ifndef _I2C_H_
#define _I2C_H_

#include "../mixer/Configuration.h"
#include "SparkFunSX1509.h" 

extern const byte SX1509_ADDRESS;                                 //SX1509 I2C address (fuer die Waage)
extern SX1509 io;                                                 //Erstellen eines SX1509 Objekts (fuer die Waage)

void Init_SparkFun();

#endif