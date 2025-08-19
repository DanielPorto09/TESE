#include "web.h"  
#include "sensor.h"
/*Pinos*/
#define pinSensor 14  // Sensor no D5
#define FreeSensor 16  // led_vermelho/bool no D0
#define pinBuzzer 4  // Buzzer no D2
#define pinLed 13     // LED no D7

// Arrays globais de configuração
byte pinos_io[QNT_PINOS] = {pinLed, pinSensor, pinBuzzer,FreeSensor};
byte setup_pinos[QNT_PINOS] = {OUTPUT, INPUT, OUTPUT,OUTPUT};

void setup() {
  conexao_wifi(); 
  for (int nP = 0; nP < QNT_PINOS; nP++) {
    pinMode(pinos_io[nP], setup_pinos[nP]);
  }
  digitalWrite(pinLed, LOW);
   
}

void loop() {

  carrega_pag();
  controla_led(pinBuzzer, FreeSensor, pinSensor, pinLed);
  
  }
  
