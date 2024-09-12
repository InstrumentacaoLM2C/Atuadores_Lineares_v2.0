#include "../include/header.h"
#include "../include/functions.h"


void setup() {
  Serial.begin(9600);

  pinMode(driverEn1, OUTPUT);
  pinMode(driverEn2, OUTPUT);
  pinMode(sensorProximidadeCima, INPUT);
  pinMode(sensorProximidadeBaixo, INPUT);
  pinMode(sensor_posicao, INPUT);

  
  //setting up some default values for maximum speed and maximum acceleration
  stepper1.setMaxSpeed(8000); //SPEED = Steps / second
  stepper1.setAcceleration(1000); //ACCELERATION = Steps /(second)^2
  stepper1.disableOutputs(); //disable outputs, so the motor is not getting warm (no current)

  stepper2.setMaxSpeed(8000); //SPEED = Steps / second
  stepper2.setAcceleration(1000); //ACCELERATION = Steps /(second)^2
  stepper2.disableOutputs(); //disable outputs, so the motor is not getting warm (no current)
  
}

void loop() {
  
  checkSerial();
  continuousRun1();
  continuousRun2();
  posicao_sensor();
  sensor_Proximidade();

}

