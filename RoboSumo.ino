#include <Servo.h>
#include "Ultrasonic.h"

  /* ------------------ */
 /* Variáveis Globais  */
/* ------------------ */


/*
 *  Ultrasom
 */

Ultrasonic ultrasonic(6,7); //triger/echo

long microsec = 0;
float distanciaCM = 0;
int ULTRA_DIST = 30;


/*
 *  Servo
 */

Servo rodaesq;
Servo rodadir;

int vesq = 90;
int vdir = 90;


/*
 *  Sensor LDR
 */

//Valor lido do Sensor LDR a cada loop
int valorSensor;

//Valor do piso preto obtivo através de calibração automática
float valor_medio_ldr = 0;

//Precisão da Calibração (Quantas vezes ele ler sensor antes de tirar a média)
int CALIB_PRECISION = 6;

//Margem de erro para calcular se valor é borda ou é arena
int CALIB_ERROR_MARGIN = 15;

//Ativa calibração automática
bool calib_auto = true;

//Ativa delay inicial automatico
bool delay_5sec = true;

//Contador da calibração automática
int calib_contador = 0;


/*
 *  Sirene
 */

//Pino dos leds
int leds = 10;

//Estado do pino dos LEDs
int ledState = LOW;

// will store last time LED was updated
long previousMillis = 0;




  /* ------------------ */
 /*    Funções Úteis   */
/* ------------------ */


/* Função para ler Ultrasom */
int ultrassom(){

  //Lendo o sensor
  microsec = ultrasonic.timing();

  //Convertendo a distância em CM
  distanciaCM = ultrasonic.convert(microsec, Ultrasonic::CM);

  delay(50);
  return (distanciaCM);
}


/* Função para acender os leds da Sirene */
void sirene(int interval){

    //Pega o tempo atual em milisegundos
    unsigned long currentMillis = millis();

    //Se já passou tempo definido no intervalo
    if(currentMillis - previousMillis > interval) {

            // save the last time you blinked the LED
            previousMillis = currentMillis;

            // if the LED is off turn it on and vice-versa:
            if (ledState == LOW)
                ledState = HIGH;
            else
                ledState = LOW;

            // set the LED with the ledState of the variable:
            digitalWrite(leds, ledState);
        }
}


/* Função para calibração automática do sensor LDR */
void calibracao_automatica() {

    //Acende os Leds da Sirene
    digitalWrite(leds, HIGH);

    //Ler Sensor
    valorSensor = analogRead(0);
    Serial.print("\nDinamic: ");
    Serial.print(valorSensor);

    //Somar valor lido numa variável
    valor_medio_ldr += (float) valorSensor;

    //Aumenta o contador
    calib_contador = calib_contador + 1;

    //Repete a calibração até que se atinja a precisão esperada
    if(calib_contador >= CALIB_PRECISION) {

        //Desativa o modo de calibração
        calib_auto = false;

        //Tira a média dos valores lidos e armazena na variável final
        valor_medio_ldr = (valor_medio_ldr / CALIB_PRECISION) + CALIB_ERROR_MARGIN;

        Serial.print("\nDinamic Valor Medio: ");
        Serial.print(valor_medio_ldr);

    }
}


/* Função para aguardar o tempo de espera inicial conforme as regras da competição */
void delay_especial(){
    if(delay_5sec) {
           delay(3000);
           delay_5sec = false;
           digitalWrite(leds, LOW);
       }
}


/* Função para ler sensor e detectar se está na borda */
void detectar_borda(){

    //Ler sensor LDR
    valorSensor = analogRead(0);

    //Se detectar borda, entra no while
    while(valorSensor > valor_medio_ldr){

            vesq = 359;
            vdir = 1;
            valorSensor = analogRead(0);
            Serial.println(valorSensor);
            rodaesq.write(vesq);
            rodadir.write(vdir);

        }
}


/* Função para ler ultrassom e detectar o inimigo */
void detectar_inimigo() {

    //Se não detectou inimigo
    if((ultrassom()) > ULTRA_DIST){

        //Desliga sirene
        digitalWrite(leds, LOW);

        //Seta modo de giro do servo
        vdir = 1;
        vesq = 1;

    }

    //Se detectou inimigo
    else{

        //Liga sirene
        sirene(100);

        //Seta modo de andar para frente do servo
        vesq = 1;
        vdir = 359;
    }

}



  /* ------------------ */
 /*   Funções Padrão   */
/* ------------------ */


void setup() {

    rodaesq.attach(9);
    rodadir.attach(5);

    pinMode(leds, OUTPUT);
    Serial.begin(9600);

}


void loop() {

    if(calib_auto)
        calibracao_automatica();

    //Algoritmo de controlamento do robo
    else {

        //Delay único e inicial de acionamento do robo
        delay_especial();

        //Aciona o Servo para mover as rodas
        rodaesq.write(vesq);
        rodadir.write(vdir);

        //Ver se está na borda
        detectar_borda();

        //Ver se detectou inimigo
        detectar_inimigo();

    }

}
