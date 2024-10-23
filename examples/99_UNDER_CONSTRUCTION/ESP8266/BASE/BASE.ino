#include <ESP8266WiFi.h>

// Definição do SSID e senha da rede Wi-Fi criada pelo ESP8266
const char* ssid = "ESP8266_AP";
const char* password = "12345678";

// Variáveis para armazenar os dados recebidos
String firstName = "";
String lastName = "";
long socialNumber = 0;

// Cria o servidor HTTP na porta 80
WiFiServer server(80);

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

  // Aguarda até que o cliente desconecte
  delay(1);
  Serial.println("Cliente desconectado.");
}
