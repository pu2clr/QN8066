/*
  
  UNDER CONSTRUCTION...
  
  This sketch runs on ESP32 device LilyGO T-Embed panel.
  


  ESP32 and components wire up.

  | Device name            | Device Pin / Description     |  ESP32    |
  | -----------------------| -----------------------------| ----------|
  | LilyGo Painel I2C      |                              |           |
  |                        | SDA/SDIO                     |  GPIO 18  |
  |                        | SCL/SCLK                     |  GPIO  8  |
  | Encoder  LilyGo Painel |                              |           |
  |                        | A                            |  GPIO  2  |
  |                        | B                            |  GPIO  1  |
  |                        | PUSH BUTTON (encoder)        |  GPIO  0  |
  |                        |                              |           |  
  | Battery Indicator      |                              |  GPIO  4  |
  | LCD LED                |                              |  GPIO 15  |  
  | Power ON               |                              |  GPIO 46  |
  |                        |                              |           |
  | PWM POWER CONTROL PIN  |                              |           |
  | DIY FM TRANSMITTER     |                              |  GPIO 16  |  




  If you are using the SI4732-A10, check the corresponding pin numbers.

  References: 
  About LilyGO T-Embed and SI473X: https://github.com/pu2clr/QN8066/tree/master/examples/SI47XX_KITS/Lilygo_t_embed
  About PU2CLR QN8066 Arduino Library: https://pu2clr.github.io/QN8066/
  PU2CLR Si47XX API documentation: https://pu2clr.github.io/QN8066/extras/apidoc/html/

  By PU2CLR, Ricardo, Dec  2022.
*/

#include <Wire.h>
#include "EEPROM.h"
#include <QN8066.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <Battery18650Stats.h>  // Install it from: https://github.com/danilopinotti/Battery18650Stats

#include "Rotary.h"
#include <patch_init.h>  // SSB patch for whole SSBRX initialization string

const uint16_t size_content = sizeof ssb_patch_content;  // see patch_init.h

#define FM_BAND_TYPE 0
#define MW_BAND_TYPE 1
#define SW_BAND_TYPE 2
#define LW_BAND_TYPE 3

#define RESET_PIN 16  // GPIO16

// Enconder PINs
#define ENCODER_PIN_A 1  // GPIO2
#define ENCODER_PIN_B 2  // GPIO1

// I2C bus pin on ESP32
#define ESP32_I2C_SDA 18
#define ESP32_I2C_SCL 8

#define PIN_BAT_VOLT 4
#define PIN_POWER_ON 46
#define PIN_LCD_BL 15

// Buttons controllers
#define ENCODER_PUSH_BUTTON 0  // GPIO0

#define MIN_ELAPSED_TIME 300
#define MIN_ELAPSED_RSSI_TIME 200
#define ELAPSED_COMMAND 2000  // time to turn off the last command controlled by encoder. Time to goes back to the FVO control
#define ELAPSED_CLICK 1500    // time to check the double click commands
#define DEFAULT_VOLUME 60     // change it for your favorite sound volume
#define ELAPSED_TIME_BATERRY 60000

#define EEPROM_SIZE 512

#define STORE_TIME 10000  // Time of inactivity to make the current receiver status writable (10s / 10000 milliseconds).

#define NUM_LEDS 7
#define DATA_PIN 42
#define CLOCK_PIN 45

#define color1 0xC638
#define color2 0xC638

Battery18650Stats battery(PIN_BAT_VOLT);

// EEPROM - Stroring control variables
const uint8_t app_id = 47;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;


CRGB leds[NUM_LEDS];

// Devices class declarations
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

QN8066 tx;

