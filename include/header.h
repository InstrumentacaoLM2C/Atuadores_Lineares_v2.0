#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
#include <String.h>
#include <AccelStepper.h>

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

#endif /*HEADER_H */