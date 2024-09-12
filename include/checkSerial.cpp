#include "../include/header.h"
#include "functions.h"
#include <AccelStepper.h>
#include <Arduino.h>

void checkSerial() //method for receiving the commands
{
    if(Serial.available()){
    data = Serial.readStringUntil('#');
    d1 = data.charAt(0);

    switch(d1){         //select action based on the first character

      case 'A':         //first character is an A = turn-on/energize motor
        if(motor == '1'){
          digitalWrite(driverEn1, 1);
          Serial.println("/Motor 1 ligado!");
        }
        else if(motor == '2'){
          digitalWrite(driverEn2, 1);
          Serial.println("/Motor 2 ligado!");
        }
      break;

      case 'a':         //first character is an a = turn-off/de-energize motor
        if(motor == '1'){
          digitalWrite(driverEn1, 0);
          Serial.println("/Motor 1 desligado!");
        }
        else if(motor == '2'){
          digitalWrite(driverEn2, 0);
          Serial.println("/Motor 2 desligado!");
        }
      break;
      
      case 'C':         //First character is an C = The equipment goes up

        if(motor == '1'){
          Serial.println("c"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
          direcao1 = -1;
        }
        else if(motor == '2'){
          Serial.println("C"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
          direcao2 = -1;
        }
      break;

      case 'B':        //First character is an B = The equipment goes down

        if(motor == '1'){
          Serial.println("b"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
          direcao1 = 1;
        }
        else if(motor == '2'){
          Serial.println("B"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
          direcao2 = 1;
        }

      break;

      case 'P':         //First character is an P = set qntd of pulses to send
        // a cada pulso se movimenta 0,6 mm -> conta: (1,8 \ (3 * 4)) * 4mm do passo
        x = data.substring(1);
        if(motor == '1'){
          receivedPulsesDistance1 = x.toFloat(); //value for the steps
          Serial.print("/Pulsos motor 1: ");
          Serial.println(receivedPulsesDistance1);
        }
        else if(motor == '2'){
          receivedPulsesDistance2 = x.toFloat(); //value for the steps
          Serial.print("/Pulsos motor 2: ");
          Serial.println(receivedPulsesDistance2);

        }
      break;

      case 'G':     //First character is an G = motor accelerates
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo com aceleração!
          moverAcelerado1();
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo com aceleração!
          moverAcelerado2();
        }

      
      break;

      case 'X':
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo com aceleração!
          aceleracao1 = 1;
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo com aceleração!
          aceleracao2 = 1;
        }
      break;

      case 'x':
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo sem aceleração!
          aceleracao1 = 0;
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo sem aceleração!
          aceleracao2 = 0;
        }
      break;


      case 'H':     //First character is an H = motor spins with constant speed
        if(motor == '1'){
          Serial.println("/O motor 1 se move com velocidade constante!");
          moverUniforme1();
          
        }
        else if(motor == '2'){
          Serial.println("/O motor 2 se move com velocidade constante!");
          moverUniforme2();

        }
      break;
      
      case 'V':         //First character is an V = set velocity
        x = data.substring(1);
        y = x.toFloat();
        if(200<y<8000){
          if(motor == '1'){  
            receivedDelay1 = y;
            Serial.println("/Velocidade do motor 1: " + x + " Pulsos por segundo");


          }
          else if(motor == '2'){
            Serial.println("/Velocidade do motor 2: " + x + " Pulsos por segundo");

            receivedDelay2 = y;
          }
        }
        else{
          Serial.println("Q"); //Printa a mensagem no aplicativo do vs code: "Valor de velocidade inválido! Insira um valor entre 200 e 8000 pulsos/segundo
        }

      break;

      case 'n': //para o motor
      
        if(motor == '1'){
          pararMotor1();
        }
        else if(motor == '2'){
          paraMotor2();
        }
      break;
      
      case 'N': //para calibração
        parar_calibracao = 1;

      break;

      case 'O': //  Insere um valor para a posição calculada do primeiro motor. 
        delayMicroseconds(1000); 
        x = data.substring(1);
        posicao_calculada1 = x.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr1  = String(posicao_calculada1);
        Serial.println('p'+ posicao_calculadaStr1);
      break;

      case 'o': //  Insere um valor para a posição calculada do segundo motor. 
        delayMicroseconds(1000); 
        x = data.substring(1);
        posicao_calculada2 = x.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr2  = String(posicao_calculada2);
        Serial.println('P'+ posicao_calculadaStr2);
      break;

      case 'J': //sets intercept of the laser

        x = data.substring(1);
        zero_laser = x.toFloat();
      break;
      
      
      case 'I': // Inicia o processo de calibração
        calibracao();

      break;

      case 'U': //Insere constante de calibração dos motores
        x = data.substring(1);
        if(motor == 1){
          constanteCalibracao1 = x.toFloat();
        }
        else{
          constanteCalibracao2 = x.toFloat();
        }

        Serial.print("w"); //Printa a constante de calibração no app do VSCode 
        Serial.println(x);
      break;



      case 'S': // ativa a funcionalidade do sensor indutivo novamente
        motorParou1 = 0;
        Serial.print("\\Sensor indutivo ativado"); //Printa a constante de calibração no app do VSCode 
      break;

      case 'M': //Função para mudar qual motor está sendo utilizado.

        motor = '2';
        Serial.println('u'); //Segundo motor sendo operado!
      break;

      case 'R': //Função para mudar qual motor está sendo utilizado.
        motor = '1';
        Serial.println('U');//Primeiro motor sendo operado!
      break;

      case 'K':
        subsidencia(); // Função que movimenta o motor para frente e para trás (2 voltas completas) ativando o mecanismo de subsidência
      break;   
    }
  }
}