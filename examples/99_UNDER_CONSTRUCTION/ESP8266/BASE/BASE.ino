/*

  ESP8266 Dev Module Wire up 

  | Device name               | QN8066 Pin           |  ESP8266 Dev Module |
  | --------------------------| -------------------- | ----------------- |
  | QN8066                    |                      |                   | 
  |                           | VCC                  |      3.3V         |
  |                           | GND                  |      GND          |    
  |                           | SDIO / SDA (pin 2)   |      GPIO4 [1 ]   |
  |                           | SCLK (pin 1)         |      GPIO5 [1]    |
  | --------------------------| ---------------------| ----------------- |
  | PWM Power Controller [2]  |                      |                   |
  |                           |                      |      GPIO8        | 

  1. It can change if you are not using the ESP8266 Dev Module. Check you ESP8266 board pinout 
  2. A suggestion if you intend to use PWM to control the RF output power of an amplifier.  

*/

#include <ESP8266WiFi.h>
#include <QN8066.h>


#define ESP8266_I2C_SDA 4    
#define ESP8266_I2C_SCL 5    
#define PWM_PIN  8


const char* ssid = "ESP8266_QN8066";
const char* password = "12345678";

#define RDS_PS_REFRESH_TIME 7000
#define RDS_RT_REFRESH_TIME 17000
#define RDS_DT_REFRESH_TIME 59000 // Date and Time Service


long rdsTimePS = millis();
long rdsTimeRT = millis();
long rdsDateTime = millis();

uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

uint8_t currentPower = 0;

char ps[9] = "PU2CLR \r";
char rt[33] = "QN8066 WEB Server control      \r";


String firstName = "";
String lastName = "";
long socialNumber = 0;

WiFiServer server(80);

QN8066 tx;

void setup() {
  // Inicia a comunicação serial para monitoramento
  Serial.begin(115200);
  Serial.println();

  // Configura o ESP8266 como Access Point com o IP fixo 10.0.0.2
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(IPAddress(10, 0, 0, 2), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));

  // Exibe o IP do AP no monitor serial
  Serial.print("Access Point iniciado. IP: ");
  Serial.println(WiFi.softAPIP());

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor iniciado.");
}


void handleRoot(WiFiClient *client) {
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
  htmlPage += "<h1>FM Transmitter Configuration with ESP8266 Active Portal</h1>";


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

  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println();
  client->println(htmlPage);
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

void handleUpdate(WiFiClient *client) {

  String postBody = client->readString();

  int nLen;

  if (postBody.indexOf("frequency=") >=0 ) {
    String frequency = getValue(postBody, "frequency")
    currentFrequency = (uint16_t) (frequency.toFloat() * 10);
    if (currentFrequency != previousFrequency) {
      tx.setTX(currentFrequency);  
      previousFrequency = currentFrequency;
    }   
    Serial.println("Frequency updated to: " + String(frequency) + " MHz");
  } else if (postBody.indexOf("power=") >=0)  {
    int power = getValue(postBody, "power").toInt();
    int pwm = map(power, 0, 16, 0, 255);
    analogWrite(PWM_PIN, pwm);    // Sets the PWM correspondent to power value
    Serial.println("Power updated to: " + String(power));
  } else if (postBody.indexOf("rds_pty") >= 0) {
    String rds_pty = getValue(postBody,"rds_pty");
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




void loop() {
  // Aguarda a conexão de um cliente
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Aguarda até que o cliente envie dados
  while (!client.available()) {
    delay(1);
  }

  // Lê a requisição completa do cliente
  String request = client.readString();
  Serial.println(request);

  // Verifica se a requisição é do tipo POST e extrai os dados
  if (request.startsWith("POST /submit")) {
    // Encontra o início do corpo da requisição (os dados enviados)
    int bodyIndex = request.indexOf("\r\n\r\n") + 4;
    String body = request.substring(bodyIndex);

    // Extrai os valores enviados no corpo da requisição
    int firstNameIndex = body.indexOf("FirstName=") + 10;
    int lastNameIndex = body.indexOf("&LastName=") + 10;
    int socialNumberIndex = body.indexOf("&SocialNumber=") + 14;

    firstName = body.substring(firstNameIndex, body.indexOf('&', firstNameIndex));
    lastName = body.substring(lastNameIndex, body.indexOf('&', lastNameIndex));
    socialNumber = body.substring(socialNumberIndex).toInt();

    // Exibe os dados no monitor serial
    Serial.println("First Name: " + firstName);
    Serial.println("Last Name: " + lastName);
    Serial.println("Social Number: " + String(socialNumber));
  }

  handleRoot(&client);

  /*
  // Envia uma resposta HTML com o formulário
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Formulário de Cadastro</h1>");
  client.println("<form action='/submit' method='POST'>");
  client.println("First Name: <input type='text' name='FirstName'><br>");
  client.println("Last Name: <input type='text' name='LastName'><br>");
  client.println("Social Number: <input type='number' name='SocialNumber'><br>");
  client.println("<input type='submit' value='Enviar'>");
  client.println("</form>");
  client.println("</html>");
  */

  // Aguarda até que o cliente desconecte
  delay(1);
  Serial.println("Cliente desconectado.");
}



