//https://forum.arduino.cc/index.php?topic=309653.0

#include <Wire.h> //I2C library 0x28H
 
byte fetch_pressure(unsigned int *p_Pressure); //convert value to byte data type


#define TRUE 1
#define FALSE 0

void setup(void)
{
   Serial.begin(9600);
   Wire.begin();
   pinMode(4, OUTPUT);
   pinMode(5, OUTPUT);
   digitalWrite(5, HIGH);  // SCL remains high
   digitalWrite(4, HIGH); // SDA transfers from high to low
   digitalWrite(4, LOW);  // this turns on the MS4525, I think
   delay(5000);
   Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>");  // just to be sure things are working 
}

void loop()
{
  byte _status;
  unsigned int P_dat;
  float PR;
 
  while(1)
  {
    _status = fetch_pressure(&P_dat);
   
    switch(_status)
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
   
    PR = (float)  ((P_dat - (.1*16383))*(1) / (.8*16383)) ;
   
Serial.println(P_dat);
Serial.println(PR);
    Serial.print(" ");
   
    delay(1000);
  }
}
 
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
 
