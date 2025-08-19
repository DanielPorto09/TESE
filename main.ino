#include "web.h"  

/*Pinos*/
#define pinSensor 14  // Sensor no D5
#define pinBuzzer 4  // Sensor no D2
#define pinLed 13     // LED no D7
#define pinLed_V 

// Arrays globais de configuração
byte pinos_io[QNT_PINOS] = {pinLed, pinSensor, pinBuzzer};
byte setup_pinos[QNT_PINOS] = {OUTPUT, INPUT, OUTPUT};

void setup() {
  conexao_wifi(); 
  for (int nP = 0; nP < QNT_PINOS; nP++) {
    pinMode(pinos_io[nP], setup_pinos[nP]);
  }
  digitalWrite(pinLed, LOW); 

}

void loop() {
  carrega_pag();
  }
  