void setup() {
  // Encoder pins
  pinMode(ENCODER_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  // The line below may be necessary to setup I2C pins on ESP32
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);

  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

  tft.begin();
  tft.writecommand(0x11);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  turnDisplay(true);

  spr.createSprite(320, 170);
  spr.setTextDatum(4);
  spr.setSwapBytes(true);
  spr.setFreeFont(&Orbitron_Light_24);
  spr.setTextColor(color1, TFT_BLACK);

  // Splash - Remove or change it for your own introduction.
  splash();

  EEPROM.begin(EEPROM_SIZE);

  // If you want to reset the eeprom, keep the VOLUME_UP button pressed during statup
  if (digitalRead(ENCODER_PUSH_BUTTON) == LOW) {
    EEPROM.write(eeprom_address, 0);
    EEPROM.commit();
    print(20, 20, &Orbitron_Light_24, "EEPROM RESETED");
    delay(3000);
  }

  // ICACHE_RAM_ATTR void rotaryEncoder(); see rotaryEncoder implementation below.
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);



  leds[0] = CRGB::Green;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;
  leds[3] = CRGB::Green;
  leds[4] = CRGB::Green;
  leds[5] = CRGB::Green;
  leds[6] = CRGB::Green;
  FastLED.show();

}

// Turn the device and display on (true) or off (false)
void turnDisplay(bool v) {
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, v);
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, v);
}

/**
 * Shows the first screen with some message.
 * Remove all code of this function if you do not wish it.
 */
void splash() {
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.setFreeFont(&Orbitron_Light_24);
  spr.drawString(" PU2CLR QN8066", 140, 12);
  spr.drawString("Arduino Library", 140, 60);
  spr.pushSprite(0, 0);
  delay(700);
}

/**
 * Prints a given content on display 
 */
void print(uint8_t col, uint8_t lin, const GFXfont *font, const char *msg) {
  spr.setFreeFont(font);
  spr.drawString(msg, col, lin);
  spr.pushSprite(0, 0);
}

void printParam(const char *msg) {
  spr.fillRect(10, 120, 220, 40, TFT_BLACK);
  print(80, 130, &Orbitron_Light_32, msg);
}

/*
   writes the conrrent receiver information into the eeprom.
   The EEPROM.update avoid write the same data in the same memory position. It will save unnecessary recording.
*/
void saveAllReceiverInformation() {

}

/**
 * reads the last receiver status from eeprom. 
 */
void readAllReceiverInformation() {

}


/**
 * Reads encoder via interrupt
 * Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
 * if you do not add ICACHE_RAM_ATTR declaration, the system will reboot during attachInterrupt call. 
 * With ICACHE_RAM_ATTR macro you put the function on the RAM.
 */
ICACHE_RAM_ATTR void rotaryEncoder() {  // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
}

/**
 * Shows frequency information on Display
 */
void showFrequency() {
  float freq, value;
  int decimals;

  // defines the frequency display format depending on the used band
  if (rx.isCurrentTuneFM()) {
    freq = currentFrequency / 100.0;
    decimals = 1;
  } else {
    if (currentFrequency < 1000) {  // LW or MW
      freq = (float)currentFrequency;
      decimals = 0;
    } else {
      freq = (float)currentFrequency / 1000.0;
      decimals = 3;
    }
  }

  value = freq * 10;

  spr.fillSprite(TFT_BLACK);                  // clear screen
  spr.drawFloat(freq, decimals, 150, 64, 7);  // shows the frequency

  // Builds the frequency ruler
  spr.fillTriangle(156, 104, 160, 114, 164, 104, TFT_RED);
  spr.drawLine(160, 114, 160, 170, TFT_RED);

  int temp = value - 20;
  for (int i = 0; i < 40; i++) {
    if ((temp % 10) == 0) {
      spr.drawLine(i * 8, 170, i * 8, 140, color1);
      spr.drawLine((i * 8) + 1, 170, (i * 8) + 1, 140, color1);
      spr.drawFloat(temp / 10.0, 1, i * 8, 130, 2);
    } else if ((temp % 5) == 0 && (temp % 10) != 0) {
      spr.drawLine(i * 8, 170, i * 8, 150, color1);
      spr.drawLine((i * 8) + 1, 170, (i * 8) + 1, 150, color1);
    } else {
      spr.drawLine(i * 8, 170, i * 8, 160, color1);
    }
    temp = temp + 1;
  }

  showBattery();

  spr.pushSprite(0, 0);

  showAudioPeak();
  showMode();
}

