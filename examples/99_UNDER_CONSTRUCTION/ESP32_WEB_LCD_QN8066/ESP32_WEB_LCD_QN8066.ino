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


// Wi-Fi setup
const char* ssid = "Your WIFI SSID";
const char* password = "Your password";

// Web server
WebServer server(80);

QN8066 tx;

void handleRoot() {
  String htmlPage = "<html><head>";
 
  // HTML page and form setup
  htmlPage += "<style>";
  htmlPage += "body { font-family: Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  htmlPage += "h1 { text-align: center; margin-bottom: 20px; }"; // Centraliza o título e coloca espaço abaixo
  htmlPage += "table { border-collapse: collapse; width: auto; margin-top: 20px; }"; // A tabela fica abaixo do título
  htmlPage += "td, th { border: 1px solid black; padding: 8px; }";
  htmlPage += "td input[type='text'], td select { width: 32ch; }"; // Define a largura dos campos de texto e select
  htmlPage += "td:nth-child(1) { text-align: right; }"; // Alinha os rótulos à direita
  htmlPage += "</style>";
  
  htmlPage += "</head><body>";

  htmlPage += "<h1>PU2CLR QN8066 Arduino Library</h1>";
  htmlPage += "<h1>FM Transmitter Configuration</h1>";


  // Open Form
  htmlPage += "<form method='POST' action='/setParameters'>";
  
  // Open Table
  htmlPage += "<table>";
  
  // Frequency
  htmlPage += "<tr>";
  htmlPage += "<td>Transmission Frequency (MHz):</td>";
  htmlPage += "<td><input type='text' id='frequency' name='frequency' maxlength='6' size='6'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"frequency\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // Power
  htmlPage += "<tr>";
  htmlPage += "<td>Power:</td>";
  htmlPage += "<td><input type='text' id='power' name='power'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"power\")'>Set</button></td>";
  htmlPage += "</tr>";
  
  // Program Type list
  htmlPage += "<tr>";
  htmlPage += "<td>RDS PTY:</td>";
  htmlPage += "<td><select id='rds_pty' name='rds_pty'>";
  htmlPage += "<option value='0'>No program</option>";
  htmlPage += "<option value='1'>News</option>";
  htmlPage += "<option value='3'>Information</option>";
  htmlPage += "<option value='4'>Sport</option>";
  htmlPage += "<option value='5'>Education</option>";
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
  
  //RDS Program Station (PS)
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
  
  // RDS Date and Time setup
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
  htmlPage += "<option value='108'>74.5kHz</option>";
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
  htmlPage += "<option value='1'>20K</option>";
  htmlPage += "<option value='2'>40K</option>";
  htmlPage += "<option value='3'>80K</option>";
  htmlPage += "</select></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"input_impedance\")'>Set</button></td>";
  htmlPage += "</tr>";


  htmlPage += "<tr>";
  htmlPage += "<td>Stereo/Mono:</td>";
  htmlPage += "<td><button type='button' id='stereoToggle' onclick='toggleStereo()'>Stereo</button></td>";
  htmlPage += "</tr>";

  // Fechar tabela e botão de enviar o formulário completo (solução tradicional)
  htmlPage += "</table><br>";
  htmlPage += "<input type='submit' value='Submit All Parameters'>";
  
  // Close form
  htmlPage += "</form>";
  
  htmlPage += "<script>";
  htmlPage += "var isStereo = true;";

  htmlPage += "function toggleStereo() {";
  htmlPage += "  var button = document.getElementById('stereoToggle');";
  htmlPage += "  if (isStereo) {";
  htmlPage += "    button.innerHTML = 'Mono';";
  htmlPage += "    isStereo = false;";
  htmlPage += "  } else {";
  htmlPage += "    button.innerHTML = 'Stereo';";
  htmlPage += "    isStereo = true;";
  htmlPage += "  }";
  htmlPage += "  // Envia a mudança de status Stereo/Mono via Ajax";
  htmlPage += "  sendData('stereo');";
  htmlPage += "}";

  htmlPage += "function sendData(fieldId) {";
  htmlPage += "  var value;";
  htmlPage += "  if (fieldId == 'stereo') {";
  htmlPage += "    value = isStereo ? 'Stereo' : 'Mono';";
  htmlPage += "  } else {";
  htmlPage += "    value = document.getElementById(fieldId).value;";
  htmlPage += "  }";

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
  }

  server.send(200, "text/plain", field + " has been updated.");
}


// Handle Forms 
void handleFormSubmit() {

  String frequency = server.arg("frequency");
  String power = server.arg("power");
  String rds_pty = server.arg("rds_pty");
  String rds_ps = server.arg("rds_ps");
  String rds_rt = server.arg("rds_rt");
  String frequency_derivation = server.arg("frequency_derivation");
  String input_impedance = server.arg("input_impedance");

  // Aqui você pode adicionar o código para enviar essas configurações para o QN8066
  // Como por exemplo, configurar a frequência de transmissão, potência, e os parâmetros RDS.

  String response = "<html><body>";
  response += "<h1>Settings Received</h1>";
  response += "<p>Frequency: " + frequency + " MHz</p>";
  response += "<p>Power%: " + power + "</p>";
  response += "<p>RDS PTY: " + rds_pty + "</p>";
  response += "<p>RDS PS: " + rds_ps + "</p>";
  response += "<p>RDS RT: " + rds_rt + "</p>";
  response += "<p>Frequency Derivation: " + frequency_derivation + "</p>";
  response += "<p>Input Impedance: " + input_impedance + "</p>";

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

  server.send(200, "text/html", response);
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


void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Conecta na rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Wi-Fi connected!");

  // Web server setup
  server.on("/", handleRoot);  // Página principal com o formulário
  server.on("/setParameters", HTTP_POST, handleFormSubmit);  // Rota para o envio tradicional
  server.on("/update", HTTP_POST, handleUpdate);  // Rota para envio individual via Ajax
  server.begin();
  Serial.println("\nServidor HTTP started.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  


  tx.setup();
  tx.setTX(currentFrequency);   // Sets frequency to 106.9 MHz 
  delay(500);
  startRDS();  

  Serial.println("Trasmitting...");

}


void loop() {
  sendRDS();
  server.handleClient();
}
