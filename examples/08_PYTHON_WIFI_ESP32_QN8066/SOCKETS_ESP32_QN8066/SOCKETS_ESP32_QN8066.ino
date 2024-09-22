/*
  ESP32 Dev Modeule version.

  Wire up ESP32 

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  | QN8066                    |                           |               | 
  |                           | VCC                       |  3.3V         |
  |                           | SDIO / SDA (pin 8)        |  GPIO21       |
  |                           | SCLK (pin 7)              |  GPIO22       |
  | --------------------------| --------------------------| --------------|
  | PWM                       |                           |               |
  |                           |                           |  GPIO12       | 

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  ESP32 Internal RTC: https://github.com/fbiego/ESP32Time

  By PU2CLR, Ricardo,  Feb  2024.
*/

#include <WiFi.h>
#include <QN8066.h>  // Inclua sua biblioteca QN8066 aqui

// I2C bus pin on ESP32
#define ESP32_I2C_SDA 21     // GPIO21
#define ESP32_I2C_SCL 22     // GPIO22 

#define SOCKET_PORT 8066

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service


long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

uint8_t currentPower = 0;

char ps[10] = "       \r";
char rt[34] = "                               \r";



// Wi-Fi setup
const char* ssid = "Your WIFI SSID";
const char* password = "Your password";


WiFiServer server(SOCKET_PORT);  // Socket Server using port 8066

QN8066 tx;

void setup() {
  Serial.begin(115200);

  // The line below may be necessary to setup I2C pins on ESP32
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  Serial.println("PORT: ");
  Serial.println(SOCKET_PORT);

  // Iniciar o servidor socket
  server.begin();
  Serial.println("Socket server started, waiting for clients...");

  tx.setup();
  tx.setTX(currentFrequency);   // Sets frequency to 106.9 MHz 
  delay(500);
  startRDS();  

  Serial.println("Trasmitting...");

}

void startRDS() {
    tx.rdsTxEnable(true);
    delay(200);
    tx.rdsInitTx(0x8,0x1,0x9B, 0, 25, 6);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
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


// Função que processa o comando recebido e executa a função correspondente
String processCommand(String command) {

  int nLen;

  // Exemplo de comando recebido: "frequency=101.5"
  int separatorIndex = command.indexOf('=');
  String field = command.substring(0, separatorIndex);  // Nome do campo
  String value = command.substring(separatorIndex + 1); // Valor do campo

  // Processa o campo específico e executa a função correspondente
  if (field == "frequency") {
    uint16_t currentFrequency = (uint16_t) ( value.toFloat() * 10.0);
    tx.setTX(currentFrequency);  
    return "Frequency set to: " + String(currentFrequency);
  } else if (field == "rds_pty") {
    String rds_pty = value.substring(0,2);
    tx.rdsSetPTY(rds_pty.toInt()); 
    return "RDS PTY set to: " + String(rds_pty);
  } else if (field == "rds_ps") {
    nLen = value.length();
    strncpy(ps, value.c_str(), nLen);
    ps[nLen] = '\r';
    ps[nLen+1] = '\0';
    return "RDS PS set to: " + value;
  } else if (field == "rds_rt") {
    nLen = value.length();
    strncpy(rt, value.c_str(), nLen);
    ps[nLen] = '\r';
    ps[nLen+1] = '\0';
    return "RDS RT set to: " + value;
  } else if (field == "stereo_mono") {
    int stereoMono = value.toInt();
    tx.setTxMono(stereoMono);  // Chama a função correspondente no QN8066
    return "Stereo/Mono Set to: " + String(value);
  }
  return "OK";
}


void loop() {
  // Verifica se há um cliente conectado
  // WiFiClient client = server.available();
  WiFiClient client = server.accept();
  if (client) {
    Serial.println("Client connected!");

    // Enquanto o cliente estiver conectado
    while (client.connected()) {
      if (client.available()) {
        // Lê os dados enviados pelo cliente (GUI Python)
        String command = client.readStringUntil('\n');
        Serial.println("Command received: " + command);

        String result = processCommand(command);
        client.println( result );
      }
      sendRDS();
    }

    // Fecha a conexão com o cliente
    client.stop();
    Serial.println("Client disconnected.");
  }

  sendRDS();

}
