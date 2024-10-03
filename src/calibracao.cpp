#include "header.h"
#include "functions.h"
#include <AccelStepper.h>
#include <Arduino.h>


void calibracao(){
  float posAnterior;
  float posMeio;
  float posFinal;
  float posVar; //variação media
  float posVar1; //variação inicial
  float posVar2; //variação final
  float deslocamento_por_pulso;
  char deslocamento_por_pulso_str[15];
  float leituras[11];   // Lista para guardar deslocamentos da calibração
  float variacoes[10];
  parar_calibracao = 0;
  contador = 0;
  digitalWrite(driverEn1, 1);
  Serial.println('j'); //Printa a mensagem no aplicativo do vs code: Calibração iniciada!
  posicao_sensor();
  posAnterior = posicao_calculada_sensor;

  while(contador != 10){    // move o motor 10 vezes
    if(MotorSeMovendo == 0){ //espera o motor parar de se mover para mandar os proxímos comandos
      contador += 1;
      
      runallowed1 = true; //allow running
      modo = 0;
      if(aceleracao1 == 1){
        stepper1.setAcceleration(200); //ACCELERATION = Steps /(second)^2
        
      }
      stepper1.setMaxSpeed(receivedDelay1); //set speed
      if(aceleracao1 == 0){
        stepper1.setAcceleration(20000); //ACCELERATION = Steps /(second)^2
        stepper1.setSpeed(receivedDelay1);
      }
      if(contador < 6){ // na calibração o atuador vai para um lado e depois pro outro
        int temporizador = 0;
        while(temporizador != 30000){
          temporizador += 1;
          posicao_sensor();
          delayMicroseconds(100);
        }
        leituras[contador -1] = posicao_calculada_sensor; 
        posMeio = posicao_calculada_sensor;
        
        stepper1.move(receivedPulsesDistance1*(direcao1)); //set distance  
      }
      else{
        int temporizador = 0;
        while(temporizador != 30000){
          temporizador += 1;
          posicao_sensor();
          delayMicroseconds(100);
        }
        leituras[contador-1] = posicao_calculada_sensor; 
        stepper1.move(receivedPulsesDistance1*(-direcao1));
      }
      
    } 

    checkSerial();
    continuousRun1();
    posicao_sensor();
    sensor_Proximidade();
    if(parar_calibracao == 1){
      pararMotor1();
      parar_calibracao = 0;
      break;
    }
       

  }
  contador = 0;
  while(contador != 1){ //espera o ultimo comando terminar 
    checkSerial();
    continuousRun1();
    posicao_sensor();
    sensor_Proximidade();

    if(MotorSeMovendo == 0){
      contador = 1;
      
      int temporizador = 0;
      while(temporizador != 30000){
        temporizador += 1;
        posicao_sensor();
        delayMicroseconds(100);
      }
      leituras[10] = posicao_calculada_sensor; 
      for (int i = 0; i < 10; ++i) {
          variacoes[i] = abs(leituras[i+1] - leituras[i]); // replace some_number with your modulus base
          
      }

      int soma = 0;
      for (int i = 0; i < 10; ++i) {
          soma += variacoes[i];
      }
      
      posFinal = posicao_calculada_sensor;
      
      posVar1 = abs(posMeio - posAnterior);
      posVar2 = abs(posFinal - posMeio);
      //posVar = (posVar1 + posVar2)/2;
      posVar = soma/10;
      deslocamento_por_pulso = posVar/60000; //A deslocamento por pulso vai ser a distancia que variou durante o trajeto dividido pela quantidade de pulsos enviada (esse 2 aparece pois o driver está enviado o dobro de pulsos q o motor realmente consegue ler)
      dtostrf(deslocamento_por_pulso, 15, 15, deslocamento_por_pulso_str);  // torna o valor em string
      Serial.print("/A constante encontrada foi: ");
      Serial.println( deslocamento_por_pulso_str); 
      
      
    }
    
    if(parar_calibracao == 1){
      pararMotor1();
      parar_calibracao = 0;
      break;
    }
  }
}