/**
 * Shows the current mode
 */
void showMode() {
  char *bandMode;
  if (currentFrequency < 520)
    bandMode = (char *)"LW  ";
  else
    bandMode = (char *)bandModeDesc[currentMode];

  spr.setTextColor(TFT_YELLOW, TFT_BLACK);
  spr.setFreeFont(&FreeMono9pt7b);
  spr.drawString(bandMode, 25, 20);
  spr.drawString(band[bandIdx].bandName, 25, 35);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.pushSprite(0, 0);
}

void showBattery() {

  uint16_t colorBattery;
  int value = map(battery.getBatteryChargeLevel(true), 0, 100, 0, 18);
  // int value = map(battery.getBatteryVolts() * 10,32,42,0,18);

  if (value < 5)
    colorBattery = TFT_RED;
  else if (value < 11)
    colorBattery = TFT_YELLOW;
  else
    colorBattery = TFT_GREEN;

  spr.drawRect(290, 6, 20, 9, TFT_WHITE);
  spr.fillRect(291, 7, 18, 7, TFT_BLACK);
  spr.fillRect(291, 7, value, 7, colorBattery);
  spr.fillRect(310, 8, 2, 5, TFT_WHITE);
  spr.pushSprite(0, 0);
}

/**
 * Shows some basic information on display
 */
void showStatus() {
  showFrequency();
  showAudioPeak();
}

/**
 *  Shows the current Bandwidth status
 */
void showBandwidth() {
  char *bw;
  char bandwidth[20];
  if (currentMode == LSB || currentMode == USB) {
    bw = (char *)bandwidthSSB[bwIdxSSB].desc;
    showBFO();
  } else if (currentMode == AM) {
    bw = (char *)bandwidthAM[bwIdxAM].desc;
  } else {
    bw = (char *)bandwidthFM[bwIdxFM].desc;
  }
  sprintf(bandwidth, "%s", bw);
  printParam(bandwidth);
}

/*
 * Concert rssi to VU
*/
int getAudioPeak(uint8_t audioPeak) {
  if ((audioPeak >= 0) and (audioPeak <= 1))
    return 1;  // S0
  if ((audioPeak > 1) and (audioPeak <= 1))
    return 2;  // S1
  if ((audioPeak > 2) and (audioPeak <= 3))
    return 3;  // S2
  if ((audioPeak > 3) and (audioPeak <= 4))
    return 4;  // S3
  if ((audioPeak > 4) and (audioPeak <= 10))
    return 5;  // S4
  if ((audioPeak > 10) and (audioPeak <= 16))
    return 6;  // S5
  if ((audioPeak > 16) and (audioPeak <= 22))
    return 7;  // S6
  if ((audioPeak > 22) and (audioPeak <= 28))
    return 8;  // S7
  if ((audioPeak > 28) and (audioPeak <= 34))
    return 9;  // S8
  if ((audioPeak > 34) and (audioPeak <= 44))
    return 10;  // S9
  if ((audioPeak > 44) and (audioPeak <= 54))
    return 11;  // S9 +10
  if ((audioPeak > 54) and (audioPeak <= 64))
    return 12;  // S9 +20
  if ((audioPeak > 64) and (audioPeak <= 74))
    return 13;  // S9 +30
  if ((audioPeak > 74) and (audioPeak <= 84))
    return 14;  // S9 +40
  if ((audioPeak > 84) and (audioPeak <= 94))
    return 15;  // S9 +50
  if (audioPeak > 94)
    return 16;  // S9 +60
  if (audioPeak > 95)
    return 17;  //>S9 +60

  return 0;
}

/**
 *   Shows the current RSSI and SNR status
 */
