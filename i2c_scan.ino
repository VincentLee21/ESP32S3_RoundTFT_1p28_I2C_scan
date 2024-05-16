
#include "Arduino.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Ticker.h>
#include "DEV_Debug.h"
#include "DEV_Config.h"

#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include "rom/rtc.h"
#endif

// #define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */


TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200);
  Info("\n\n\nESP32-S3 RoundTFT 1.28\n");

  if( psramInit()) {
    Info("PSRAM is correctly initialized\n");
  } else {
    Error("PSRAM not available\n");
  }

  Info( "Init LCD\n");
  // pinMode(2, OUTPUT);
  // digitalWrite(2, HIGH);
  // PWM for backlight
  ledcSetup(0, 500, 8);
  ledcAttachPin(2, 0);
  ledcWrite(0, 255);

  if (DEV_Module_Init() != 0)
    Error("GPIO Init Fail!\n");
  else
    Info("GPIO Init successful!\n");

  // Setup LCD
  tft.init();
  tft.setRotation(0);
  img.createSprite(240, 240);
  img.setTextColor(TFT_BLACK, TFT_WHITE, true);
  img.fillScreen(TFT_WHITE);

  Info( "Init LCD done\n");


  img.fillRect( 0, 0, 100, 100, TFT_GREEN);
  // img.drawString("calibration run", 60, 60, 2);

  img.pushSprite(0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  int nDevices = 0;

  usleep(50000); // 50ms
  Info("Scanning for I2C0 devices ...\n");
  for(address = 0x01; address < 0x7f; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Info("I2C0 device found at address 0x%02X\n", address);
      nDevices++;
    } else if(error != 2){
      Error("%d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    Warn("No I2C0 devices found");
  }

  Info("Scanning for I2C1 devices ...\n");
  for(address = 0x01; address < 0x7f; address++){
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();
    if (error == 0){
      Info("I2C1 device found at address 0x%02X\n", address);
      nDevices++;
    } else if(error != 2){
      Error("%d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    Warn("No I2C1 devices found");
  }

  Serial.printf("\n\n");
}