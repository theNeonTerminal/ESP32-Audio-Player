#include <Adafruit_GFX.h>
#include <SH1106.h>
#include <Wire.h>

SH1106 display;
//
// void setup(){
//   display.begin();
//   Serial.begin(115200);
// }
// void loop(){}

// =======================================

// #include "Arduino.h"
// #include "Audio.h"
// #include "SPI.h"
// #include "SD.h"
// #include "FS.h"
//
// #define SD_CS        5
// #define SPI_MOSI    23
// #define SPI_MISO    19
// #define SPI_SCK     18 // SD Card Pins
//
// #define I2S_DOUT     4
// #define I2S_BCLK     2
// #define I2S_LRC     15 // Max98357A Pins
//
// Audio audio;
//
// void my_audio_info(Audio::msg_t m) {
//     Serial.printf("%s: %s\n", m.s, m.msg);
// }
//
// void setup() {
//     Audio::audio_info_callback = my_audio_info;
//     pinMode(SD_CS, OUTPUT);
//     digitalWrite(SD_CS, HIGH);
//     SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
//     SPI.setFrequency(1000000);
//     Serial.begin(115200);
//     SD.begin(SD_CS);
//     audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
//     audio.setVolume(12); // 0...21
//     audio.connecttoFS(SD, "audio.wav");
// }
//
// void loop(){
//     vTaskDelay(1);
//     audio.loop();
// }
//
//
// ========================================
//


#include <SD.h>

const int chipSelect = 5;
File root;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    display.println("initialization failed");
    display.display();
    while (true);
  }
  display.clearDisplay();

  Serial.println("initialization done.");

  root = SD.open("/");
  printWavFiles(root, 0);

  display.println("done!");
  display.display();
}

void loop() {
  // nothing happens after setup finishes.
}

void printWavFiles(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) { break; } // No more files
    if (!entry.isDirectory()) {
      String filename = String(entry.name());
      if (filename.endsWith(".wav") || filename.endsWith(".mp3")){ display.println(filename); }
    } else {
      display.println("/");
      printWavFiles(entry, numTabs + 1);
    }
    entry.close();

  }
}
