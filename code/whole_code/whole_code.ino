#include "SD.h"
#include <SPI.h>
#include <Wire.h>

// Initialize FeatherWing display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif

// Define CS pin for the SD card module
#define SD_CS 33

byte fetch_pressure(unsigned int *p_Pressure); //convert value to byte data type
#define TRUE 1
#define FALSE 0

// pressure sensor varibales
byte pressure_sensor_status;
unsigned int pressure_data;
float PR;
String PR_data;

void setup() {
  Serial.begin(9600);

  // Setup display -- and show some messages
  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
//  display.display();
//  delay(1000);
  
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Initializing ...");
  display.display(); // actually display all of the above

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
    writeFile(SD, "/data.txt", "\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();


  // Sensor initializing
  Wire.begin();
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);  // SCL remains high
  digitalWrite(4, HIGH); // SDA transfers from high to low
  digitalWrite(4, LOW);  // this turns on the MS4525, I think
  delay(5000);
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>");  // just to be sure things are working 
}

void loop() {

  pressure_sensor_status = fetch_pressure(&pressure_data);
  
  switch(pressure_sensor_status)
  {
    case 0: Serial.println("Read_MR.");
    break;
    case 1: Serial.println("Read_DF2.");
    break;
    case 2: Serial.println("Read_DF3.");
    break;
    default: Serial.println("Read_DF4.");
    break;
  }
 
  PR = (float)  ((pressure_data - (.1*16383))*(1) / (.8*16383)) ;
 
  Serial.println(pressure_data);
  Serial.println(PR);
  Serial.print(" ");
  PR_data = String(pressure_data);
  appendFile(SD, "/data.txt", PR_data.c_str());

  display.setCursor(0,0);
  display.println(pressure_data);
  display.println(PR);
  yield();
  display.display();

  delay(1000);
//  delay(100);
  display.clearDisplay(); 
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
  file.print("\n");
  file.close();
}

// fetch pressure data
byte fetch_pressure(unsigned int *p_P_dat)
{     
  byte address, Press_H, Press_L, _status;
  unsigned int P_dat;
  address= 0x28;
  Wire.beginTransmission(address); 
  Wire.endTransmission();
  delay(100);
 
  Wire.requestFrom((int)address, (int) 4);
  Press_H = Wire.read();
  Press_L = Wire.read();
  Wire.endTransmission();
 
  _status = (Press_H >> 6) & 0x03;
      Press_H = Press_H & 0x3f;
      P_dat = (((unsigned int)Press_H) << 8) | Press_L;
      *p_P_dat = P_dat;
      return(_status);
}
