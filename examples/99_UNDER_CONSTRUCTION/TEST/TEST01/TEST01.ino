#include <LiquidCrystal_I2C.h>
#include <QN8066.h>

#define DOWN_BUTTON_PIN 10
#define UP_BUTTON_PIN 11
#define PWM_PIN 9

uint16_t FREQ = 1069;

QN8066 tx;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.noCursor();
  lcd.clear();

  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT);

  // QN8066 PROCESSOR CONFIGURATION
  tx.setup();
  delay(500);
  tx.setTX(FREQ);

  
  tx.setTxOffAfterOneMinuteNoAudio(false); // The transmitter will never sleep.
   tx.setPAC(56); // PA output power target is 0.91*PA_TRGT+70.2dBu. Valid values
                 // are 24-56.
  tx.setToggleTxPdClear();
  tx.setTxPilotGain(10);
  tx.setTxStereo(true);
  tx.setTxPreEmphasis(75);
  tx.setTxInputImpedance(0);  // 0=10; 1 = 20; 2=40 (default); 3=80. Kohms.
  tx.setTxInputBufferGain(5); // With input impedance 0 (10K), and input buffer
                              // 5, the gain shoud be 18dB
  tx.setTxSoftClippingEnable(false);
  tx.setTxSoftClipThreshold(2);
  tx.setAudioTxDiff(true);
  tx.setTxDigitalGain(2); // TX digital gain => 2 = 2dB (default is 0 dB)
  tx.setTxFrequencyDerivation(200); // Valid valued from 0 to 255
  
  // analogWrite(PWM_PIN, 35);

  Send_LCD();
}

void loop() {

  if (digitalRead(UP_BUTTON_PIN) == LOW) {
    FREQ++; // Increment in steps of 1
    FREQ = constrain(FREQ, 880, 1080);
    delay(100); // Debounce
    Send_LCD();
  }

  if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
    FREQ--; // Decrement in steps of 1
    FREQ = constrain(FREQ, 880, 1080);
    delay(100); // Debounce
    Send_LCD();
  }
}

void Send_LCD() {

  analogWrite(PWM_PIN, 0);
  delay(100);
  tx.setTX(FREQ);
  // analogWrite(PWM_PIN, 35);

  float FREQ_Float = FREQ / 10.0;
  lcd.setCursor(0, 0);
  lcd.print("TUNE: ");
  lcd.print(FREQ_Float);
  lcd.print(" MHz ");
  lcd.setCursor(0, 1);
  lcd.print("HEIZER FM STEREO");
}
