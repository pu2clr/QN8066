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
#include <WiFiNINA.h>



long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

char ps[9] = "PU2CLR \r";
char rt[33] = "QN8066 WEB Server control      \r";

uint16_t currentFrequency = 1069;   // 106.9 MHz
uint16_t previousFrequency = 1069;  // 106.9 MHz
uint8_t currentPower = 0;


// Define the SSID and password for the access point
char ssid[] = "QN8066";  // Wi-Fi network name for the access point
char pass[] = "qn8066123456";  // Wi-Fi password for the access point
IPAddress localIP(10, 0, 0, 1);     // Fixed IP for the access point
IPAddress subnet(255, 255, 255, 0); // Subnet mask
WiFiServer server(80);  // Sets up the HTTP server on port 80
WiFiClient client;


void setup() {
  // Initialize serial communication
  Serial.begin(9600);


  WiFi.setHostname("qn8066");
  // Check if the Wi-Fi module is available
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Failed to detect the Wi-Fi module!");
    while (true);
  }

  WiFi.config(localIP);

  // Configure the Arduino as an access point
  Serial.println("Setting up the access point...");
  WiFi.beginAP(ssid, pass);


  // Start the HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void doFormParameters() {
  String htmlPage = "<html><head>";
  htmlPage += "<style>";
  htmlPage += "body { font-family: Arial, sans-serif; background-color: #006400; color: yellow;";  // Fundo verde Amazonas (#006400) e texto amarelo
  htmlPage += " display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  htmlPage += "h1 { text-align: center; margin-bottom: 20px; color: yellow; }";  // Título com fonte amarela
  htmlPage += "table { border-collapse: collapse; width: auto; margin-top: 20px; }";
  htmlPage += "td, th { border: 1px solid black; padding: 8px; color: yellow; }";                                                       
  htmlPage += "td input[type='text'], td select { width: 32ch; color: yellow; background-color: #004d00; border: 1px solid yellow; }";  
  htmlPage += "td:nth-child(1) { text-align: right; }";                                                                                 
  htmlPage += "</style>";

  htmlPage += "</head><body>";

  htmlPage += "<h1>PU2CLR - QN8066 Arduino Library</h1>";
  htmlPage += "<h2>FM Transmitter Controlled by ESP8266 via Wi-Fi</h2>";


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
  // htmlPage += "</table><br>";
  // htmlPage += "<input type='submit' value='Submit All Parameters'>";

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

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println(htmlPage);
}

// Function to process POST requests
void processPostRequest() {
  // Read the body of the POST request (the form data)
  String postBody = client.readString();
  Serial.println("Received POST data:");
  Serial.println(postBody);

  // Parse and process the parameters sent via POST
  if (postBody.indexOf("frequency=") >= 0) {
    String freqStr = getValue(postBody, "frequency");
    currentFrequency = freqStr.toInt();
    Serial.print("Frequency set to: ");
    Serial.println(currentFrequency);
  }

  if (postBody.indexOf("power=") >= 0) {
    String powerStr = getValue(postBody, "power");
    currentPower = powerStr.toInt();
    Serial.print("Power set to: ");
    Serial.println(currentPower);
  }

  // Respond to the client with a success message
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/plain");
  client.println();
  client.println("Parameters updated successfully!");
  client.println();
}

// Function to extract value of a specific parameter from the POST body
String getValue(String data, String field) {
  int startIndex = data.indexOf(field + "=") + field.length() + 1;
  int endIndex = data.indexOf('&', startIndex);
  if (endIndex == -1) {
    endIndex = data.length();
  }
  return data.substring(startIndex, endIndex);
}


void doNothing() {

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/plain");
  client.println();
  client.println("Nothing!");
  client.println();

}

void loop() {
  // Check if there are any connected clients
  client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Wait for the client's HTTP request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.readStringUntil('\n');  // Skip the remaining headers

    // Process the request manually based on URL
    if (request.indexOf("GET / ") >= 0) {
       Serial.println("---- 1");
      doFormParameters();  // Route for the root page
    }
    else if (request.indexOf("POST /setParameters") >= 0) {
      Serial.println("---- 2");
      doFormParameters();  // Route for handling form submission
    }
    else if (request.indexOf("POST /update") >= 0) {
      Serial.println("---- 3");
      processPostRequest();  // Route for handling AJAX updates
    }
    else {
      doNothing();  // Handle 404 - Not Found
    }

    // Close the connection with the client
    client.stop();
    Serial.println("Client disconnected");
  }
}


