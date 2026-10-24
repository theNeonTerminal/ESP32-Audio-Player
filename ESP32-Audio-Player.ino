#include "AudioTools.h"
#include "AudioTools/Disk/AudioSourceSD.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SD_CS        5
#define SD_MOSI    23
#define SD_MISO    19
#define SD_SCK     18 // SD Card Pins

#define I2S_DOUT     4
#define I2S_BCLK     2
#define I2S_LRC     15 // Max98357A Pins

const int pauseB = 34, nextB = 36, prevB = 39;

// ========== AUDIO PLAYER ==========
const char *startFilePath = "/";
const char *ext = "mp3";

AudioSourceSD source(startFilePath, ext, SD_CS);
I2SStream i2s;
MP3DecoderHelix decoder;
AudioPlayer player(source, i2s, decoder);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void printMetaData(MetaDataType type, const char* str, int len) {
  display.print("Currently Playing:");
  display.print(toStr(type));
  display.print(": ");
  display.println(str);
}

void printCurrentSong(){}

bool detectButtonPress(uint8_t pin) { // Simple function to detect keystrokes
    static unsigned long lastTrigger[40] = {0};
    static bool wasPressed[40] = {false};

    const unsigned long repeatInterval = 300;

    if (pin >= 40) return false;

    bool pressed = (digitalRead(pin) == LOW);

    if (!pressed) {
        wasPressed[pin] = false;
        return false;
    }

    // First press
    if (!wasPressed[pin]) {
        wasPressed[pin] = true;
        lastTrigger[pin] = millis();
        return true;
    }

    // Repeated triggers while held
    if (millis() - lastTrigger[pin] >= repeatInterval) {
        lastTrigger[pin] = millis();
        return true;
    }

    return false;
}
void setup() {
  Serial.begin(115200);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // Initialize SD SPI bus before player.begin()
  pinMode(pauseB, INPUT);
  pinMode(nextB, INPUT);
  pinMode(prevB, INPUT);
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Initialize I2S output
  auto cfg = i2s.defaultConfig(TX_MODE);

  cfg.pin_bck = I2S_BCLK;
  cfg.pin_ws = I2S_LRC;
  cfg.pin_data = I2S_DOUT;

  i2s.begin(cfg);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Initialize player
  player.setMetadataCallback(printMetaData);
  player.begin();
  player.setActive(0);

  player.setVolume(0.7);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void loop() {
  player.copy();
  display.setCursor(0,0);

  if (detectButtonPress(pauseB)) { player.setActive(!player.isActive()); }
  display.println(player.isActive() ? "Playing" : "Inactive"); // print current audio state
  display.println(player.audioSource().toStr()); // print current audio path on SD card
  
  if (detectButtonPress(nextB)) { player.next(); }
  if (detectButtonPress(prevB)) { player.previous(); }

  display.display();
  display.clearDisplay();
}

//NOTE: This code works to get all the mp3 files names in an SD card - not impleted as of now, but will be very soon
//
// SH1106 display;
// #include <SD.h>
//
// const int chipSelect = 5;
// File root;
//
// void setup() {
//   Serial.begin(115200);
//   display.begin();
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setCursor(0,0);
//   display.setTextColor(WHITE);
//
//   Serial.print("Initializing SD card...");
//
//   if (!SD.begin(chipSelect)) {
//     display.println("initialization failed");
//     display.display();
//     while (true);
//   }
//   display.clearDisplay();
//
//   Serial.println("initialization done.");
//
//   root = SD.open("/");
//   printWavFiles(root, 0);
//
//   display.println("done!");
//   display.display();
// }
//
// void loop() {
//   // nothing happens after setup finishes.
// }
//
// void printmp3Files(File dir, int numTabs) {
//   while (true) {
//
//     File entry =  dir.openNextFile();
//     if (! entry) { break; } // No more files
//     if (!entry.isDirectory()) {
//       String filename = String(entry.name());
//       if (filename.endsWith(".mp3")){ display.println(filename); }
//     } else {
//       display.println("/");
//       printmp3Files(entry, numTabs + 1);
//     }
//     entry.close();
//
//   }
// }
