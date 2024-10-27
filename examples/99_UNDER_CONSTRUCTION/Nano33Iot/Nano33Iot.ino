#include <WiFiNINA.h>
#include <Arduino.h>
#include <QN8066.h>

// Configuração do Access Point
const char* ssid = "Nano33IoT-QN8066";
const char* password = "12345678";

// Criação do servidor na porta 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Inicia o Access Point
  WiFi.beginAP(ssid, password);
  Serial.print("Configurando AP: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_AP_LISTENING) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nAP Pronto!");

  // Inicia o servidor HTTP
  server.begin();
  Serial.println("Servidor HTTP Iniciado");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Cliente conectado");

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("GET /") >= 0) {
      sendHTML(client);
    }

    // 
    if (request.indexOf("GET /submit?field=firstName&value=") >= 0) {
      String value = parseValue(request, "firstName");
      Serial.println("First Name: " + value);
      client.println("HTTP/1.1 200 OK");
      client.println();
      client.println("First Name recebido");
    } else if (request.indexOf("GET /submit?field=lastName&value=") >= 0) {
      String value = parseValue(request, "lastName");
      Serial.println("Last Name: " + value);
      client.println("HTTP/1.1 200 OK");
      client.println();
      client.println("Last Name recebido");
    } else if (request.indexOf("GET /submit?field=socialNumber&value=") >= 0) {
      String value = parseValue(request, "socialNumber");
      Serial.println("Social Number: " + value);
      client.println("HTTP/1.1 200 OK");
      client.println();
      client.println("Social Number recebido");
    }
  }
}

void sendHTML(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println(R"HTML(
    <!DOCTYPE html>
    <html>
      <head>
        <title>Formulário AJAX</title>
        <script>
          function sendData(field) {
            const value = document.getElementById(field).value;
            const xhr = new XMLHttpRequest();
            xhr.open("GET", `/submit?field=${field}&value=${value}`, true);
            xhr.send();
          }
        </script>
      </head>
      <body>
        <h2>Formulário com AJAX</h2>
        <div>
          <label>First Name: </label>
          <input type="text" id="firstName">
          <button onclick="sendData('firstName')">Enviar</button>
        </div>
        <div>
          <label>Last Name: </label>
          <input type="text" id="lastName">
          <button onclick="sendData('lastName')">Enviar</button>
        </div>
        <div>
          <label>Social Number: </label>
          <input type="text" id="socialNumber">
          <button onclick="sendData('socialNumber')">Enviar</button>
        </div>
      </body>
    </html>
  )HTML");
}

String parseValue(String request, String field) {
  int start = request.indexOf("value=") + 6;
  int end = request.indexOf(" ", start);
  String value = request.substring(start, end);
  value.replace("%20", " ");  // Substituir espaços codificados
  return value;
}
