#ifndef WEB_H
#define WEB_H

#include <ESP8266WiFi.h>

// Quantidade de pinos usados
#define QNT_PINOS 4

// Globais
extern byte pinos_io[QNT_PINOS];
extern byte setup_pinos[QNT_PINOS];
extern bool sensor_liberado;

/*inversão por causa do ESP*/
/*
#define low low 
#define high high
*/

// padronizar nomeclatura
#define pinosDigitais pinos_io
#define modoPinos setup_pinos


// Variáveis de conexão
extern const char* ssid;
extern const char* password;
extern WiFiServer server;

extern String HTTP_req; 
extern String URLValue;

// Funções principais
void conexao_wifi();
void carrega_pag();

// Utilitários auxiliares
void processaPorta(byte porta, byte posicao, WiFiClient cl);
void lePortaDigital(byte porta, byte posicao, WiFiClient cl);        
String getURLRequest(String *requisicao);
bool mainPageRequest(String *requisicao);

#endif
