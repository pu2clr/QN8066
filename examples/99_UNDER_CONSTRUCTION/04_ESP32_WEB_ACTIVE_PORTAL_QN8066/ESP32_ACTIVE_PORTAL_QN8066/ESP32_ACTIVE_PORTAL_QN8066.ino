/*
  ESP32 Dev Modeule version.

1. This application is very similar to the '07_WEB_WIFI_ESP32_QN8066' application, with one subtle difference: 
   the ESP32 itself will provide its own internet network (Access Point), allowing you to configure the FM 
   Transmitter by connecting your device (phone or computer) directly to the ESP32. This eliminates the need 
   for another Wi-Fi network configured in your home or any similar setup.
2. This sketch uses HTML form to get and process commands from external browser applications to configure the 
   FM transmitter (see the Python example `esp32_qn8066.py`).
3. It also utilizes the internal Real Time Clock (RTC) of the ESP32 to send RDS messages with
   this information.
4. Sets the Date and Time based on your network/Internet information. 
5. You need to modify the sketch to set the local time for your location.
6. To connect to your Wi-Fi network, you must provide the SSID and password of your router. 
   Otherwise, this application will not function.
7. This sketch was compiled and tested on the ESP32 Der Module. If you are using a different ESP32 model, 
   consider reviewing the pin connections.
8. In the Arduino IDE, during application execution, check the Serial Monitor for the IP address 
   assigned to the ESP32 by your router's DHCP service. 
9. RDS message updates such as PTY, PS, RT, and Time are not executed immediately. This may depend on 
   the receiver's update timing as well as the distribution of each message's timing programmed in this sketch.   



  ESP32 Dev Module Wire up 

  | Device name               | QN8066 Pin           |  ESP32 Dev Module |
  | --------------------------| -------------------- | ----------------- |
  | QN8066                    |                      |                   | 
  |                           | VCC                  |      3.3V         |
  |                           | GND                  |      GND          |    
  |                           | SDIO / SDA (pin 2)   |      GPIO04 [1]   |
  |                           | SCLK (pin 1)         |      GPIO05 [1]   |
  | --------------------------| ---------------------| ----------------- |
  | PWM Power Controller [2]  |                      |                   |
  |                           |                      |      GPIO06       | 

  1. It can change if you are not using the ESP32 Dev Module. Check you ESP32 board pinout 
  2. A suggestion if you intend to use PWM to control the RF output power of an amplifier.  


  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  ESP32 Internal RTC: https://github.com/fbiego/ESP32Time

  By PU2CLR, Ricardo,  Feb  2024.
*/


#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <time.h>    // Using internal RTC of ESP32 
#include <driver/ledc.h>
#include <QN8066.h>

#define ESP32_I2C_SDA 4     // GPIO4
#define ESP32_I2C_SCL 5     // GPIO5 

#define ACTIVE_CLOCK32K  8     // GPIO08  


#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service


const int ledcChannel = 0;   // PWM channel (0 to 15)
const int clock_frequency = 32768; // Frequency in Hz
const int resolution = 8;    // Resolution in bits (can be adjusted as needed)

long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

uint8_t currentPower = 0;

char ps[9] = "PU2CLR \r";
char rt[33] = "QN8066 WEB Server control      \r";


// Wi-Fi setup
const char *ssid = "ESP32_QN8066";
const char *password = "esp32qn8066";


IPAddress local_IP(10, 0, 0, 1);        // Endereço IP fixo
IPAddress gateway(10, 0, 0, 1);         // Gateway (o próprio ESP32)
IPAddress subnet(255, 255, 255, 0);     // Máscara de sub-rede


// DNS 
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Local Time setup
const long gmtOffset_sec = -10800;  // Brasilia local date and time offset (-3h)
const int daylightOffset_sec = 0;   // There is some confusion about this here in Brazil. As of now, there is no daylight saving time in effect in Brazil.


// Web server
WebServer server(80);

QN8066 tx;