void showAudioPeak() {
  spr.fillRect(240, 20, 76, 88, TFT_BLACK);  // Clear the indicator areas

  for (int i = 0; i < getAudioPeak(rssi); i++) {
    if (i < 9)
      spr.fillRect(244 + (i * 4), 80 - i, 2, 4 + i, TFT_GREEN);
    else
      spr.fillRect(244 + (i * 4), 80 - i, 2, 4 + i, TFT_RED);
  }

  if (currentMode == FM) {
    spr.drawRoundRect(240, 20, 76, 22, 4, TFT_WHITE);
    if (rx.getCurrentPilot())
      spr.drawString("Stereo", 275, 31, 2);
    else
      spr.drawString("Mono", 275, 31, 2);
  }
  spr.pushSprite(0, 0);
}

/**
 *    Shows the current AGC and Attenuation status
 */
void showAgcAtt() {
  char sAgc[15];
  // lcd.clear();
  rx.getAutomaticGainControl();
  if (agcNdx == 0 && agcIdx == 0)
    strcpy(sAgc, "AGC ON");
  else
    sprintf(sAgc, "ATT: %2.2d", agcNdx);

  printParam(sAgc);
}

/**
   Shows current Automatic Volume Control
*/
void showAvc() {
  char sAvc[10];
  if (currentMode != FM) {
    sprintf(sAvc, "%2.2d", avcIdx);
    printParam(sAvc);
  }
}

/**
 *   Shows the current step
 */
void showStep() {
  char sStep[15];
  sprintf(sStep, "%4d", (currentMode == FM) ? (tabFmStep[currentStepIdx] * 10) : tabAmStep[currentStepIdx]);
  printParam(sStep);
}

/**
 *  Shows the current BFO value
 */
void showBFO() {
  char bfo[18];

  if (currentBFO > 0)
    sprintf(bfo, "+%4.4d", currentBFO);
  else
    sprintf(bfo, " %4.4d", currentBFO);

  printParam(bfo);
  elapsedCommand = millis();
}

/*
 *  Shows the volume level on LCD
 */
void showVolume() {
  char volAux[12];
  sprintf(volAux, "%2u", rx.getVolume());
  printParam(volAux);
}

/**
 * Show Soft Mute 
 */
void showSoftMute() {
  char sMute[18];
  sprintf(sMute, "%2d", softMuteMaxAttIdx);
  printParam(sMute);
}

/**
 *   Sets Band up (1) or down (!1)
 */
void setBand(int8_t up_down) {
  band[bandIdx].currentFreq = currentFrequency;
  band[bandIdx].currentStepIdx = currentStepIdx;
  if (up_down == 1)
    bandIdx = (bandIdx < lastBand) ? (bandIdx + 1) : 0;
  else
    bandIdx = (bandIdx > 0) ? (bandIdx - 1) : lastBand;
  useBand();
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  elapsedCommand = millis();
}

/**
 * Switch the radio to current band
 */
void useBand() {
  if (band[bandIdx].bandType == FM_BAND_TYPE) {
    currentMode = FM;
    rx.setTuneFrequencyAntennaCapacitor(0);
    rx.setFM(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, tabFmStep[band[bandIdx].currentStepIdx]);
    rx.setSeekFmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);
    bfoOn = ssbLoaded = false;
    bwIdxFM = band[bandIdx].bandwidthIdx;
    rx.setFmBandwidth(bandwidthFM[bwIdxFM].idx);
    rx.setRdsConfig(3, 3, 3, 3, 3);
    rx.setFifoCount(1);
  } else {
    // set the tuning capacitor for SW or MW/LW
    rx.setTuneFrequencyAntennaCapacitor((band[bandIdx].bandType == MW_BAND_TYPE || band[bandIdx].bandType == LW_BAND_TYPE) ? 0 : 1);
    if (ssbLoaded) {
      rx.setSSB(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, tabAmStep[band[bandIdx].currentStepIdx], currentMode);
      rx.setSSBAutomaticVolumeControl(1);
      rx.setSsbSoftMuteMaxAttenuation(softMuteMaxAttIdx);  // Disable Soft Mute for SSB
      bwIdxSSB = band[bandIdx].bandwidthIdx;
      rx.setSSBAudioBandwidth(bandwidthSSB[bwIdxSSB].idx);
    } else {
      currentMode = AM;
      rx.setAM(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq, band[bandIdx].currentFreq, tabAmStep[band[bandIdx].currentStepIdx]);
      bfoOn = false;
      bwIdxAM = band[bandIdx].bandwidthIdx;
      rx.setBandwidth(bandwidthAM[bwIdxAM].idx, 1);
      rx.setAmSoftMuteMaxAttenuation(softMuteMaxAttIdx);  // Soft Mute for AM or SSB
    }
    rx.setAutomaticGainControl((disableAgc = (agcIdx > 0)), agcNdx);
    rx.setSeekAmLimits(band[bandIdx].minimumFreq, band[bandIdx].maximumFreq);  // Consider the range all defined current band
    rx.setSeekAmSpacing(5);                                                    // Max 10kHz for spacing
    rx.setAvcAmMaxGain(avcIdx);
  }
  delay(100);
  currentFrequency = band[bandIdx].currentFreq;
  currentStepIdx = band[bandIdx].currentStepIdx;
  rssi = 0;
  showStatus();
  showCommandStatus((char *)"Band");
}

