#include "web.h"

// Definição das variáveis globais
const char* ssid = "daniel";
const char* password = "amoasophia";
bool sensor_liberado = false;

WiFiServer server(80);

String HTTP_req; 
String URLValue;

// Arrays de pinos (definidos em main.ino)
extern byte pinos_io[QNT_PINOS];
extern byte setup_pinos[QNT_PINOS];

void conexao_wifi() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");
  server.begin();
  Serial.println("Server iniciado");
  Serial.println(WiFi.localIP());
}

void carrega_pag() {
  WiFiClient client = server.available();     

  if (client) { 
      boolean currentLineIsBlank = true;
      while (client.connected()) {
          if (client.available()) { 
              char c = client.read(); 
              HTTP_req += c;  
              
              if (c == '\n' && currentLineIsBlank ) { 

                  if ( mainPageRequest(&HTTP_req) ) {
                      URLValue = getURLRequest(&HTTP_req);
                                                
                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-Type: text/html");
                      client.println("Connection: keep-alive");              //<------ ATENCAO
                      client.println();
                      
                      //Conteudo da Página HTML
                      client.println("<!DOCTYPE html>");
                      client.println("<html>");

                      
                      client.println("<head>");
                      client.println("<title>Arduino via WEB</title>");

                      client.println("<script>");
                      client.println("function LeDadosDoArduino() {");
                      client.println("nocache = \"&nocache=\" + Math.random() * 1000000;");
                      client.println("var request = new XMLHttpRequest();");
                      client.println("var posIni;");
                      client.println("var valPosIni;");
                      client.println("var valPosFim;");
                      client.println("request.onreadystatechange = function() {");
                      client.println("if (this.readyState == 4) {");
                      client.println("if (this.status == 200) {");
                      client.println("if (this.responseText != null) {");

                      for (int nL=0; nL < QNT_PINOS; nL++) {                                                    //<-------NOVO
                          client.print("posIni = this.responseText.indexOf(\"PD");
                          client.print(pinos_io[nL]);
                          client.println("\");");
                          client.println("if ( posIni > -1) {");
                          client.println("valPosIni = this.responseText.indexOf(\"#\", posIni) + 1;");
                          client.println("valPosFim = this.responseText.indexOf(\"|\", posIni);");
                          client.print("document.getElementById(\"pino");
                          client.print(pinos_io[nL]);
                          client.println("\").checked = Number(this.responseText.substring(valPosIni, valPosFim));");
                          client.println("}");
                      }
            
                      client.println("}}}}");
                      client.println("request.open(\"GET\", \"solicitacao_via_ajax\" + nocache, true);");
                      client.println("request.send(null);");
                      client.println("setTimeout('LeDadosDoArduino()', 1000);");
                      client.println("}");
                      client.println("</script>");
                      
                      client.println("</head>");

                      client.println("<body onload=\"LeDadosDoArduino()\">");                      //<------ALTERADO                    
            
                      client.println("<br/>");                        
                      client.println("<h1>PORTAS EM FUN&Ccedil;&Atilde;O DIGITAL</h1>");
                      client.println("<form method=\"get\">");

                      for (int nL=0; nL < QNT_PINOS; nL++) {
                          processaPorta(pinos_io[nL], nL, client);
                          client.println("<br/>");
                      }
                      
                      client.println("<br/>");
                      client.println("<button type=\"submit\">Envia para o ESP8266</button>");
                      client.println("</form>");                      
                      
                      client.println("</body>");
                      
                      client.println("</html>");

                  
                  } else if (HTTP_req.indexOf("solicitacao_via_ajax") > -1) {    

                      //Serial.println("Requisição AJAX recebida!"); //mensagem para teste de STR

                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-Type: text/html");
                      client.println("Connection: keep-alive");      
                      client.println();                      
                
                      for (int nL=0; nL < QNT_PINOS; nL++) {
                          lePortaDigital(pinos_io[nL], nL, client);
                      }
                          
                  } else {

                      client.println("HTTP/1.1 200 OK");
                  }
                  HTTP_req = "";    
                  break;
              }
              
              if (c == '\n') {
                  currentLineIsBlank = true;
              } 
              else if (c != '\r') {
                  currentLineIsBlank = false;
              }
          }
      } 
      delay(1);     
      client.stop(); 
  } 
}

void processaPorta(byte porta, byte posicao, WiFiClient cl)
{
static boolean LED_status = 0;
String cHTML;

    cHTML = "P";
    cHTML += porta;
    cHTML += "=";
    cHTML += porta;

    if (modoPinos[posicao] == OUTPUT) { 
        
        if (URLValue.indexOf(cHTML) > -1) { 
           LED_status = HIGH;
        } else {
           LED_status = LOW;
        }
        digitalWrite(porta, LED_status);
    } else {

        LED_status = digitalRead(porta);
    }
        
    cl.print("<input type=\"checkbox\" name=\"P");
    cl.print(porta);
    cl.print("\" value=\"");
    cl.print(porta);
    
    cl.print("\"");

    cl.print(" id=\"pino");           //<------NOVO
    cl.print(porta);
    cl.print("\"");
    
    if (LED_status) { 
        cl.print(" checked ");
    }

    if (modoPinos[posicao] != OUTPUT) { 
        cl.print(" disabled ");
    }
    
    cl.print(">Porta ");
    cl.print(porta);

    cl.println();
}

void lePortaDigital(byte porta, byte posicao, WiFiClient cl)
{
    if (modoPinos[posicao] != OUTPUT) {
       cl.print("PD");
       cl.print(porta);
       cl.print("#");
       
       if (digitalRead(porta)) {
          cl.print("1");
       } else {
          cl.print("0");
       }
       cl.println("|");
    }
}

String getURLRequest(String *requisicao) {
int inicio, fim;
String retorno;

  inicio = requisicao->indexOf("GET") + 3;
  fim = requisicao->indexOf("HTTP/") - 1;
  retorno = requisicao->substring(inicio, fim);
  retorno.trim();

  return retorno;
}

bool mainPageRequest(String *requisicao) {
String valor;
bool retorno = false;

  valor = getURLRequest(requisicao);
  valor.toLowerCase();

  if (valor == "/") {
     retorno = true;
  }

  if (valor.substring(0,2) == "/?") {
     retorno = true;
  }  

  if (valor.substring(0,10) == "/index.htm") {
     retorno = true;
  }  

  return retorno;
}
