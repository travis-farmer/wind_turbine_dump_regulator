/* Wind Turbine Dump Regulator
 * by: Travis Farmer (www.tjfhome.net)
 */
#include <ModbusRTUSlave.h>

const uint8_t dePin = 2;
ModbusRTUSlave modbus(Serial, dePin);
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
bool coils[1];
bool discreteInputs[1];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

void setup() {
  // put your setup code here, to run once:
  pinMode(pinDumpPWM,OUTPUT); digitalWrite(pinDumpPWM,LOW);
  pinMode(dePin,OUTPUT); digitalWrite(dePin,LOW);
  Serial.begin(9600);
  modbus.configureCoils(coils, 1);                       // bool array of coil values, number of coils
  modbus.configureDiscreteInputs(discreteInputs, 1);     // bool array of discrete input values, number of discrete inputs
  modbus.configureHoldingRegisters(holdingRegisters, 2); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.configureInputRegisters(inputRegisters, 2);     // unsigned 16 bit integer array of input register values, number of input registers
  modbus.begin(4, 9600); 
}

void loop() {
  unsigned long timMillis = millis();
  if ((timMillis - timDelay) >= timLast) {
    timLast = timMillis;
    if (holdingRegisters[0] > 0) regStartVolts = holdingRegisters[0];
    if (holdingRegisters[1] > 0) regMaxVolts = holdingRegisters[1];
    
    int curVolts = vdCalculator(analogRead(0),vdUpper,vdLower);
    int curPwm = map(curVolts,regStartVolts,regMaxVolts,0,255);
    analogWrite(pinDumpPWM,curPwm);
    inputRegisters[0] = curVolts;
    inputRegisters[1] = curPwm;
  }
  
  
  modbus.poll();
}