/**
  RDS Begin
 */


char *programInfo;
char *stationName;
char *utcTime;
char *dummy;

long polling_rds = millis();

int progInfoIndex = 0;  // controls the part of the rdsMsg text will be shown on LCD 16x2 Display

long delayProgramInfo = millis();
long delayUtcTime = millis();
long delayStationName = millis();


/**
  showProgramInfo - Shows the Program Information
*/
void showProgramInfo() {
  char txtAux[14];

  if (programInfo == NULL || strlen(programInfo) < 2 || (millis() - delayProgramInfo) < 1000L) return;
  delayProgramInfo = millis();
  programInfo[61] = '\0';  // Truncate the message to fit on display line
  strncpy(txtAux, &programInfo[progInfoIndex], 14);
  txtAux[13] = '\0';
  progInfoIndex += 3;
  if (progInfoIndex > 60) progInfoIndex = 0;

  spr.fillRect(0, 100, 152, 20, TFT_BLACK);
  spr.setTextColor(TFT_YELLOW, TFT_BLACK);
  spr.setFreeFont(&FreeMono9pt7b);
  spr.drawString(txtAux, 77, 111);
  spr.pushSprite(0, 0);
}

void showStationName() {
  if (stationName == NULL || (millis() - delayStationName) < 3000) return;
  delayStationName = millis();
}

void showTime() {
  if (utcTime == NULL || strlen(utcTime) < 4 || (millis() - delayUtcTime) < 50000) return;

  spr.fillRect(240, 100, 138, 20, TFT_BLACK);
  spr.setTextColor(TFT_YELLOW, TFT_BLACK);
  spr.setFreeFont(&FreeMono9pt7b);
  spr.drawString(utcTime, 241, 111);
  spr.pushSprite(0, 0);
  delayProgramInfo = millis();  // Stop showing Program Information for 10s
  delayUtcTime = millis();
}


void clearRds() {
  utcTime = programInfo = stationName = NULL;
  rx.clearRdsBuffer();
  progInfoIndex = 0;
}


void checkRDS() {

  if (rx.getRdsAllData(&stationName, &dummy, &programInfo, &utcTime)) {
    showTime();
    showProgramInfo();
    showStationName();
  }
}

/**
  RDS END
*/




void loadSSB() {
  rx.setI2CFastModeCustom(400000);  // You can try rx.setI2CFastModeCustom(700000); or greater value
  rx.loadPatch(ssb_patch_content, size_content, bandwidthSSB[bwIdxSSB].idx);
  rx.setI2CFastModeCustom(100000);
  ssbLoaded = true;
}

/**
 *  Switches the Bandwidth
 */
