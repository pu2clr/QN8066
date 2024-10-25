#include <EEPROM.h>
#include <ArduinoJson.h> // Instale a biblioteca ArduinoJson (https://arduinojson.org/)

const int EEPROM_SIZE = 96;

struct UserData {
  char firstName[30];
  char lastName[30];
  char socialNumber[30];
};

// Função para gravar dados na EEPROM
void saveToEEPROM(UserData &data) {
  EEPROM.put(0, data);
}

// Função para ler dados da EEPROM
UserData readFromEEPROM() {
  UserData data;
  EEPROM.get(0, data);
  return data;
}

void setup() {
  Serial.begin(9600);  // Comunicação Bluetooth
  EEPROM.begin();  // Inicializa EEPROM
  
  Serial.println("Arduino pronto para receber dados via Bluetooth...");
}

void loop() {
  if (Serial.available()) {
    String json = Serial.readStringUntil('\n'); // Recebe o JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (!error) {
      UserData data;
      strlcpy(data.firstName, doc["firstName"] | "", sizeof(data.firstName));
      strlcpy(data.lastName, doc["lastName"] | "", sizeof(data.lastName));
      strlcpy(data.socialNumber, doc["socialNumber"] | "", sizeof(data.socialNumber));

      saveToEEPROM(data); // Grava na EEPROM
      Serial.println("Dados armazenados com sucesso!");
    } else {
      // Solicitação para enviar os dados salvos
      if (json == "GET_DATA") {
        UserData data = readFromEEPROM();
        JsonDocument response;
        response["firstName"] = data.firstName;
        response["lastName"] = data.lastName;
        response["socialNumber"] = data.socialNumber;

        String responseData;
        serializeJson(response, responseData);
        Serial.println(responseData); // Envia resposta via Bluetooth
      }
    }
  }
}
