//#include "FS.h"
#include <SPI.h>

#define SD_CS 33

void setup() {
    // Start serial communication for debugging purposes
  Serial.begin(9600);
  Serial.println(MOSI);
Serial.println(MISO);
Serial.println(SCK);
Serial.println(SS);
//  SPI.begin(5, 19, 18);


//   SD.begin(SD_CS); 
//  if(!SD.begin(SD_CS)) {
//    Serial.println("Card Mount Failed");
//    return;
//  }
    while(! SD.begin(SD_CS)){
      Serial.println("Card Mount Failed");
      delay(30);
    }
    Serial.println("SD OK");

  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return; // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Reading ID, Date, Hour, Temperature \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

 appendFile(SD, "/data.txt", "Hello, this is my first sd card log");

  // Start deep sleep
  Serial.println("DONE! Going to sleep now.");
  esp_deep_sleep_start(); 
}

void loop() {
  // put your main code here, to run repeatedly:

}


// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
