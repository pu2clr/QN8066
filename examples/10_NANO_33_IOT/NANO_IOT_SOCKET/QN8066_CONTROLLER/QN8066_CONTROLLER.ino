#include <WiFiNINA.h>

char ssid[] = "PU2CLR";         // Wi-Fi network name
char pass[] = "pu2clr123456";   // Wi-Fi password

int status = WL_IDLE_STATUS;
WiFiServer server(8066);        // Create a server that listens on port 8066

String receivedData = "";
String field = "";
String value = "";

void setup() {
  // Start serial communication
  Serial.begin(9600);

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
}


// Print the Wi-Fi status (IP address, etc.)
void printWiFiStatus() {
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


// Function to process the received field and value
// TODO
void processField(String field, String value) {
  if (field == "frequency") {
    Serial.print("Updating frequency to: ");
    Serial.println(value);
    // Update frequency value
    // e.g., setFrequency(value.toInt());
  } else if (field == "power") {
    Serial.print("Updating power to: ");
    Serial.println(value);
    // Update power value
    // e.g., setPower(value.toInt());
  } else {
    Serial.print("Unknown field: ");
    Serial.println(field);
  }
}

void loop() {
  // Check for an incoming client
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected");

    // Read incoming data from client
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        receivedData += c;

        // Process the data when a newline is received
        if (c == '\n') {
          Serial.print("Received: ");
          Serial.println(receivedData);

          // Parse the received data (format: "field=value")
          int separatorIndex = receivedData.indexOf('=');
          if (separatorIndex > 0) {
            field = receivedData.substring(0, separatorIndex);
            value = receivedData.substring(separatorIndex + 1).trim();  // Remove newline

            // Process the field and value
            processField(field, value);

            // Clear the data for the next message
            receivedData = "";
          }
        }
      }
    }

    // Close the connection with the client
    client.stop();
    Serial.println("Client disconnected");
  }
}