void doBandwidth(int8_t v) {
  if (currentMode == LSB || currentMode == USB) {
    bwIdxSSB = (v == 1) ? bwIdxSSB + 1 : bwIdxSSB - 1;

    if (bwIdxSSB > maxSsbBw)
      bwIdxSSB = 0;
    else if (bwIdxSSB < 0)
      bwIdxSSB = maxSsbBw;

    rx.setSSBAudioBandwidth(bandwidthSSB[bwIdxSSB].idx);
    // If audio bandwidth selected is about 2 kHz or below, it is recommended to set Sideband Cutoff Filter to 0.
    if (bandwidthSSB[bwIdxSSB].idx == 0 || bandwidthSSB[bwIdxSSB].idx == 4 || bandwidthSSB[bwIdxSSB].idx == 5)
      rx.setSSBSidebandCutoffFilter(0);
    else
      rx.setSSBSidebandCutoffFilter(1);

    band[bandIdx].bandwidthIdx = bwIdxSSB;
  } else if (currentMode == AM) {
    bwIdxAM = (v == 1) ? bwIdxAM + 1 : bwIdxAM - 1;

    if (bwIdxAM > maxAmBw)
      bwIdxAM = 0;
    else if (bwIdxAM < 0)
      bwIdxAM = maxAmBw;

    rx.setBandwidth(bandwidthAM[bwIdxAM].idx, 1);
    band[bandIdx].bandwidthIdx = bwIdxAM;

  } else {
    bwIdxFM = (v == 1) ? bwIdxFM + 1 : bwIdxFM - 1;
    if (bwIdxFM > maxFmBw)
      bwIdxFM = 0;
    else if (bwIdxFM < 0)
      bwIdxFM = maxFmBw;

    rx.setFmBandwidth(bandwidthFM[bwIdxFM].idx);
    band[bandIdx].bandwidthIdx = bwIdxFM;
  }
  showBandwidth();
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
}

/**
 * Show cmd on display. It means you are setting up something.  
 */
void showCommandStatus(char *currentCmd) {
  spr.setFreeFont(&Orbitron_Light_24);
  spr.fillRect(110, 0, 90, 25, TFT_BLACK);
  spr.drawString(currentCmd, 160, 12);
  spr.pushSprite(0, 0);
}

/**
 * Show menu options
 */
void showMenu() {
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.setFreeFont(&Orbitron_Light_32);
  spr.drawString(menu[menuIdx], 100, 50);
  showCommandStatus((char *)"Menu");
  spr.pushSprite(0, 0);
}

/**
 *  AGC and attenuattion setup
 */
void doAgc(int8_t v) {
  agcIdx = (v == 1) ? agcIdx + 1 : agcIdx - 1;
  if (agcIdx < 0)
    agcIdx = 35;
  else if (agcIdx > 35)
    agcIdx = 0;
  disableAgc = (agcIdx > 0);  // if true, disable AGC; esle, AGC is enable
  if (agcIdx > 1)
    agcNdx = agcIdx - 1;
  else
    agcNdx = 0;
  rx.setAutomaticGainControl(disableAgc, agcNdx);  // if agcNdx = 0, no attenuation
  showAgcAtt();
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  elapsedCommand = millis();
}

/**
   sets the Automatic Volume Control
*/
void doAvc(int8_t v) {
  if (currentMode != FM) {
    avcIdx = (v == 1) ? avcIdx + 2 : avcIdx - 2;
    if (avcIdx > 90)
      avcIdx = 12;
    else if (avcIdx < 12)
      avcIdx = 90;

    rx.setAvcAmMaxGain(avcIdx);
    showAvc();
    delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  }
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  elapsedCommand = millis();
}

/**
 * Switches the current step
 */
void doStep(int8_t v) {
  if (currentMode == FM) {
    idxFmStep = (v == 1) ? idxFmStep + 1 : idxFmStep - 1;
    if (idxFmStep > lastFmStep)
      idxFmStep = 0;
    else if (idxFmStep < 0)
      idxFmStep = lastFmStep;

    currentStepIdx = idxFmStep;
    rx.setFrequencyStep(tabFmStep[currentStepIdx]);

  } else {
    idxAmStep = (v == 1) ? idxAmStep + 1 : idxAmStep - 1;
    if (idxAmStep > lastAmStep)
      idxAmStep = 0;
    else if (idxAmStep < 0)
      idxAmStep = lastAmStep;

    currentStepIdx = idxAmStep;
    rx.setFrequencyStep(tabAmStep[currentStepIdx]);
    rx.setSeekAmSpacing(5);  // Max 10kHz for spacing
  }
  band[bandIdx].currentStepIdx = currentStepIdx;
  showStep();
  elapsedCommand = millis();
}

