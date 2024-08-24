#include <QN8066.h>

QN8066 rx;

void setup() {
  Serial.begin(9600);
  rx.begin();
  Serial.println("\nTuning at 107.9Mhz");
  rx.setRX(1079);  // Tuning at 107.9MHz
  delay(10000);
  Serial.println("\nTuning at 93.9Mhz");  
  rx.setRxFrequency(939); // 93.9 MHz

}

void loop() {
  delay(5);
}