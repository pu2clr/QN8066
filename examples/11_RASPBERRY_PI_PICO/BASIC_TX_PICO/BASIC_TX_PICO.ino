/*
   QN8066 Arduino Library Basic TX Example for Raspberry Pi Pico
   
   This sketch demonstrates how to use the QN8066 library with the Raspberry Pi Pico
   to create a basic FM transmitter. The QN8066 will transmit on a specified frequency
   with stereo audio capability.

   Author: Ricardo Lima Caratti (PU2CLR) - pu2clr@gmail.com
   December 2024

   Connections:
   Raspberry Pi Pico  |  QN8066
   -------------------|----------
   3.3V (Pin 36)      |  VCC
   GND (Pin 38)       |  GND
   GP0 (Pin 1)        |  SDA
   GP1 (Pin 2)        |  SCL
   GP16 (Pin 21)      |  CEN (Chip Enable - Optional)
   
   Audio Input:
   Audio Left         |  ALI (Pin 10)
   Audio Right        |  ARI (Pin 9)
   Audio GND          |  GND
   
   External Components:
   - 32.768 kHz active crystal oscillator connected to XCLK (Pin 14)
   - Antenna connected to RFO (Pin 12)

   IMPORTANT NOTES:
   1. The Raspberry Pi Pico operates at 3.3V, which is compatible with the QN8066
   2. Make sure to use an active crystal oscillator, not a passive crystal
   3. Use proper antenna matching for optimal transmission
   4. Comply with local regulations for FM transmission
   5. For testing, use a dummy load instead of an antenna
*/

#include <QN8066.h>

// Pin definitions for Raspberry Pi Pico
#define SDA_PIN 0      // GP0 - I2C SDA
#define SCL_PIN 1      // GP1 - I2C SCL 
#define CEN_PIN 16     // GP16 - Chip Enable (optional)

// Transmission parameters
#define FREQUENCY 1069  // 106.9 MHz - Change this to your desired frequency
#define TX_POWER 55
QN8066 tx;
char status_msg[100];

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial connection
  }
  
  Serial.println("\n==================================");
  Serial.println("QN8066 Basic TX - Raspberry Pi Pico");
  Serial.println("==================================");
  
  // Initialize I2C with custom pins for Pico
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  Wire.begin();
  
  // Optional: Setup chip enable pin
  if (CEN_PIN >= 0) {
    pinMode(CEN_PIN, OUTPUT);
    digitalWrite(CEN_PIN, HIGH); // Enable the chip
    delay(100);
  }
  
  Serial.print("Detecting QN8066 device... ");
  if (!tx.detectDevice()) {
    Serial.println("FAILED!");
    Serial.println("QN8066 device not detected.");
    Serial.println("Please check your connections:");
    Serial.println("- VCC to 3.3V");
    Serial.println("- GND to GND");
    Serial.println("- SDA to GP0");
    Serial.println("- SCL to GP1");
    Serial.println("- Active crystal to XCLK");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("SUCCESS!");
  
  Serial.println("Initializing QN8066...");
  
  // Setup QN8066 with basic parameters
  // Parameters: crystal_divider, mono_mode, rds_enable, preemphasis, crystal_type
  tx.setup(1,      // Divider for 32.768 kHz crystal
           false,  // Stereo mode (false = stereo, true = mono)
           false,  // RDS disabled for basic example
           1,      // Pre-emphasis 75μs (1 = 75μs, 0 = 50μs)
           1);     // Digital clock type (1 = digital, 0 = sine wave)
  
  Serial.println("Setting transmission frequency...");
  tx.setTX(FREQUENCY);
  
  Serial.println("Setting transmission power...");
  setPower(TX_POWER);
  
  Serial.println("Enabling stereo transmission...");
  tx.setTxStereo(true);
  
  sprintf(status_msg, "Broadcasting on %.1f MHz", FREQUENCY / 10.0);
  Serial.println(status_msg);
  Serial.println("Connect audio source to ALI/ARI pins");
  Serial.println("Connect antenna to RFO pin (or dummy load for testing)");
  Serial.println("\nTransmission Status:");
  Serial.println("====================");
}

void setPower(int power) { 
  if (power < 25) power = 25;
  if (power > 55) power = 55;
  tx.setPAC(power);
} 

void loop() {
  // Get and display system status
  qn8066_status1 status = tx.getStatus1();
  
  // Get audio peak level
  int audioPeak = tx.getAudioPeakValue();
  
  // Display status information
  sprintf(status_msg, "FSM State: %d | Audio Peak: %d mV | RX Status: %s | Stereo: %s", 
          status.arg.FSM, 
          audioPeak,
          status.arg.RXSTATUS ? "ON" : "OFF",
          status.arg.ST_MO_RX ? "MONO" : "STEREO");
  
  Serial.println(status_msg);
  
  // Reset audio peak measurement for next reading
  tx.resetAudioPeak();
  
  // Check FSM state for troubleshooting
  if (status.arg.FSM == 10) {
    Serial.println("Status: TRANSMITTING - OK");
  } else {
    Serial.print("Status: FSM State ");
    Serial.print(status.arg.FSM);
    switch(status.arg.FSM) {
      case 0: Serial.println(" - STANDBY"); break;
      case 1: Serial.println(" - RESET"); break;
      case 2: Serial.println(" - CALIBRATING"); break;
      case 3: Serial.println(" - IDLE"); break;
      case 4: Serial.println(" - PLL CALIBRATION"); break;
      case 7: Serial.println(" - TX PLL CALIBRATION"); break;
      case 8: Serial.println(" - TX RESET"); break;
      case 9: Serial.println(" - PA CALIBRATION"); break;
      case 11: Serial.println(" - TX CCA"); break;
      default: Serial.println(" - UNKNOWN"); break;
    }
  }
  
  delay(5000); // Update status every 5 seconds
}
