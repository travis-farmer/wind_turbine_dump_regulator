/* Wind Turbine Dump Regulator
 * by: Travis Farmer (www.tjfhome.net)
 */
#include <Arduino.h>



int maxVolts = 50; // connect secondary dump load, and disconnect charge controller  (max 60v MOSFET)
int regMaxVolts = 45; // upper limit to PWM. full-on dump load.
int regStartVolts = 35; // start PWM to vary dump load.
int vdUpper = 10000; // 10Kohm
int vdLower = 500; // 500ohm
unsigned long timDelay = 500UL; //  fires every half-second
unsigned long timLast = 0UL;

// Hardware Pins
int pinDumpPWM = 2; // to an NPN MOSFET IXFH270N06T3
int pinEmergDisconnect = 3; // to a SPDT relay, where N.C. to EmergDump load, and N.O. to the Charge Controller

int vdCalculator(int inADC, int inUpper, int inLower) {
  int inVolts = (inADC*(5/1024));
  return(inVolts/(inLower/(inUpper+inLower)));
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pinDumpPWM,OUTPUT); digitalWrite(pinDumpPWM,LOW);
  pinMode(pinEmergDisconnect,OUTPUT); digitalWrite(pinEmergDisconnect,LOW);
}

void loop() {
  unsigned long timMillis = millis();
  if ((timMillis - timDelay) >= timLast) {
    timLast = timMillis;
    int curVolts = vdCalculator(analogRead(0),vdUpper,vdLower);
    if (curVolts >= maxVolts) {
      digitalWrite(pinEmergDisconnect,LOW);
      analogWrite(pinDumpPWM,255);
    } else if (curVolts < maxVolts && curVolts >= regMaxVolts) {
      digitalWrite(pinEmergDisconnect,HIGH);
      analogWrite(pinDumpPWM,255);
    } else if (curVolts < regMaxVolts && curVolts >= regStartVolts) {
      digitalWrite(pinEmergDisconnect,HIGH);
      analogWrite(pinDumpPWM,map(curVolts,regStartVolts,regMaxVolts,0,255));
    } else {
      digitalWrite(pinEmergDisconnect,HIGH);
      analogWrite(pinDumpPWM,0);
    }
  }
}
