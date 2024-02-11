//
//    FILE: SHT31_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/SHT31_SWW


#include "SHT31_SWW.h"

#define SHT31_ADDRESS   0x44

#define SHT_SENSOR_COUNT 4

SoftwareWire sw1(4, 5);
SoftwareWire sw2(6, 7);
SoftwareWire sw3(8, 9);
SoftwareWire sw4(10, 11);

uint32_t start;
uint32_t stop;

SHT31_SWW sht1;
SHT31_SWW sht2;
SHT31_SWW sht3;
SHT31_SWW sht4;


void setup()
{
  Serial.begin(115200);
  
  sw1.begin();
  sw2.begin();
  sw3.begin();
  sw4.begin();

  bool b = sht1.begin(SHT31_ADDRESS, &sw1);
  Serial.print("SHT1 CONN: ");
  Serial.print(b);Serial.print("\tSTAT: ");
  uint16_t stat = sht1.readStatus();
  Serial.print(stat, HEX);
  Serial.println();

  b = sht2.begin(SHT31_ADDRESS, &sw2);
  Serial.print("SHT2 CONN: ");
  Serial.print(b);Serial.print("\tSTAT: ");
  stat = sht2.readStatus();
  Serial.print(stat, HEX);
  Serial.println();

  b = sht3.begin(SHT31_ADDRESS, &sw3);
  Serial.print("SHT3 CONN: ");
  Serial.print(b);Serial.print("\tSTAT: ");
  stat = sht3.readStatus();
  Serial.print(stat, HEX);
  Serial.println();

  b = sht4.begin(SHT31_ADDRESS, &sw4);
  Serial.print("SHT4 CONN: ");
  Serial.print(b);Serial.print("\tSTAT: ");
  stat = sht4.readStatus();
  Serial.print(stat, HEX);
  Serial.println();
}


void loop()
{
  start = micros();
  sht1.read(false);         //  default = true/fast       slow = false
  sht2.read(false);
  sht3.read(false);
  sht4.read(false);
  stop = micros();

  Serial.print("SHT1_Temp:");
  Serial.print(sht1.getTemperature(), 1);
  Serial.print(",");
  Serial.print("SHT1_Hum:");
  Serial.print(sht1.getHumidity(), 1);
  Serial.print(",");

  Serial.print("SHT2_Temp:");
  Serial.print(sht2.getTemperature(), 1);
  Serial.print(",");
  Serial.print("SHT2_Hum:");
  Serial.print(sht2.getHumidity(), 1);
  Serial.print(",");

  Serial.print("SHT3_Temp:");
  Serial.print(sht3.getTemperature(), 1);
  Serial.print(",");
  Serial.print("SHT3_Hum:");
  Serial.print(sht3.getHumidity(), 1);
  Serial.print(",");

  Serial.print("SHT4_Temp:");
  Serial.print(sht4.getTemperature(), 1);
  Serial.print(",");
  Serial.print("SHT4_Hum:");
  Serial.print(sht4.getHumidity(), 1);

  Serial.println();

  //Serial.print(stop - start);
  delay(100);
}


//  -- END OF FILE --
