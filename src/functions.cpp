#include "functions.h"
#include <AccelStepper.h>
#include <Arduino.h>

void continuousRun1() //method for the motor 1
{
  if (runallowed1 == true)
  {
    if (abs(stepper1.currentPosition()) < receivedPulsesDistance1) //abs() is needed because of the '<'
    {
      MotorSeMovendo = 1;
      stepper1.enableOutputs(); //enable pins
      stepper1.run(); //step the motor (this will step the motor by 1 step at each loop)
    }
    
    else if ((abs(stepper1.currentPosition()) < receivedPulsesDistance1) && (modo == 1)) //abs() is needed because of the '<'
    { 
      MotorSeMovendo = 1;
      stepper1.enableOutputs(); //enable pins
      stepper1.runSpeedToPosition(); //step the motor (this will step the motor by 1 step at each loop)
    }
    else //program enters this part if the required distance is completed
    {
      runallowed1 = false; //disable running -> the program will not try to enter this if-else anymore
      stepper1.disableOutputs(); // disable power
      stepper1.setCurrentPosition(0); //reset the position to zero
      MotorSeMovendo = 0;
    }
  }
  else //program enters this part if the runallowed1 is FALSE, we do not do anything
  {
    return;
  }
}

void continuousRun2() //method for the motor 2
{
  if (runallowed2 == true)
  {
    if (abs(stepper2.currentPosition()) < receivedPulsesDistance2) //abs() is needed because of the '<'
    {
      MotorSeMovendo = 1;
      stepper2.enableOutputs(); //enable pins
      stepper2.run(); //step the motor (this will step the motor by 1 step at each loop)
    }
    
    else if ((abs(stepper2.currentPosition()) < receivedPulsesDistance2) && (modo == 1)) //abs() is needed because of the '<'
    { 
      MotorSeMovendo = 1;
      stepper2.enableOutputs(); //enable pins
      stepper2.runSpeedToPosition(); //step the motor (this will step the motor by 1 step at each loop)
    }
    else //program enters this part if the required distance is completed
    {

      runallowed2 = false; //disable running -> the program will not try to enter this if-else anymore
      stepper2.disableOutputs(); // disable power
      stepper2.setCurrentPosition(0); //reset the position to zero
      MotorSeMovendo = 0;
    }
 
 
  }
  else //program enters this part if the runallowed2 is FALSE, we do not do anything
  {
    
    return;
 
  }
}

void posicao_sensor() {
  // Get the current time
  unsigned long currentMillis = millis();
  
  // Check if it's time to take a new reading
  if (time_now == -1 || currentMillis - time_now >= interval) {
    // Update the last reading time
    time_now = currentMillis;
    
    // Initialize variables
    int reading = 0;
  
    // Read sensor value
    reading = analogRead(sensor_posicao);
  
    // Subtract the oldest reading from the total
    total -= readings[index];
  
    // Add the new reading to the total
    total += reading;
  
    // Store the new reading in the array
    readings[index] = reading;
  
    // Increment index for the next reading
    index = (index + 1) % numReadings;
  
    // Calculate the average reading
    int averageReading = total / numReadings;
  
    // Calculate position using the average reading
    posicao_calculada_sensor = zero_laser + averageReading / coeficiente_linear_laser;
  
    // Convert position to string
    String posicao_calculada_Sensor_Str = String(posicao_calculada_sensor);
    String readingStr = String(averageReading);
    // Print the position
    Serial.println('l' + posicao_calculada_Sensor_Str); 
  }
}

void sensor_Proximidade(){
  int sensorC = digitalRead(sensorProximidadeCima);
  int sensorB = digitalRead(sensorProximidadeBaixo);


  if((sensorC || sensorB) && motorParou1 == 0){
    pararMotor1();
    motorParou1 = 1;
    parar_calibracao = 1;
    Serial.println("/O motor foi travado pelo sensor indutivo!");
    Serial.println('T'); //Mude a direção do deslocamento para movimenta-lo.
  }
}

void pararMotor1(){
      runallowed1 = false; //disable running
      stepper1.setCurrentPosition(0); // reset position
      stepper1.stop(); //stop motor
      stepper1.disableOutputs(); //disable power
      digitalWrite(driverEn1, 0);
      Serial.println("/Motor 1 desligado!");
      MotorSeMovendo = 0;
      contador = 3;
}

void pararMotor2(){
      runallowed2 = false; //disable running
      stepper2.setCurrentPosition(0); // reset position
      stepper2.stop(); //stop motor
      stepper2.disableOutputs(); //disable power
      digitalWrite(driverEn2, 0);
      Serial.println("/Motor 2 desligado!");
      MotorSeMovendo = 0;
      contador = 3;
}
  
void moverAcelerado1(){//função que muda o modo de aceleração do motor
      stepper1.setAcceleration(1000); //ACCELERATION = Steps /(second)^2
      runallowed1 = true; //allow running
      modo = 0;
      stepper1.setMaxSpeed(receivedDelay1); //set speed
      stepper1.move(receivedPulsesDistance1*direcao1); //set distance
}

void moverAcelerado2(){//função que muda o modo de aceleração do motor
  stepper1.setAcceleration(1000); //ACCELERATION = Steps /(second)^2
      runallowed2 = true; //allow running
      modo = 0;
      stepper2.setMaxSpeed(receivedDelay2); //set speed
      stepper2.move(receivedPulsesDistance2*direcao2); //set distance
}

void moverUniforme1(){//função que muda o modo de aceleração do motor
      stepper1.setAcceleration(20000); //ACCELERATION = Steps /(second)^2
      stepper1.setMaxSpeed(receivedDelay1); //set speed
      stepper1.setSpeed(receivedDelay1);
      stepper1.move(receivedPulsesDistance1*direcao1); //set distance
      runallowed1 = true; //allow running
      modo = 1;
}

void moverUniforme2(){//função que muda o modo de aceleração do motor
      stepper2.setAcceleration(20000); //ACCELERATION = Steps /(second)^2
      stepper2.setMaxSpeed(receivedDelay2); //set speed
      stepper2.setSpeed(receivedDelay2);
      stepper2.move(receivedPulsesDistance2*direcao2); //set distance
      runallowed2 = true; //allow running
      modo = 1;
}

void subsidencia() { // Função que movimenta o motor para frente e para trás (2 voltas completas)
    // Configura a velocidade máxima e a aceleração do motor de passo
  stepper1.setMaxSpeed(60000);
  stepper1.setAcceleration(3600000);

  // Movimento no sentido positivo
  stepper1.moveTo(1200);
  stepper1.runToPosition();
  delay(50); 

  // Movimento no sentido negativo
  stepper1.moveTo(-1200);
  stepper1.runToPosition();
  delay(50);    
}