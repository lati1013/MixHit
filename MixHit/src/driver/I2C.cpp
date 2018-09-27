#include "I2C.h"

const byte SX1509_ADDRESS = I2C_SX1509address;                                 //SX1509 I2C address
SX1509 io;

void Init_SparkFun()
{
	static bool IS_INIT = false;
	if (!IS_INIT)
	{
		delay(1000);
		if (!io.begin(SX1509_ADDRESS))
		{
			Serial.print(SX1509_ADDRESS);
			Serial.println("I2C_Problem");
			return;                                                   //If we fail to communicate, loop forever.
		}
		else
		{
			Serial.println("I2C_OK");
		}
		io.clock(INTERNAL_CLOCK_2MHZ, 4);

		IS_INIT = true;
	}
}