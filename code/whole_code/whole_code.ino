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

const int threshold = 100; // threashold around balance point to remove unwanted noise
const int balance_point = 8191; // balance point (16383/2) -- when there is no pressure difference
const int Pmin = -1; // psi
const int Pmax = 1; // psi

float volume = 0;
float dt;
long measureTime = 1, lastMeasureTime;

// Volume calculation constants
const float rho = 1.225; // density of air [kg/m^3]
const float area1 = 3.1415e-6; // m^2
const float area2 = 78.225e-6; //m^2
const float C = 2 * rho / (1 / (area1*area1) - 1 / (area2*area2)); // calculate the constants before hand

// button stuff
int bounceTime = 50;
int holdTime = 2000;

int lastReading = LOW;
bool hold = false;
bool remove_old_data = false;

long onTime = 0;
//long lastSwitchTime = 0;

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

  // Initialize buttons on feather wing
  pinMode(BUTTON_A, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing ...");
  display.display(); // actually display all of the above

  while (! SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    display.setCursor(128, 16);
    display.println("Please insert SD card");
    display.display(); // actually display all of the above
    delay(30);
  }
  Serial.println("SD OK");

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
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
  if (!file) {
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
  delay(3000);
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>");  // just to be sure things are working

  display.setCursor(0, 0);
  display.print("To remove the old data press and hold top button for two seconds.");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
}

void loop() {
  int reading = !digitalRead(BUTTON_A);

  //first pressed
  if (reading == HIGH && lastReading == LOW) {
    onTime = millis();
  }

  //held
  if (reading == HIGH && lastReading == HIGH) {
    if ((millis() - onTime) > holdTime) {
      hold = true;
    }
  }

  //released
  if (reading == LOW && lastReading == HIGH) {
    if (((millis() - onTime) > bounceTime) && hold != 1) {
      onRelease();
    }
    if (hold) {
      hold = false;
      remove_old_data = true;
    }
  }
  lastReading = reading;

  // read pressure sensor
  lastMeasureTime = measureTime;
  pressure_sensor_status = fetch_pressure(&pressure_data);
  measureTime = millis();

  //  switch(pressure_sensor_status)
  //  {
  //    case 0: Serial.println("Read_MR.");
  //    break;
  //    case 1: Serial.println("Read_DF2.");
  //    break;
  //    case 2: Serial.println("Read_DF3.");
  //    break;
  //    default: Serial.println("Read_DF4.");
  //    break;
  //  }

  // remove noise when inactive
  if (absolute((int)pressure_data - balance_point) < threshold) {
    PR = 0;
  } else {
    PR = (((float)pressure_data - 0.1 * 16383)) * (Pmax - Pmin) / (0.8 * 16383) + Pmin;
    PR = PR + 0.007f;
  }
    PR = PR * 6894.767; // convert to psi to kPa

  float volume_flow = 1000 * sqrt(C * absolute(PR)); // volume flow in lit/s

  //  PR = (float)  ((pressure_data - (.1*16383))*(1) / (.8*16383)) ;

  dt = (measureTime - lastMeasureTime) / 1000.;
  volume = volume_flow * dt + volume;
  Serial.println(volume,5);
  Serial.println(PR, 8);
  Serial.println(volume_flow, 8);
  Serial.print(" ");
  
  PR_data = String(pressure_data);

  //  if (remove_old_data) {
  //    display.println("Old data removed!");
  //    display.display();
  //    writeFile(SD, "/data.txt", PR_data.c_str());
  //    delay(1000);
  //    remove_old_data = false;
  //  } else {
  //    appendFile(SD, "/data.txt", PR_data.c_str());
  //  }

  display.println(volume_flow, 3);
  display.println(PR, 3);
  display.setCursor(0, 0);
  yield();
  display.display();

  delay(100);
  //  delay(100);
  display.clearDisplay();
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
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
  if (!file) {
    Serial.println("Failed to open file for appending");
    display.println("Please insert SD card");
    return;
  }
  if (file.print(message)) {
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
  address = 0x28;
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
  return (_status);
}

float absolute(float x) {
  return (x >= 0) ? x : -x;
}

void onRelease() {
  // Do nothing for now

  //  if ((millis() - lastSwitchTime) >= doubleTime) {
  //    single = 1;
  //    lastSwitchTime = millis();
  //    return;
  //  }
  //
  //  if ((millis() - lastSwitchTime) < doubleTime) {
  //    toggleLED();
  //    Serial.println("double press");
  //    single = 0;
  //    lastSwitchTime = millis();
  //  }

}
