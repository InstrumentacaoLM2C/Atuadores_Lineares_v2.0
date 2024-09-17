#include "functions.h"

//Variaveis para comunicação serial

char receivedCommand; //character for commands
String dadoPulsos, dadoVelocidade, dadoPosicao, dadoLaser, dadoCteCalibracao, dadosMotor; //variavél usada para receber strings pelo serial
float y, velocidade;//variavél usada para receber floats

String data;
char d1; 


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
        dadoPulsos = data.substring(1);
        if(motor == '1'){
          receivedPulsesDistance1 = dadoPulsos.toFloat(); //value for the steps
          Serial.print("/Pulsos motor 1: ");
          Serial.println(receivedPulsesDistance1);
        }
        else if(motor == '2'){
          receivedPulsesDistance2 = dadoPulsos.toFloat(); //value for the steps
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
        dadoVelocidade = data.substring(1);
        velocidade = dadoVelocidade.toFloat(); //recebe velocidade e transforma em float
        if(motor == '1'){  
          receivedDelay1 = velocidade;
          Serial.println("/Velocidade do motor 1: " + dadoVelocidade + " Pulsos por segundo");}
          
        else if(motor == '2'){
          Serial.println("/Velocidade do motor 2: " + dadoVelocidade + " Pulsos por segundo");
          receivedDelay2 = velocidade;
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
        dadoPosicao = data.substring(1);
        posicao_calculada1 = dadoPosicao.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr1  = String(posicao_calculada1);
        Serial.println('p'+ posicao_calculadaStr1);
      break;

      case 'o': //  Insere um valor para a posição calculada do segundo motor. 
        delayMicroseconds(1000); 
        dadoPosicao = data.substring(1);
        posicao_calculada2 = dadoPosicao.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr2  = String(posicao_calculada2);
        Serial.println('P'+ posicao_calculadaStr2);
      break;

      case 'J': //sets intercept of the laser

        dadoLaser = data.substring(1);
        zero_laser = dadoLaser.toFloat();
      break;
      
      
      case 'I': // Inicia o processo de calibração
        calibracao();

      break;

      case 'U': //Insere constante de calibração dos motores
        dadoCteCalibracao = data.substring(1);
        if(motor == 1){
          constanteCalibracao1 = dadoCteCalibracao.toFloat();
        }
        else{
          constanteCalibracao2 = dadoCteCalibracao.toFloat();
        }

        Serial.print("w"); //Printa a constante de calibração no app do VSCode 
        Serial.println(dadoCteCalibracao);
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

      case 'T': // recebe todas as informações do motor de uma vez e aciona o motor
        dadosMotor = data.substring(1);

        //código para separar as strings 
        int firstSeparatorIndex = dadosMotor.indexOf(';');
        int secondSeparatorIndex = dadosMotor.indexOf(';', firstSeparatorIndex + 1);
        int thirdSeparatorIndex = dadosMotor.indexOf(';', secondSeparatorIndex + 1);

        // Extract substrings based on the positions of the separators
        String pulso = dadosMotor.substring(0, firstSeparatorIndex); // "primeiro numero"
        String velocidade = dadosMotor.substring(firstSeparatorIndex + 1, secondSeparatorIndex); // "segundo numero"
        String direcao = dadosMotor.substring(secondSeparatorIndex + 1, thirdSeparatorIndex); // "caracter B ou C"
        String mover = dadosMotor.substring(thirdSeparatorIndex + 1); // "caracter H ou x"

        
        if(motor == '1'){
          //liga motor
          digitalWrite(driverEn1, 1);
          Serial.println("/Motor 1 ligado!");
          //recebe pulsos
          receivedPulsesDistance1 = pulso.toFloat(); //value for the steps
          Serial.print("/Pulsos motor 1: ");
          Serial.println(receivedPulsesDistance1);
          //recebe velocidade
          receivedDelay1 = velocidade.toFloat();
          Serial.println("/Velocidade do motor 1: " + velocidade + " Pulsos por segundo");
          if(direcao == "B"){
            Serial.println("b"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
            direcao1 = 1;
          }
          else if(direcao == "C"){
              Serial.println("c"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
              direcao1 = -1;
          }
          if(mover == "H"){moverUniforme1();}

          
        }
        else if(motor == '2'){
          //liga motor
          digitalWrite(driverEn2, 1);
          Serial.println("/Motor 2 ligado!");
          //recebe pulsos
          receivedPulsesDistance2 = pulso.toFloat(); //value for the steps
          Serial.print("/Pulsos motor 2: ");
          Serial.println(receivedPulsesDistance2);
          //recebe velocidade
          Serial.println("/Velocidade do motor 2: " + dadosMotor + " Pulsos por segundo");
          receivedDelay2 = velocidade.toFloat();
          if(direcao == "B"){
            Serial.println("B"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
            direcao2 = 1;
          }
          else if(direcao == "C"){
              Serial.println("C"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
              direcao1 = -1;
          }
          if(mover == "H"){moverUniforme2();}
          
        }
      break;   
    }
  }
}