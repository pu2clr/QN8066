/*
  Nano 33 IoT board

  This application allows remote control of a transmitter based on the QN8066 through a 
  Python program using a Socket connection. The Arduino sketch for the NANO 33 IoT implements 
  a server that connects to a Wi-Fi network and listens on port 8066. The Python application, 
  in turn, connects to the server or service provided by the NANO 33 IoT and sends configuration
  commands to the QN8066 transmitter.

  Nano 33 IoT Wire up 

  | Device name               | QN8066 Pin           |  Nano 33 IoT      |
  | --------------------------| -------------------- | ----------------- |
  | QN8066                    |                      |                   | 
  |                           | VCC                  |      3.3V         |
  |                           | GND                  |      GND          |    
  |                           | SDIO / SDA (pin 2)   |      A4           |
  |                           | SCLK (pin 1)         |      A5           |
  | --------------------------| ---------------------| ----------------- |


  1. A suggestion if you intend to use PWM to control the RF output power of an amplifier.  

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Oct  2024.
*/
#include <QN8066.h>
#include <WiFiNINA.h>

#define SOCKET_PORT 8066

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 31000
#define RDS_DT_REFRESH_TIME 61000 // Date and Time Service

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

uint8_t currentPower = 0;

// 
char ps[9] = "QN8066 \r";
char rt[33] = "NANO33 IOT FM TX REMOTE CONTROL\r";

// WI-FI Setup
char ssid[] = "PU2CLR";         // Wi-Fi network name
char pass[] = "pu2clr123456";   // Wi-Fi password

int status = WL_IDLE_STATUS;
WiFiServer server(SOCKET_PORT);        // Create a server that listens on port 8066

String receivedData = "";
String field = "";
String value = "";

QN8066 tx;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  delay(1000);

  Serial.println("Start connecting...");
  Serial.flush();

  // Check for Wi-Fi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not detected!");
    while (true);
  }

  // Connect to Wi-Fi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  // Start the server
  server.begin();
  Serial.println("Server started on port 8066");
  printWiFiStatus();

  if (!tx.detectDevice()) {
    Serial.println("\nQN8066 not detected");
    while (true);
  }
  Serial.println("\nQN8066 Detected");
  tx.setup();
  tx.setTX(currentFrequency);   // Sets frequency to 106.9 MHz 
  delay(500);
  startRDS();  

}

// Print the Wi-Fi status (IP address, etc.)
void printWiFiStatus() {
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
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
  /*
  if ((millis() - rdsDateTime) > RDS_DT_REFRESH_TIME) {
    printLocalTime();
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    // Sends RDS local Date and Time
    tx.rdsSendDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min);  
    rdsDateTime = millis();
  }
  */
}

// Sets the Nano 33 IoT Real Time Clock 
void setRTC(String datetime) {
  // TODO
}

// Parse and execute the QN8066 command
String processCommand(String command) {
  int separatorIndex = command.indexOf('=');
  String field = command.substring(0, separatorIndex);  // Field name
  String value = command.substring(separatorIndex + 1); // Field value

  // Process the QN8066 command 
  if (field == "frequency") {
    uint16_t currentFrequency = (uint16_t) ( value.toFloat() * 10.0);
    tx.setTX(currentFrequency);  
    return "Frequency set to: " + String(currentFrequency);
  } else if (field == "rds_pty") {
    tx.rdsSetPTY(value.toInt()); 
    tx.rdsSendPS(ps);
    return "RDS PTY set to: " + String(value);
  } else if (field == "rds_ps") {
    strcpy(ps, value.c_str());
    tx.rdsSendPS(rt);
    return "RDS PS set to: " + value;
  } else if (field == "rds_rt") {
    strcpy(rt, value.c_str());
    tx.rdsSendRT(rt);
    return "RDS RT set to: " + value;
  } else if (field == "stereo_mono") {
    tx.setTxMono(value.toInt());  // 
    return "Stereo/Mono Set to: " + String(value);
  } else if (field == "pre_emphasis") {
    tx.setPreEmphasis(value.toInt());
    return  "Pre-Emphasis set to: " + String(value);
  } else if (field == "impedance") {
    tx.setTxInputImpedance(value.toInt());
    return  "Impedance set to: " + String(value);     
  } else if (field == "buffer_gain") {
    tx.setTxInputBufferGain(value.toInt());
    return  "Impedance set to: " + String(value);   
  } else if (field == "freq_dev") {
    float fd = value.toFloat();
    tx.rdsSetFrequencyDerivation((uint8_t) (fd / 0.69) );
    return  "Frequency Deviation set to: " + String(fd);   
  } else if (field == "soft_clip") {
    tx.setTxInputBufferGain(value.toInt());
    return  "Soft Clip set to: " + String(value);   
  } else if (field == "datetime") { 
    // setRTC(value);
    return  "Local Date and Time set to: " + String(value);
  }    
  return "OK";
}




void loop() {
  // Check for an incoming client
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected");
    // Read incoming data from client
    while (client.connected()) {
      if (client.available()) {
        String command = client.readString();
        Serial.print("\nCommand: ");
        Serial.println(command);
        String result = processCommand(command);
        client.println(result);
      }
      sendRDS();
    }
    client.stop();
    Serial.println("Client disconnected");
  }
  sendRDS();
}
