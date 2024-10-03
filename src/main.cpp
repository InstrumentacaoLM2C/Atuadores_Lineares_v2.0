#include <Arduino.h>
#include <string.h>
#include <AccelStepper.h>
#include "functions.h"
#include "header.h"

  //Variaveis para comunicação serial

char receivedCommand; //character for commands
String x; //variavél usada para receber strings pelo serial
float y;//variavél usada para receber floats
bool newData, runallowed1, runallowed2 = false; // booleans for new data from serial, and runallowed flag
String data;
char d1; 

  //Variaveis para sensores

int sensorProximidadeCima = 2;  //pin do sensor de proximidade da direita
int sensorProximidadeBaixo = 3; //pin do sensor de proximidade da esquerda
int modo; //modo = 0 (velocidade constante), modo = 1 (aceleração)
int velocidadeLinear;


//variaveis para a leitura do laser fazendo média
String posicao_calculada_Sensor_Str;
float posicao_calculada_sensor;
const int numReadings = 40; // Number of readings to average
int readings[numReadings];   // Array to store sensor readings
int index = 0;               // Index for the readings array
int total = 0;               // Total sum of readings
unsigned long time_now = -1; // Variable to store the last time a reading was taken
const unsigned long interval = 50; // Interval between readings in milliseconds
int sensor_posicao = A4;
float zero_laser = 50;
float coeficiente_linear_laser = 4.350738916; // adquirido empiricamente(antes era 4.397818611 ou 4.359860704)

    //Configurações dos motores

// Variaveis gerais


long receivedAcceleration = 0; //acceleration value from computer ( variavel ainda não está sendo usada, aceleração, quando utilizada, tem um valor default de 1000 Steps /(second)^2)
char motor = '1'; // variavel para escolher entre o primeiro ou o segundo motor ou os dois ao mesmo tempo. 1 -> motor 1;  2 -> motor 2; 3 -> motor 1 e 2; 
int MotorSeMovendo = 0; //variavel para saber o estado do motor; se o motor não está se movendo = 0; se o motor está se movendo = 1 (variavel utilizada apenas na calibração)
int contador = 0;  //(variavel utilizada apenas na calibração)
bool parar_calibracao;



//Definindo pinos

// Primeiro motor

// Direção - = Digital 9 (CCW), Pulsos - = Digital 8 (CLK)
AccelStepper stepper1(1, 7, 8);
int direcao1 = 1; // Direção = 1 (CCW) Direção = -1 (CW)
int driverEn1 = 9; //en- pin

int aceleracao1 = 1; //indica se o motor se move com aceleração ou não
long receivedDelay1 = 0; //delay between two steps, received from the computer
long receivedPulsesDistance1 = 0; //distance in pulses from the computer
float distanceMM1; //Distancia em MM 
float constanteCalibracao1 = 0.0002222;
int motorParou1 = 0; // variavel para saber se o motor foi parado pelo sensor indutivo. se seu valor for 0 então não foi.
float posicao_calculada1 =0; // Armazena posição calculada por pulso
String posicao_calculadaStr1;

// Segundo motor

// Direção - = Digital 5 (CCW), Pulsos - = Digital 4 (CLK)
AccelStepper stepper2(1, 4, 5);
int direcao2 = 1; // Direção = 1 (CCW) Direção = -1 (CW)
int driverEn2 = 6; //en- pin 

int aceleracao2 = 1; //indica se o motor se move com aceleração ou não
long receivedPulsesDistance2 = 0; //distance in pulses from the computer
float distanceMM2; //Distancia em MM 
float constanteCalibracao2 = 0.0002222;
long receivedDelay2 = 0; //delay between two steps of , received from the computer
float posicao_calculada2 =0; // Armazena posição calculada por pulso
String posicao_calculadaStr2;


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

