#include <WiFiNINA.h> // Install from https://www.arduino.cc/reference/en/libraries/wifinina/

// Definição da rede Wi-Fi
char ssid[] = "APRC";       // Coloque o nome da sua rede Wi-Fi
char pass[] = "Ap69Rc642023";      // Coloque a senha da sua rede Wi-Fi


uint16_t currentFrequency = 1069; // 106.9 MHz
uint16_t previousFrequency = 1069; // 106.9 MHz

uint8_t currentPower = 0;


int status = WL_IDLE_STATUS;    // Variável para armazenar o status da conexão
WiFiServer server(80);          // Cria um servidor na porta 80

WiFiClient client; 

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(9600);

  // Verifica o status do módulo Wi-Fi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Falha ao detectar o módulo Wi-Fi!");
    while (true);
  }

  // Conectando à rede Wi-Fi
  while (status != WL_CONNECTED) {
    Serial.print("Tentando se conectar a: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    delay(10000);  // Aguarde um pouco para tentar reconectar
  }

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor iniciado");
  
  // Mostra o IP do dispositivo
  printWiFiStatus();
}

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
  htmlPage += "<h1>FM Transmitter Configuration</h1>";


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
  /*
  htmlPage += "<tr>";
  htmlPage += "<td>RDS DT:</td>";
  htmlPage += "<td><input type='text' id='rds_dt' name='rds_dt' maxlength='32'></td>";
  htmlPage += "<td><button type='button' onclick='sendData(\"rds_dt\")'>Set</button></td>";
  htmlPage += "</tr>";
  */
  


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
  client.println(htmlPage);
}



void loop() {
  // Verifica se há clientes conectados
  client = server.available();
  if (client) {
    Serial.println("Novo cliente conectado");
    
    // Aguarda até o cliente enviar dados
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();

        // Responde com uma página HTML simples
        // client.println("HTTP/1.1 200 OK");
        // client.println("Content-type:text/html");
        // client.println();
        // client.println("<html><body><h1>Servidor HTTP Arduino Nano 33 IoT</h1>");
        // client.println("<p>Conectado com sucesso!</p>");
        // client.println("</body></html>");
        // client.println();
        handleRoot();
        break;
      }
    }
    
    // Fecha a conexão com o cliente
    client.stop();
    Serial.println("Cliente desconectado");
  }
}

// Função auxiliar para mostrar o status da rede Wi-Fi
void printWiFiStatus() {
  // Mostra o IP do dispositivo
  Serial.print("IP do Arduino: ");
  Serial.println(WiFi.localIP());
}
