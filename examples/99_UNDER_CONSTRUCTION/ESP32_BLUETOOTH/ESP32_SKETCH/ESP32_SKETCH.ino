#include <BluetoothSerial.h>
#include <ArduinoJson.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");  // Nome Bluetooth do ESP32
  Serial.println("ESP32 pronto para conectar via Bluetooth.");
}

void loop() {
  if (SerialBT.available()) {
    String jsonString = SerialBT.readStringUntil('\n');  // Recebe o JSON
    Serial.println("Recebido: " + jsonString);

    // Desempacotando o JSON recebido
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
      Serial.println("Falha ao parsear JSON");
      return;
    }

    String firstName = doc["first_name"];
    String lastName = doc["last_name"];
    String socialNumber = doc["social_number"];

    Serial.printf("Nome: %s %s, Social: %s\n", 
                  firstName.c_str(), lastName.c_str(), socialNumber.c_str());

    // Enviar de volta o mesmo JSON para confirmar
    String response;
    serializeJson(doc, response);
    SerialBT.println(response);
  }
}