void checkSerial() //method for receiving the commands
{
    if(Serial.available()){
    data = Serial.readStringUntil('#');
    d1 = data.charAt(0);

    switch(d1){         //select action based on the first character

      case LIGAR_MOTOR:         //first character is an A = turn-on/energize motor
        if(motor == '1'){
          digitalWrite(driverEn1, 1);
          Serial.println("/Motor 1 ligado!");
        }
        else if(motor == '2'){
          digitalWrite(driverEn2, 1);
          Serial.println("/Motor 2 ligado!");
        }
      break;

      case DESLIGAR_MOTOR:         //first character is an a = turn-off/de-energize motor
        if(motor == '1'){
          digitalWrite(driverEn1, 0);
          Serial.println("/Motor 1 desligado!");
        }
        else if(motor == '2'){
          digitalWrite(driverEn2, 0);
          Serial.println("/Motor 2 desligado!");
        }
      break;
      
      case MOVER_MOTOR_CIMA:         //First character is an C = The equipment goes up

        if(motor == '1'){
          Serial.println("c"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
          direcao1 = -1;
        }
        else if(motor == '2'){
          Serial.println("C"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para cima
          direcao2 = -1;
        }
      break;

      case MOVER_MOTOR_BAIXO:        //First character is an B = The equipment goes down

        if(motor == '1'){
          Serial.println("b"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
          direcao1 = 1;
        }
        else if(motor == '2'){
          Serial.println("B"); // Printa a mensagem no aplicativo do vs code:: Direcão: Para baixo
          direcao2 = 1;
        }

      break;

      case QTD_PULSOS:         //First character is an P = set qntd of pulses to send
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

      case ACELERAR_MOTOR:     //First character is an G = motor accelerates
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo com aceleração!
          moverAcelerado1();
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo com aceleração!
          moverAcelerado2();
        }

      
      break;

      case MSG_MOTOR_MOVENDO_COM_ACELERACAO:
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo com aceleração!
          aceleracao1 = 1;
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo com aceleração!
          aceleracao2 = 1;
        }
      break;

      case MSG_MOTOR_MOVENDO_SEM_ACELERACAO:
        if(motor == '1'){
          Serial.println("a"); // Printa a mensagem no aplicativo do vs code:: O motor 1 está se movendo sem aceleração!
          aceleracao1 = 0;
        }
        else if(motor == '2'){
          Serial.println("A"); // Printa a mensagem no aplicativo do vs code: O motor 2 está se movendo sem aceleração!
          aceleracao2 = 0;
        }
      break;


      case ATIVAR_MOTOR_VEL_CTE:     //First character is an H = motor spins with constant speed
        if(motor == '1'){
          Serial.println("/O motor 1 se move com velocidade constante!");
          moverUniforme1();
          
        }
        else if(motor == '2'){
          Serial.println("/O motor 2 se move com velocidade constante!");
          moverUniforme2();

        }
      break;
      
      case DEFINIR_VELOCIDADE:         //First character is an V = set velocity
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

      case PARAR_MOTOR: //para o motor
      
        if(motor == '1'){
          pararMotor1();
        }
        else if(motor == '2'){
          pararMotor2();
        }
      break;
      
      case PARAR_CALIBRACAO: //para calibração
        parar_calibracao = 1;

      break;

      case POSICAO_MOTOR_1: //  Insere um valor para a posição calculada do primeiro motor. 
        delayMicroseconds(1000); 
        x = data.substring(1);
        posicao_calculada1 = x.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr1  = String(posicao_calculada1);
        Serial.println('p'+ posicao_calculadaStr1);
      break;

      case POSICAO_MOTOR_2: //  Insere um valor para a posição calculada do segundo motor. 
        delayMicroseconds(1000); 
        x = data.substring(1);
        posicao_calculada2 = x.toFloat();
        delayMicroseconds(1000); 
        posicao_calculadaStr2  = String(posicao_calculada2);
        Serial.println('P'+ posicao_calculadaStr2);
      break;

      case CONFIGURAR_ZERO_LASER: //sets intercept of the laser

        x = data.substring(1);
        zero_laser = x.toFloat();
      break;
      
      
      case INICIAR_CALIBRACAO: // Inicia o processo de calibração
        calibracao();

      break;

      case INSERIR_CONSTANTES_CALIBRACAO: //Insere constante de calibração dos motores
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



      case ATIVAR_SENSOR_INDUTIVO: // ativa a funcionalidade do sensor indutivo novamente
        motorParou1 = 0;
        Serial.print("\Sensor indutivo ativado"); //Printa a constante de calibração no app do VSCode 
      break;

      case ALTERAR_PARA_MOTOR_2: //Função para mudar qual motor está sendo utilizado.

        motor = '2';
        Serial.println('u'); //Segundo motor sendo operado!
      break;

      case ALTERAR_PARA_MOTOR_1: //Função para mudar qual motor está sendo utilizado.
        motor = '1';
        Serial.println('U');//Primeiro motor sendo operado!
      break;

      case SUBSIDENCIA:
        subsidencia(); // Função que movimenta o motor para frente e para trás (2 voltas completas) ativando o mecanismo de subsidência
      break;

      case ENVIAR_CONFIG_COMPLETA: // recebe todas as informações do motor de uma vez e aciona o motor
        x = data.substring(1);

        //código para separar as strings 
        int firstSeparatorIndex = x.indexOf(';');
        int secondSeparatorIndex = x.indexOf(';', firstSeparatorIndex + 1);
        int thirdSeparatorIndex = x.indexOf(';', secondSeparatorIndex + 1);

        // Extract substrings based on the positions of the separators
        String pulso = x.substring(0, firstSeparatorIndex); // "primeiro numero"
        String velocidade = x.substring(firstSeparatorIndex + 1, secondSeparatorIndex); // "segundo numero"
        String direcao = x.substring(secondSeparatorIndex + 1, thirdSeparatorIndex); // "caracter B ou C"
        String mover = x.substring(thirdSeparatorIndex + 1); // "caracter H ou x"

        
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
          Serial.println("/Velocidade do motor 2: " + x + " Pulsos por segundo");
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
    //Serial.println('l' + posicao_calculada_Sensor_Str); 
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

void calibracao(){ //função para calibrar o atuador automaticamente utilizando o laser
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

void subsidencia() { // Função que movimenta o motor para frente e para trás (2 voltas completas)
    // Configura a velocidade máxima e a aceleração do motor de passo
  stepper1.setMaxSpeed(receivedDelay1);
  stepper1.setAcceleration(3600000);

  // Movimento no sentido positivo
  stepper1.moveTo(-receivedPulsesDistance1);
  stepper1.runToPosition();
  delay(50); 

  // Movimento no sentido negativo
  stepper1.moveTo(receivedPulsesDistance1);
  stepper1.runToPosition();
  delay(50); 
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