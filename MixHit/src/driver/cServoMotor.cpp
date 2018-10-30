#include "cServoMotor.h"

cServoMotor::cServoMotor()
{

}
cServoMotor::cServoMotor(int pServoPin)
{
	mPinNummer = pServoPin;                      //Der Pin mit dem der Servo verbunden ist wird definiert
	mServo.attach(mPinNummer);                   //Der Pin wird dem Servo zugewiesen
}
void cServoMotor::Aktivieren()
{
	mServo.write(ServoPosAktiv);                 //Winkelposition in Grad (°) wird angefahren
}
void cServoMotor::Deaktivieren()
{
	mServo.write(ServoPosDeaktiv);               //Winkelposition in Grad (°) wird angefahren   
}