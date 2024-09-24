/*
  ESP32 Dev Modeule version.

Key Features and Notes of This Sketch:

1. This sketch uses sockets to receive commands from external applications to configure the 
   FM transmitter (see the Python example `esp32_qn8066.py`).
2. It also utilizes the internal Real Time Clock (RTC) of the ESP32 to send RDS messages with
   this information.
3. Sets the Date and Time based on your network/Internet information or via the client 
   application (see the Python example `esp32_qn8066.py`).
4. You need to modify the sketch to set the local time for your location.
5. To connect to your Wi-Fi network, you must provide the SSID and password of your router. 
   Otherwise, this application will not function.
6. This sketch was compiled and tested on the ESP32 Der Module. If you are using a different ESP32 model, 
   consider reviewing the pin connections.
7. In the Arduino IDE, during application execution, check the Serial Monitor for the IP address 
   assigned to the ESP32 by your router's DHCP service. This IP must be used in the Python application 
   to connect to the ESP32.
8. RDS message updates such as PTY, PS, RT, and Time are not executed immediately. This may depend on 
   the receiver's update timing as well as the distribution of each message's timing programmed in this sketch.   

This translation enhances clarity and structure for better readability. Let me know if you'd like further refinements!

  Wire up ESP32 

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  | QN8066                    |                           |               | 
  |                           | VCC                       |  3.3V         |
  |                           | GND                       |  GND          |  
  |                           | SDIO / SDA (pin 8)        |  GPIO21       |
  |                           | SCLK (pin 7)              |  GPIO22       |
  | --------------------------| --------------------------| --------------|
  | PWM                       |                           |               |
  |                           |                           |  GPIO12       | 

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Feb  2024.
*/

#include <WiFi.h>
#include <QN8066.h>  // Inclua sua biblioteca QN8066 aqui
#include <time.h>    // Using internal RTC of ESP32 

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

char ps[9] = "NO PS  \r";
char rt[33] = "NO RT                          \r";

// Wi-Fi setup
const char* ssid =   "PU2CLR";          // Change to your WIFI SSID
const char* password = "pu2clr123456";  // Change to your password



// Local Time setup
const long gmtOffset_sec = -10800;  // Brasilia local date and time offset (-3h)
const int daylightOffset_sec = 0;   // There is some confusion about this here in Brazil. As of now, there is no daylight saving time in effect in Brazil.

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

  // Sets the current RTC based on Network if it is available  
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");

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
    printLocalTime();
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    // Sends RDS local Date and Time
    tx.rdsSendDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min);  
    rdsDateTime = millis();
  }
}


String processCommand(String command) {
  int nLen;
  int separatorIndex = command.indexOf('=');
  String field = command.substring(0, separatorIndex);  // Field name
  String value = command.substring(separatorIndex + 1); // Field value

  // Process the QN8066 command 
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
    tx.setTxMono(stereoMono);  // 
    return "Stereo/Mono Set to: " + String(value);
  } else if (field == "pre_emphasis") {
    String pe = value.substring(0,1);
    tx.setPreEmphasis(pe.toInt());
    return  "Pre-Emphasis set to: " + String(pe);
  } else if (field == "impedance") {
    String imp = value.substring(0,1);
    tx.setTxInputImpedance(imp.toInt());
    return  "Impedance set to: " + String(imp);     
  } else if (field == "buffer_gain") {
    String gain = value.substring(0,1);
    tx.setTxInputBufferGain(gain.toInt());
    return  "Impedance set to: " + String(gain);   
  } else if (field == "freq_dev") {
    float fd = value.toFloat();
    tx.rdsSetFrequencyDerivation((uint8_t) (fd / 0.69) );
    return  "Frequency Deviation set to: " + String(fd);   
  } else if (field == "soft_clip") {
    String sc = value.substring(0,1);
    tx.setTxInputBufferGain(sc.toInt());
    return  "Soft Clip set to: " + String(sc);   
  } else if (field == "datetime") { 
    setRTC(value);
    return  "Local Date and Time set to: " + String(value);
  }    
  return "OK";
}

void setRTC(String datetime) {
  struct tm tm;
  tm.tm_year = datetime.substring(0, 4).toInt() - 1900; // Year 
  tm.tm_mon = datetime.substring(5, 7).toInt() - 1;     // Month [0, 11]
  tm.tm_mday = datetime.substring(8, 10).toInt();       // Day [1, 31]
  tm.tm_hour = datetime.substring(11, 13).toInt();      // Hour [0, 23]
  tm.tm_min = datetime.substring(14, 16).toInt();       // Minute [0, 59]
  tm.tm_sec = 0;            

  time_t t = mktime(&tm);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL); // Defines ESP32 RTC time
  Serial.println("Internal ESP32 RTC updated!");
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Local Time fails");
    return;
  }
  Serial.println(&timeinfo, "%Y/%m/%d %H:%M:%S");
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    Serial.println("Client connected!");

    while (client.connected()) {
      if (client.available()) {
        String command = client.readStringUntil('\n');
        Serial.println("Command received: " + command);

        String result = processCommand(command);
        client.println( result );
      }
      sendRDS();
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
  sendRDS();
}