void handleRoot() {
  String htmlPage = "<html><head>";
 
  // Adicionando estilo CSS para centralizar o formulário e ajustar a tabela
  
  htmlPage += "<style>";
  htmlPage += "body { font-family: Arial, sans-serif; background-color: #006400; color: yellow;"; // Fundo verde Amazonas (#006400) e texto amarelo
  htmlPage += " display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  htmlPage += "h1 { text-align: center; margin-bottom: 20px; color: yellow; }"; // Título com fonte amarela
  htmlPage += "table { border-collapse: collapse; width: auto; margin-top: 20px; }";
  htmlPage += "td, th { border: 1px solid black; padding: 8px; color: yellow; }"; // Texto da tabela amarelo
  htmlPage += "td input[type='text'], td select { width: 32ch; color: yellow; background-color: #004d00; border: 1px solid yellow; }"; // Campos com fundo verde escuro e borda amarela
  htmlPage += "td:nth-child(1) { text-align: right; }"; // Alinha os rótulos à direita
  htmlPage += "</style>";

  htmlPage += "</head><body>";

  htmlPage += "<h1>PU2CLR QN8066 Arduino Library</h1>";
  htmlPage += "<h1>FM Transmitter Configuration with ESP32 Active Portal</h1>";


  // Formulário com Ajax e solução tradicional
  htmlPage += "<form method='POST' action='/setParameters'>";
  
  // Tabela para alinhar campos e botões
  htmlPage += "<table>";
  
  // Frequency
  htmlPage += "<tr>";
  htmlPage += "<td>Transmission Frequency (MHz):</td>";
  htmlPage += "<td><input type='text' id='frequency' name='frequency' maxlength='6' size='6' value='" + String((float) currentFrequency / 10.0) + "'></td>"; 
  htmlPage += "<td><button type='button' onclick='sendData(\"frequency\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // Power (Not implemented so far)
  htmlPage += "<tr>";
  htmlPage += "<td>Power:</td>";
  htmlPage += "<td><input type='text' id='power' name='power' maxlength='3' size='3' value='" + String(currentPower) + "'></td>"; 
  htmlPage += "<td><button type='button' onclick='sendData(\"power\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // Stereo / Mono
  htmlPage += "<tr>";
  htmlPage += "<td>Stereo/Mono:</td>";
  htmlPage += "<td><select id='stereo_mono' name='stereo_mono'>";
  htmlPage += "<option value='0' selected>Stereo</option>";
  htmlPage += "<option value='1'>Mono</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"stereo_mono\")'>Set</button></td>";
  htmlPage += "</tr>";

  // PreEmphasis
  htmlPage += "<tr>";
  htmlPage += "<td>Pre-Emphasis:</td>";
  htmlPage += "<td><select id='pre_emphasis' name='pre_emphasis'>";
  htmlPage += "<option value='0' >50 us</option>";
  htmlPage += "<option value='1' selected>75 us</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"pre_emphasis\")'>Set</button></td>";
  htmlPage += "</tr>";

  // RDS PTY (ComboBox)
  htmlPage += "<tr>";
  htmlPage += "<td>RDS PTY:</td>";
  htmlPage += "<td><select id='rds_pty' name='rds_pty'>";
  htmlPage += "<option value='0' selected>No program</option>";
  htmlPage += "<option value='1'>News</option>";
  htmlPage += "<option value='3'>Information</option>";
  htmlPage += "<option value='4'>Sport</option>";
  htmlPage += "<option value='5' >Education</option>";
  htmlPage += "<option value='7'>Culture</option>";
  htmlPage += "<option value='8'>Science</option>";
  htmlPage += "<option value='10'>Pop Music</option>";
  htmlPage += "<option value='16'>Weather</option>";
  htmlPage += "<option value='20'>Religion</option>";
  htmlPage += "<option value='29'>Documentary</option>";
  htmlPage += "<option value='30'>Alarm</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"rds_pty\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // RDS Programa Station (PS)
  htmlPage += "<tr>";
  htmlPage += "<td>RDS PS:</td>";
  htmlPage += "<td><input type='text' id='rds_ps' name='rds_ps' maxlength='8'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"rds_ps\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // RDS Radio Text (RT)
  htmlPage += "<tr>";
  htmlPage += "<td>RDS RT:</td>";
  htmlPage += "<td><input type='text' id='rds_rt' name='rds_rt' maxlength='32'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"rds_rt\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // Linha do RDS DT
  htmlPage += "<tr>";
  htmlPage += "<td>RDS DT (yyyy/mm/dd hh:mm):</td>";
  htmlPage += "<td><input type='text' id='datetime' name='datetime' maxlength='32'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"datetime\")'>Set</button></td>";
  htmlPage += "</tr>";

  // Frequency Derivation list
  htmlPage += "<tr>";
  htmlPage += "<td>Frequency Derivation:</td>";
  htmlPage += "<td><select id='frequency_derivation' name='frequency_derivation'>";
  htmlPage += "<option value='60'>41.5kHz</option>";
  htmlPage += "<option value='87'>60.0kHz</option>";
  htmlPage += "<option value='108' selected>74.5kHz</option>";
  htmlPage += "<option value='120'>92,8kHz</option>";
  htmlPage += "<option value='140'>96.6kHz</option>";
  htmlPage += "<option value='160'>110.4kHz</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"frequency_derivation\")'>Set</button></td>";
  htmlPage += "</tr>";

  // Impedance
  htmlPage += "<tr>";
  htmlPage += "<td>Input Impedance:</td>";
  htmlPage += "<td><select id='input_impedance' name='input_impedance'>";
  htmlPage += "<option value='0'>10K</option>";
  htmlPage += "<option value='1' selected>20K</option>";
  htmlPage += "<option value='2'>40K</option>";
  htmlPage += "<option value='3'>80K</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"input_impedance\")'>Set</button></td>";
  htmlPage += "</tr>";

  // Buffer Gain
  htmlPage += "<tr>";
  htmlPage += "<td>Buffer Gain:</td>";
  htmlPage += "<td><select id='buffer_gain' name='buffer_gain'>";
  htmlPage += "<option value='0'>3dB</option>";
  htmlPage += "<option value='1' selected>6dB</option>";
  htmlPage += "<option value='2'>9dB</option>";
  htmlPage += "<option value='3'>12dB</option>";
  htmlPage += "<option value='4'>15dB</option>";  
  htmlPage += "<option value='5'>18dB</option>";  
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"buffer_gain\")'>Set</button></td>";
  htmlPage += "</tr>";


  // Soft Clip
  htmlPage += "<tr>";
  htmlPage += "<td>Soft Clip:</td>";
  htmlPage += "<td><select id='soft_clip' name='soft_clip'>";
  htmlPage += "<option value='0' selected>Disable</option>";
  htmlPage += "<option value='1'>Enable</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"soft_clip\")'>Set</button></td>";
  htmlPage += "</tr>";


  // Fechar tabela e botão de enviar o formulário completo (solução tradicional)
  htmlPage += "</table><br>";
  htmlPage += "<input type='submit' value='Submit All Parameters'>";
  
  // Close the form definition
  htmlPage += "</form>";
  
  // Script para enviar dados via Ajax (envio individual)
  htmlPage += "<script>";
  htmlPage += "function sendData(fieldId) {";
  htmlPage += "  var value = document.getElementById(fieldId).value;";
  htmlPage += "  var xhr = new XMLHttpRequest();";
  htmlPage += "  xhr.open('POST', '/update', true);";
  htmlPage += "  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  htmlPage += "  xhr.onreadystatechange = function() {";
  htmlPage += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  htmlPage += "      console.log('Response: ' + xhr.responseText);";
  htmlPage += "    }";
  htmlPage += "  };";
  htmlPage += "  xhr.send(fieldId + '=' + encodeURIComponent(value));";
  htmlPage += "}";
  htmlPage += "</script>";
  
  // Fechar corpo e HTML
  htmlPage += "</body></html>";

  // Enviar a página HTML ao cliente
  server.send(200, "text/html", htmlPage);
}


void handleUpdate() {
  String field = server.argName(0);  // Nome do campo enviado (por exemplo, "frequency", "rds_ps")
  String value = server.arg(0);      // Valor enviado

  int nLen;

  // Processa e aplica o valor do campo correspondente
  if (field == "frequency") {
    String frequency = server.arg("frequency");
    currentFrequency = (uint16_t) (frequency.toFloat() * 10);
    if (currentFrequency != previousFrequency) {
      tx.setTX(currentFrequency);  
      previousFrequency = currentFrequency;
    }   
    Serial.println("Frequency updated to: " + String(frequency) + " MHz");
  } else if (field == "power") {
    String power = server.arg("power");
    Serial.println("Power updated to: " + String(power));
  } else if (field == "rds_pty") {
    String rds_pty = server.arg("rds_pty");
    tx.rdsSetPTY(rds_pty.toInt());
    tx.rdsSendPS(ps);
    Serial.println("RDS PTY updated to: " + String(rds_pty));
  } else if (field == "rds_ps") {
    String rds_ps = server.arg("rds_ps");
    nLen = rds_ps.length();
    strncpy(ps, rds_ps.c_str(), nLen);
    ps[nLen] = '\r';
    ps[nLen+1] = '\0';
    tx.rdsSendPS(ps);
    Serial.println("RDS PS updated to: " + String(ps));
  } else if (field == "rds_rt") {
    String rds_rt = server.arg("rds_rt");
    nLen = rds_rt.length();
    strncpy(rt, rds_rt.c_str(), nLen);
    rt[nLen] = '\r';
    rt[nLen+1] = '\0';
    tx.rdsSendRT(rt); 
    Serial.println("RDS RT updated to: " + String(rt));
  } else if (field == "frequency_derivation") {
    String frequency_derivation = server.arg("frequency_derivation");
    tx.setTxFrequencyDerivation(frequency_derivation.toInt());   
    Serial.println("Frequency Derivation updated to: " + String(frequency_derivation)); 
  } else if (field == "input_impedance") {
    String input_impedance = server.arg("input_impedance");
    tx.setTxInputImpedance(input_impedance.toInt());   
    Serial.println("Input Impedance updated to: " + String(input_impedance)); 
  } else if (field == "stereo_mono") {
    String stereo_mono = server.arg("stereo_mono");
    tx.setTxMono(stereo_mono.toInt());   
    Serial.println("Stereo/Mono updated to: " + String(stereo_mono)); 
  } else if (field == "buffer_gain") {
    String buffer_gain = server.arg("buffer_gain");
    tx.setTxInputBufferGain(buffer_gain.toInt());   
    Serial.println("Buffer Gain updated to: " + String(buffer_gain)); 
  } else if (field == "pre_emphasis") {
    String pre_emphasis = server.arg("pre_emphasis");
    tx.setPreEmphasis(pre_emphasis.toInt());   
    Serial.println("Pre-Emphasis updated to: " + String(pre_emphasis)); 
  } else if (field == "soft_clip") {
    String soft_clip = server.arg("soft_clip");
    tx.setTxSoftClippingEnable(soft_clip.toInt());   
    Serial.println("Soft Clip updated to: " + String(soft_clip)); 
  } else if (field == "datetime") {
    String datetime = server.arg("datetime");
    setRTC(datetime);  
    Serial.println("Date Time updated to: " + datetime); 
  }


  // Enviar resposta ao cliente confirmando o recebimento do campo
  server.send(200, "text/plain", field + " has been updated.");
}


// Função para tratar o envio do formulário
void handleFormSubmit() {
  String frequency = server.arg("frequency");
  String power = server.arg("power");
  String rds_pty = server.arg("rds_pty");
  String rds_ps = server.arg("rds_ps");
  String rds_rt = server.arg("rds_rt");
  String frequency_derivation = server.arg("frequency_derivation");
  String input_impedance = server.arg("input_impedance");
  String stereo_mono = server.arg("stereo_mono");
  String buffer_gain = server.arg("buffer_gain");
  String pre_emphasis = server.arg("pre_emphasis");  
  String soft_clip = server.arg("soft_clip");

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
 
  tx.setTxFrequencyDerivation(frequency_derivation.toInt());   
  tx.setTxInputImpedance(input_impedance.toInt());  
  tx.setTxMono(stereo_mono.toInt());  
  tx.setTxInputBufferGain(buffer_gain.toInt());   
  tx.setPreEmphasis(pre_emphasis.toInt());    
  tx.setTxSoftClippingEnable(soft_clip.toInt());     

  server.send(200, "text/html", response);
}


void startRDS() {
    tx.rdsTxEnable(true);
    delay(200);
    tx.rdsInitTx(0x8,0x1,0x9B, 0, 25, 6);  // See: https://pu2clr.github.io/QN8066/extras/apidoc/html/index.html) 
}

// Gets the local Date and Time from ESP32 RTC and send RDS Date Time message
void rdsSendDateTime() {
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    // Sends RDS local Date and Time
    tx.rdsSendDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min);  
}

