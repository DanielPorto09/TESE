#ifndef SENSOR_H
#define SENSOR_H

bool delay_fake(int espera);
bool libera_sensor(int FreeSensor);
void controla_led(int pinBuzzer, int FreeSensor, int pinSensor, int pinLed);
void blinka_led(int pinLed);

#endif