/**
 * Switches to the AM, LSB or USB modes
 */
void doMode(int8_t v) {
  if (currentMode != FM) {
    if (v == 1) {  // clockwise
      if (currentMode == AM) {
        // If you were in AM mode, it is necessary to load SSB patch (avery time)
        loadSSB();
        ssbLoaded = true;
        currentMode = LSB;
      } else if (currentMode == LSB)
        currentMode = USB;
      else if (currentMode == USB) {
        currentMode = AM;
        bfoOn = ssbLoaded = false;
      }
    } else {  // and counterclockwise
      if (currentMode == AM) {
        // If you were in AM mode, it is necessary to load SSB patch (avery time)
        loadSSB();
        ssbLoaded = true;
        currentMode = USB;
      } else if (currentMode == USB)
        currentMode = LSB;
      else if (currentMode == LSB) {
        currentMode = AM;
        bfoOn = ssbLoaded = false;
      }
    }
    // Nothing to do if you are in FM mode
    band[bandIdx].currentFreq = currentFrequency;
    band[bandIdx].currentStepIdx = currentStepIdx;
    useBand();
  }
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  elapsedCommand = millis();
}

/**
 * Sets the audio volume
 */
void doVolume(int8_t v) {
  if (v == 1)
    rx.volumeUp();
  else
    rx.volumeDown();

  showVolume();
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
}

/**
 *  This function is called by the seek function process.
 */
void showFrequencySeek(uint16_t freq) {
  currentFrequency = freq;
  showFrequency();
}

/**
 *  Find a station. The direction is based on the last encoder move clockwise or counterclockwise
 */
void doSeek() {
  if ((currentMode == LSB || currentMode == USB)) return;  // It does not work for SSB mode

  rx.seekStationProgress(showFrequencySeek, seekDirection);
  currentFrequency = rx.getFrequency();
}

/**
 * Sets the Soft Mute Parameter
 */
void doSoftMute(int8_t v) {
  softMuteMaxAttIdx = (v == 1) ? softMuteMaxAttIdx + 1 : softMuteMaxAttIdx - 1;
  if (softMuteMaxAttIdx > 32)
    softMuteMaxAttIdx = 0;
  else if (softMuteMaxAttIdx < 0)
    softMuteMaxAttIdx = 32;

  rx.setAmSoftMuteMaxAttenuation(softMuteMaxAttIdx);
  showSoftMute();
  elapsedCommand = millis();
}

/**
 *  Menu options selection
 */
void doMenu(int8_t v) {
  menuIdx = (v == 1) ? menuIdx + 1 : menuIdx - 1;

  if (menuIdx > lastMenu)
    menuIdx = 0;
  else if (menuIdx < 0)
    menuIdx = lastMenu;

  showMenu();
  delay(MIN_ELAPSED_TIME);  // waits a little more for releasing the button.
  elapsedCommand = millis();
}

/**
 * Starts the MENU action process
 */
void doCurrentMenuCmd() {
  disableCommands();
  switch (currentMenuCmd) {
    case 0:  // VOLUME
      cmdVolume = true;
      showVolume();
      break;
    case 1:  // STEP
      cmdStep = true;
      showStep();
      break;
    case 2:  // MODE
      cmdMode = true;
      showMode();
      break;
    case 3:
      bfoOn = true;
      if ((currentMode == LSB || currentMode == USB)) {
        showBFO();
      }
      // showFrequency();
      break;
    case 4:  // BW
      cmdBandwidth = true;
      showBandwidth();
      break;
    case 5:  // AGC/ATT
      cmdAgc = true;
      showAgcAtt();
      break;
    case 6:
      cmdAvc = true;
      showAvc();
      break;
    case 7:
      cmdSoftMuteMaxAtt = true;
      showSoftMute();
      break;
    case 8:
      doSeek();
      break;
    default:
      showStatus();
      break;
  }
  currentMenuCmd = -1;
  elapsedCommand = millis();
}

