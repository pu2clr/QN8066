#include <WiFi.h>
#include <WebServer.h>
#include <QN8066.h>


#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service


long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

char ps[10] = "       \r";
char rt[34] = "                               \r";


// Configurações da rede Wi-Fi
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWD";

// Servidor Web na porta 80
WebServer server(80);

QN8066 tx;

void handleRoot() {
  String htmlPage = "<html><body>";
  htmlPage += "<h1>QN8066 Transmitter Settings</h1>";
  htmlPage += "<form action=\"/setParameters\" method=\"POST\">";
  
  // Tabela para organizar os campos em colunas alinhadas
  htmlPage += "<table>";
  
  // Linha da Frequência de Transmissão
  htmlPage += "<tr><td>Frequency(MHz):</td>";
  htmlPage += "<td><input type=\"text\" name=\"frequency\" maxlength=\"6\" size=\"6\"></td></tr>";
  
  // Linha da Potência
  htmlPage += "<tr><td>Power (%):</td>";
  htmlPage += "<td><input type=\"text\" name=\"power\" maxlength=\"3\" size=\"3\"></td></tr>";
  
  // Linha do RDS PTY
  htmlPage += "<tr><td>RDS PTY:</td>";
  htmlPage += "<td><select name=\"rds_pty\">";
  htmlPage += "<option value=\"0\">No program</option>";
  htmlPage += "<option value=\"1\">News</option>";
  htmlPage += "<option value=\"3\">Information</option>";
  htmlPage += "<option value=\"4\">Sport</option>";
  htmlPage += "<option value=\"5\">Education/Rock</option>";
  htmlPage += "<option value=\"7\">Culture</option>";
  htmlPage += "<option value=\"8\">Science</option>";
  htmlPage += "<option value=\"10\">Pop Music</option>";
  htmlPage += "<option value=\"16\">Weather</option>";
  htmlPage += "<option value=\"20\">Religion</option>";
  htmlPage += "<option value=\"29\">Documentaty</option>";
  htmlPage += "<option value=\"30\">Alarm</option>";
  htmlPage += "</select></td></tr>";

  // Linha do RDS PS
  htmlPage += "<tr><td>RDS PS:</td>";
  htmlPage += "<td><input type=\"text\" name=\"rds_ps\" maxlength=\"8\" size=\"8\"></td></tr>";
  
  // Linha do RDS RT
  htmlPage += "<tr><td>RDS RT:</td>";
  htmlPage += "<td><input type=\"text\" name=\"rds_rt\" maxlength=\"32\" size=\"32\"></td></tr>";
 
  
  // Finalizando a tabela e o formulário
  htmlPage += "</table><br>";
  htmlPage += "<input type=\"submit\" value=\"Set Transmitter\">";
  htmlPage += "</form></body></html>";
  
  server.send(200, "text/html", htmlPage);
}

void habdleConfig() {
  String htmlPage = "<html><body>";
  htmlPage += "<h1>QN8066 Transmitter Config</h1>";
  htmlPage += "<form action=\"/setParameters\" method=\"POST\">";
  
  // Tabela para organizar os campos em colunas alinhadas
  htmlPage += "<table>";
  
  htmlPage += "<tr><td>PAC:</td>";
  htmlPage += "<td><input type=\"text\" name=\"pac\" maxlength=\"6\" size=\"6\"></td></tr>";
  

  htmlPage += "<tr><td>Stereo:</td>";
  htmlPage += "<td><input type=\"text\" name=\"stereo\" maxlength=\"3\" size=\"3\"></td></tr>";

  htmlPage += "</table><br>";
  htmlPage += "<input type=\"submit\" value=\"Set Transmitter\">";
  htmlPage += "</form></body></html>";
  
  server.send(200, "text/html", htmlPage);
}


// Função para tratar o envio do formulário
void handleFormSubmit() {
  String frequency = server.arg("frequency");
  String power = server.arg("power");
  String rds_pty = server.arg("rds_pty");
  String rds_ps = server.arg("rds_ps");
  String rds_rt = server.arg("rds_rt");

  // Aqui você pode adicionar o código para enviar essas configurações para o QN8066
  // Como por exemplo, configurar a frequência de transmissão, potência, e os parâmetros RDS.

  String response = "<html><body>";
  response += "<h1>Settings Received</h1>";
  response += "<p>Frequency: " + frequency + " MHz</p>";
  response += "<p>Power%: " + power + "</p>";
  response += "<p>RDS PTY: " + rds_pty + "</p>";
  response += "<p>RDS PS: " + rds_ps + "</p>";
  response += "<p>RDS RT: " + rds_rt + "</p>";
  response += "</body></html>";
  

  currentFrequency = (uint16_t) (frequency.toFloat() * 10);
  if (currentFrequency != previousFrequency) {
    tx.setTX(currentFrequency);  
    previousFrequency = currentFrequency;
  }


  int nLen;

  nLen = rds_ps.length();
  strncpy(ps, rds_ps.c_str(), nLen);
  ps[nLen] = '\r';
  ps[nLen+1] = '\0';

  nLen = rds_rt.length();
  strncpy(rt, rds_rt.c_str(), nLen);
  rt[nLen] = '\r';
  rt[nLen+1] = '\0';

  tx.rdsSetPTY(rds_pty.toInt());
 

  server.send(200, "text/html", response);
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Conecta na rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando na rede Wi-Fi...");
  }
  Serial.println("Wi-Fi conectado!");



  // Inicia o servidor HTTP
  server.on("/", handleRoot);
  server.on("/setParameters", HTTP_POST, handleFormSubmit);
  server.on("/config", habdleConfig);
  server.begin();
  Serial.println("\nServidor HTTP iniciado.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  


  tx.setup();
  tx.setTX(currentFrequency);   // Sets frequency to 106.9 MHz 
  delay(500);
  startRDS();  

  Serial.println("Trasmitting...");

}

void startRDS() {
    tx.rdsTxEnable(true);
    delay(200);
    tx.rdsInitTx(0x8,0x1,0x9B, 5, 25, 6);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
}

void sendRDS() {

  // PS refreshing control
  if ((millis() - rdsTimePS) > RDS_PS_REFRESH_TIME) {
    tx.rdsSendPS(ps);
    rdsTimePS = millis();
  }

  // RT refreshing control
  if ((millis() - rdsTimeRT) > RDS_RT_REFRESH_TIME) {
    tx.rdsSendRT(rt);     // See rdsSendRTMessage in https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html
    rdsTimeRT = millis();
  }


  // Date Time Service refreshing control
  if ((millis() - rdsDateTime) > RDS_DT_REFRESH_TIME) {
    delay(100);
    // To use the function (service) below, you will need to add an integrated clock to your 
    // system that provides the date and time to the system. The following example presents 
    // only a fixed date and time and is intended solely to illustrate the use of the function.
    tx.rdsSendDateTime(2024, 8, 30, 13, 01, 0);  // Sends Year = 2024; month = 8; day = 29; At 12:45 (local time)    
    rdsDateTime = millis();
  }
}

void loop() {
  sendRDS();
  server.handleClient();
}
