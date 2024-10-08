#include "Wire.h"

////////////////////////
#define ARDUCOMM_SDA A4
#define ARDUCOMM_SCL A5
#define ADDR 90
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
int Bed[36][12];

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
  Serial.println("Uyandım");

  Wire.begin(110);
  Wire.setClock(100000);
  Wire.onRequest(requestEvent);
}

void loop()
{
  byte lowbyte=0;
  byte highbyte=0;
  int fsrdeger=0;
  for (int j=1; j<=34; j++){
    //Serial.print(j);Serial.print(". Data:");
    Wire.requestFrom(7+j,24); //24 byte talep ediyoruz
    if (Wire.available()) {
      for(int i=0;i<12;i++){
        highbyte = Wire.read();
        lowbyte = Wire.read();
        //Bed[j-1][i] = highbyte << 8 | lowbyte;
        fsrdeger = highbyte << 8 | lowbyte;
        //Serial.print(Bed[j-1][i]);Serial.print(",");
        //if(i<4){Serial.print(fsrdeger);Serial.print(",");}
        Serial.print(fsrdeger);Serial.print(",");
      }
      }
      else{Serial.print("#,#,#,#,#,#,#,#,#,#,#,#");Serial.print(",");}
  }
  Serial.println();
}
void requestEvent() {

}
