//
//    FILE: SHT31_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/SHT31
#define SHT_address   0x44
#define USESOFTWIRE
#include "SHT31_SWW.h"

#include "Wire.h"
//#include "SHT31.h"
////////////////////////
#define ARDUCOMM_SDA A4
#define ARDUCOMM_SCL A5
#define ADDR 1
#define ARDUCOMM_ADRESS (ADDR+7) //Adress=X queue: X-7 eg. Adress=8 queue:8-7=1
////////////////////////
#define SHT1_SDA 2
#define SHT1_SCL 3
////////////////////////
#define SHT2_SDA 4
#define SHT2_SCL 5
////////////////////////
#define SHT3_SDA 6
#define SHT3_SCL 7
////////////////////////
#define SHT4_SDA 8
#define SHT4_SCL 9
////////////////////////
#define ARDU_RST 10
////////////////////////
#define FSR1 A0
#define FSR2 A1
#define FSR3 A2
#define FSR4 A3
////////////////////////
SoftwareWire sw1(SHT1_SDA, SHT1_SCL);
SoftwareWire sw2(SHT2_SDA, SHT2_SCL);
SoftwareWire sw3(SHT3_SDA, SHT3_SCL);
SoftwareWire sw4(SHT4_SDA, SHT4_SCL);
////////////////////////
int sensorValue = 0;
uint16_t raw_shtVal=0;
byte buffer[40];

SHT31_SWW sht1;
SHT31_SWW sht2;
SHT31_SWW sht3;
SHT31_SWW sht4;

int MASK[]   = {0x00FF, 0xFF00};
int SHIFT[]  = {0,    8};

byte getByteAt(int value, int position)
{
  int result = value & MASK[position];  // binary AND
  result = result >> SHIFT[position];    // Shift right, moving all bits
  byte resultAsByte = (byte) result;     // Convert to an actual byte
  return resultAsByte;  
}

void setup()
{
  //pinMode(ARDU_RST, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  //digitalWrite(ARDU_RST, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  Serial.println("Uyandim");

  //Wire.begin(ARDUCOMM_ADRESS);
  Wire.begin(ARDUCOMM_ADRESS);
  Wire.setClock(100000);
  Wire.onRequest(requestEvent);
  //Wire.onRequest(requesttt);

  sw1.begin();
  sw2.begin();
  sw3.begin();
  sw4.begin();

  sht1.begin(SHT_address, &sw1);
  sht2.begin(SHT_address, &sw2);
  sht3.begin(SHT_address, &sw3);
  sht4.begin(SHT_address, &sw4);
}


void loop()
{
  ///////////////////////////////////////////////////////////////////////////////
  sensorValue = analogRead(FSR1);
  buffer[0] = getByteAt(sensorValue,1);//High byte
  buffer[1] = getByteAt(sensorValue,0);//Low byte
  ////////////////////
  sensorValue = analogRead(FSR2);
  buffer[2] = getByteAt(sensorValue,1);//High byte
  buffer[3] = getByteAt(sensorValue,0);//Low byte
  ////////////////////
  sensorValue = analogRead(FSR3);
  buffer[4] = getByteAt(sensorValue,1);//High byte
  buffer[5] = getByteAt(sensorValue,0);//Low byte
  ////////////////////
  sensorValue = analogRead(FSR4);
  buffer[6] = getByteAt(sensorValue,1);//High byte
  buffer[7] = getByteAt(sensorValue,0);//Low byte
  ///////////////////////////////////////////////////////////////////////////////
  sht1.read(false);         //  default = true/fast       slow = false
  sht2.read(false);
  sht3.read(false);
  sht4.read(false);
  ////////////////////
  if (sht1.isConnected()){
    raw_shtVal = sht1.getRawHumidity();    buffer[8]  = getByteAt(raw_shtVal,1);  buffer[9] = getByteAt(raw_shtVal,0);
    raw_shtVal = sht1.getRawTemperature(); buffer[10] = getByteAt(raw_shtVal,1); buffer[11] = getByteAt(raw_shtVal,0);
    }
  else{
      raw_shtVal = 1000 + ADDR;    buffer[8]  = getByteAt(raw_shtVal,1);  buffer[9] = getByteAt(raw_shtVal,0);
      raw_shtVal = 1000 + ADDR;    buffer[10] = getByteAt(raw_shtVal,1); buffer[11] = getByteAt(raw_shtVal,0);
    }
  ////////////////////
  if (sht2.isConnected()){
    raw_shtVal = sht2.getRawHumidity();    buffer[12]  = getByteAt(raw_shtVal,1);  buffer[13] = getByteAt(raw_shtVal,0);
    raw_shtVal = sht2.getRawTemperature(); buffer[14] = getByteAt(raw_shtVal,1); buffer[15] = getByteAt(raw_shtVal,0);
    }
  else{
      raw_shtVal = 1000 + ADDR;    buffer[12]  = getByteAt(raw_shtVal,1);  buffer[13] = getByteAt(raw_shtVal,0);
      raw_shtVal = 1000 + ADDR;    buffer[14] = getByteAt(raw_shtVal,1); buffer[15] = getByteAt(raw_shtVal,0);
    }
  ////////////////////
  if (sht3.isConnected()){
    raw_shtVal = sht3.getRawHumidity();    buffer[16]  = getByteAt(raw_shtVal,1);  buffer[17] = getByteAt(raw_shtVal,0);
    raw_shtVal = sht3.getRawTemperature(); buffer[18] = getByteAt(raw_shtVal,1); buffer[19] = getByteAt(raw_shtVal,0);
    }
  else{
      raw_shtVal = 1000 + ADDR;    buffer[16]  = getByteAt(raw_shtVal,1);  buffer[17] = getByteAt(raw_shtVal,0);
      raw_shtVal = 1000 + ADDR;    buffer[18] = getByteAt(raw_shtVal,1); buffer[19] = getByteAt(raw_shtVal,0);
    }
  ////////////////////
  if (sht4.isConnected()){
    raw_shtVal = sht4.getRawHumidity();    buffer[20]  = getByteAt(raw_shtVal,1);  buffer[21] = getByteAt(raw_shtVal,0);
    raw_shtVal = sht4.getRawTemperature(); buffer[22] = getByteAt(raw_shtVal,1); buffer[23] = getByteAt(raw_shtVal,0);
    }
  else{
      raw_shtVal = 1000 + ADDR;    buffer[20]  = getByteAt(raw_shtVal,1);  buffer[21] = getByteAt(raw_shtVal,0);
      raw_shtVal = 1000 + ADDR;    buffer[22] = getByteAt(raw_shtVal,1); buffer[23] = getByteAt(raw_shtVal,0);
    }
  ///////////////////////////////////////////////////////////////////////////////
  delay(1);
}
void requesttt(){
  int two_byte = 1022;
  Wire.write(getByteAt(two_byte,1)); //High byte
  Wire.write(getByteAt(two_byte,0)); //Low byte
}
void requestEvent() {
  for(int i=0;i<=23;i++){
    Wire.write(buffer[i]);
  }
}
//uint16_t getRawHumidity()    { return _rawHumidity; };
//uint16_t getRawTemperature() { return _rawTemperature; };