// Converts String Date format (yyyy/mm/dd hh:mm) to ESP32 RTC and update RDS Date and Time message. 
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
  rdsSendDateTime();
  Serial.println("Internal ESP32 RTC updated!");
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
    rdsSendDateTime();
    rdsDateTime = millis();
  }

}


void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Local Time fails");
    return;
  }
  Serial.println(&timeinfo, "%Y/%m/%d %H:%M:%S");
}


void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // *** Start configuring 32.768K source clock
  // Set up the PWM channel with the desired frequency
  // ledcSetup(ledcChannel, clock_frequency, resolution);
  // Attach the PWM channel to the specified pin
  ledcAttach(ACTIVE_CLOCK32K, clock_frequency, resolution);
  ledcWrite(ledcChannel, 128); // Midpoint value for a square wave signal (0-255, where 128 is 50% duty cycle)
  // *** END Clock setup

  // The line below may be necessary to setup I2C pins on ESP32
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Access Point setup IP Error.");
  }


  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Web server setup
  server.on("/", handleRoot);  // Página principal com o formulário
  server.on("/setParameters", HTTP_POST, handleFormSubmit);  // Rota para o envio tradicional
  server.on("/update", HTTP_POST, handleUpdate);  // Rota para envio individual via Ajax
  server.begin();
  Serial.println("\nServidor HTTP started.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());


  tx.setup(1);  // Devider is 1 (32.768kHz and not 32.768 MHz)
  tx.setTX(currentFrequency);   // Sets frequency to 106.9 MHz 
  delay(500);
  startRDS();  

  // Sets the current RTC based on Network if it is available  
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");

  Serial.println("Trasmitting...");

}

void loop() {
  sendRDS();
  dnsServer.processNextRequest();
  server.handleClient();
}