/**
 * Return true if the current status is Menu command
 */
bool isMenuMode() {
  return (cmdMenu | cmdStep | cmdBandwidth | cmdAgc | cmdAvc | cmdVolume | cmdSoftMuteMaxAtt | cmdMode);
}

/**
 * Main loop
 */
void loop() {
  // Check if the encoder has moved.
  if (encoderCount != 0) {
    if (bfoOn & (currentMode == LSB || currentMode == USB)) {
      currentBFO = (encoderCount == 1) ? (currentBFO + currentBFOStep) : (currentBFO - currentBFOStep);
      rx.setSSBBfo(currentBFO);
      showBFO();
    } else if (cmdMenu)
      doMenu(encoderCount);
    else if (cmdMode)
      doMode(encoderCount);
    else if (cmdStep)
      doStep(encoderCount);
    else if (cmdAgc)
      doAgc(encoderCount);
    else if (cmdAvc)
      doAvc(encoderCount);
    else if (cmdBandwidth)
      doBandwidth(encoderCount);
    else if (cmdVolume)
      doVolume(encoderCount);
    else if (cmdSoftMuteMaxAtt)
      doSoftMute(encoderCount);
    else if (cmdBand)
      setBand(encoderCount);
    else {
      if (encoderCount == 1) {
        seekDirection = 1;
        rx.frequencyUp();
      } else {
        seekDirection = 0;
        rx.frequencyDown();
      }
      // Show the current frequency only if it has changed
      currentFrequency = rx.getFrequency();
      showFrequency();
    }
    encoderCount = 0;
    resetEepromDelay();
  } else {
    if (digitalRead(ENCODER_PUSH_BUTTON) == LOW) {
      countClick++;
      if (cmdMenu) {
        currentMenuCmd = menuIdx;
        doCurrentMenuCmd();
      } else if (countClick == 1) {  // If just one click, you can select the band by rotating the encoder
        if (isMenuMode()) {
          disableCommands();
          showStatus();
          showCommandStatus((char *)"VFO ");
        } else if (bfoOn) {
          bfoOn = false;
          showStatus();
        } else {
          cmdBand = !cmdBand;
          showCommandStatus((char *)"Band");
        }
      } else {  // GO to MENU if more than one click in less than 1/2 seconds.
        cmdMenu = !cmdMenu;
        if (cmdMenu)
          showMenu();
      }
      delay(MIN_ELAPSED_TIME);
      elapsedCommand = millis();
    }
  }

  // Show RSSI status only if this condition has changed
  if ((millis() - elapsedRSSI) > (MIN_ELAPSED_RSSI_TIME * 6)) {
    rx.getCurrentReceivedSignalQuality();
    int aux = rx.getCurrentRSSI();
    if (rssi != aux && !isMenuMode()) {
      rssi = aux;
      showAudioPeak();
    }
    elapsedRSSI = millis();
  }

  // Disable commands control
  if ((millis() - elapsedCommand) > ELAPSED_COMMAND) {
    if ((currentMode == LSB || currentMode == USB))
      bfoOn = false;
    elapsedCommand = millis();
  }

  if ((millis() - elapsedClick) > ELAPSED_CLICK) {
    countClick = 0;
    elapsedClick = millis();
  }

  // Show the current frequency only if it has changed
  if (itIsTimeToSave) {
    if ((millis() - storeTime) > STORE_TIME) {
      saveAllReceiverInformation();
      storeTime = millis();
      itIsTimeToSave = false;
    }
  }

  if ((millis() - elapsedBattery) > ELAPSED_TIME_BATERRY) {
    elapsedBattery = millis();
    showBattery();
  }


  if ((millis() - polling_rds) > POLLING_RDS && rx.isCurrentTuneFM())  {
    checkRDS();
    polling_rds = millis();
  }

}
