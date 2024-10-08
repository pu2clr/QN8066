/*
  Nano 33 IoT board

1. This sketch uses HTML form to get and process commands from external browser applications to configure the 
   FM transmitter based on QN8066.

  Nano 33 IoT Wire up 

  | Device name               | QN8066 Pin           |  Nano 33 IoT      |
  | --------------------------| -------------------- | ----------------- |
  | QN8066                    |                      |                   | 
  |                           | VCC                  |      3.3V         |
  |                           | GND                  |      GND          |    
  |                           | SDIO / SDA (pin 2)   |      A4           |
  |                           | SCLK (pin 1)         |      A5           |
  | --------------------------| ---------------------| ----------------- |
  | PWM Power Controller [1]  |                      |                   |
  |                           |                      |      D9           | 

  1. A suggestion if you intend to use PWM to control the RF output power of an amplifier.  

  Prototype documentation: https://pu2clr.github.io/QN8066/
  PU2CLR QN8066 API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo,  Oct  2024.
*/

#include <QN8066.h>
#include <WiFiNINA.h>  // Install from https://www.arduino.cc/reference/en/libraries/wifinina/

#define I2C_SDA 4  // A4
#define I2C_SCL 5  // A5


// Definição da rede Wi-Fi
char ssid[] = "PU2CLR";        // Coloque o nome da sua rede Wi-Fi
char pass[] = "pu2clr123456";  // Coloque a senha da sua rede Wi-Fi


long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

char ps[9] = "PU2CLR \r";
char rt[33] = "QN8066 WEB Server control      \r";

uint16_t currentFrequency = 1069;   // 106.9 MHz
uint16_t previousFrequency = 1069;  // 106.9 MHz
uint8_t currentPower = 0;


int status = WL_IDLE_STATUS;  // Variável para armazenar o status da conexão
WiFiServer server(80);        // Cria um servidor na porta 80

WiFiClient client;

QN8066 tx;

void setup() {

  Serial.begin(9600);
  Serial.println("Starting WiFi!");

  // Verifica o status do módulo Wi-Fi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi Error!");
    while (true)
      ;
  }

  while (status != WL_CONNECTED) {
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  // Starting Server
  server.begin();
  Serial.println("Server started");

  Serial.print("IP do Arduino: ");
  Serial.println(WiFi.localIP());
}

void doFormParameters() {
  String htmlPage = "<html><head>";
  htmlPage += "<style>";
  htmlPage += "body { font-family: Arial, sans-serif; background-color: #006400; color: yellow;";  // Fundo verde Amazonas (#006400) e texto amarelo
  htmlPage += " display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  htmlPage += "h1 { text-align: center; margin-bottom: 20px; color: yellow; }";  // Título com fonte amarela
  htmlPage += "table { border-collapse: collapse; width: auto; margin-top: 20px; }";
  htmlPage += "td, th { border: 1px solid black; padding: 8px; color: yellow; }";                                                       // Texto da tabela amarelo
  htmlPage += "td input[type='text'], td select { width: 32ch; color: yellow; background-color: #004d00; border: 1px solid yellow; }";  // Campos com fundo verde escuro e borda amarela
  htmlPage += "td:nth-child(1) { text-align: right; }";                                                                                 // Alinha os rótulos à direita
  htmlPage += "</style>";

  htmlPage += "</head><body>";

  htmlPage += "<h1>PU2CLR QN8066 Arduino Library</h1>";
  htmlPage += "<h1>FM Transmitter Configuration</h1>";


  // Formulário com Ajax e solução tradicional
  htmlPage += "<form method='POST' action='/setParameters'>";

  // Tabela para alinhar campos e botões
  htmlPage += "<table>";

  // Frequency
  htmlPage += "<tr>";
  htmlPage += "<td>Transmission Frequency (MHz):</td>";
  htmlPage += "<td><input type='text' id='frequency' name='frequency' maxlength='6' size='6' value='" + String((float)currentFrequency / 10.0) + "'></td>";
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
  htmlPage += "<td>RDS DT:</td>";
  htmlPage += "<td><input type='text' id='rds_dt' name='rds_dt' maxlength='32'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"rds_dt\")'>Set</button></td>";
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

  htmlPage += "</body></html>";
  client.println(htmlPage);
}


String getValue(String data, String field) {
  int startIndex = data.indexOf(field + "=") + field.length() + 1;
  int endIndex = data.indexOf('&', startIndex);
  if (endIndex == -1) {
    endIndex = data.length();
  }
  return data.substring(startIndex, endIndex);
}


void doFormUpdate() {
  String body = client.readString();
  Serial.println("Request: " + body);

  int nLen;

  // Processa e aplica o valor do campo correspondente
  if (body.indexOf("frequency=") >= 0) {
    String frequency = getValue(body, "frequency");
    currentFrequency = (uint16_t)(frequency.toFloat() * 10);
    if (currentFrequency != previousFrequency) {
      tx.setTX(currentFrequency);
      previousFrequency = currentFrequency;
    }
    Serial.println("Frequency updated to: " + String(frequency) + " MHz");
  }
  /* else if (field == "power") {
    String power = server.arg("power");
    Serial.println("Power updated to: " + String(power));
  } else if (field == "rds_pty") {
    String rds_pty = server.arg("rds_pty");
    tx.rdsSetPTY(rds_pty.toInt());
    Serial.println("RDS PTY updated to: " + String(rds_pty));
  } else if (field == "rds_ps") {
    String rds_ps = server.arg("rds_ps");
    nLen = rds_ps.length();
    strncpy(ps, rds_ps.c_str(), nLen);
    ps[nLen] = '\r';
    ps[nLen+1] = '\0';
    Serial.println("RDS PS updated to: " + String(ps));
  } else if (field == "rds_rt") {
    String rds_rt = server.arg("rds_rt");
    nLen = rds_rt.length();
    strncpy(rt, rds_rt.c_str(), nLen);
    rt[nLen] = '\r';
    rt[nLen+1] = '\0';
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
  }
  */
  // Enviar resposta ao cliente confirmando o recebimento do campo
  // server.send(200, "text/plain", field + " has been updated.");
}


void loop() {
  // Verifica se há clientes conectados
  client = server.available();
  if (client) {
    Serial.println("Connected");
    String request = client.readStringUntil('\r');
    client.flush();
    // Aguarda até o cliente enviar dados
    if (request.indexOf("POST") >= 0) {
      while (client.connected()) {
        if (client.available()) {
          String request = client.readStringUntil('\r');
          Serial.println(request);
          client.flush();          
          break;
        }
        doFormUpdate();
      }
    } else {
      doFormParameters();  
    }
    client.stop();
    Serial.println("Connection Closed");
  }
}
