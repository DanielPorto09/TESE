#include "sensor.h"
#include <Arduino.h>

unsigned long tempo_inicial = 0;
bool delay_ativo = false;

bool libera_sensor(int FreeSensor) {
  int estadoSensor = digitalRead(FreeSensor); 

  if (estadoSensor == HIGH) {
    return true;
  } else {
    // Caso contrário, retorna falso
    return false;
  }
}

void controla_led(int pinBuzzer, int FreeSensor, int pinSensor, int pinLed) {
  int sensorValue = digitalRead(pinSensor);  // Lê o valor do sensor

  if (sensorValue == HIGH && libera_sensor(FreeSensor)) {
    blinka_led(pinLed);
  } else if (sensorValue == HIGH && !libera_sensor(FreeSensor)) {
    digitalWrite(pinBuzzer, HIGH); 
  } else {
    digitalWrite(pinBuzzer, LOW);  
  }
}

void blinka_led(int pinLed) {
  static int estado = 0;
  static unsigned long tempo_inicial = 0;
  const unsigned long intervalo = 500; // 500 ms entre os estados

  unsigned long agora = millis();

  if (estado == 0) {
    digitalWrite(pinLed, HIGH);
    tempo_inicial = agora;
    estado = 1;
  } 
  else if (estado == 1) {
    if (agora - tempo_inicial >= intervalo) {
      digitalWrite(pinLed, LOW);
      tempo_inicial = agora;
      estado = 2;
    }
  } 
  else if (estado == 2) {
    if (agora - tempo_inicial >= intervalo) {
      digitalWrite(pinLed, HIGH);
      tempo_inicial = agora;
      estado = 3;
    }
  } 
  else if (estado == 3) {
    if (agora - tempo_inicial >= intervalo) {
      digitalWrite(pinLed, LOW);
      estado = 0;
    }
  }
}





