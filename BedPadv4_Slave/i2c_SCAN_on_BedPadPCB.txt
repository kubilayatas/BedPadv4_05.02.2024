// Her bir Arduino hücresinin, kendisine bağlı SHT sensörlerini
// test etmesi için tasarlanmış diagnostik I2C tarama kodu.
// Bu kod, Jetson'dan bağımsız olarak çalışır ve sonuçları
// Arduino IDE'nin Seri Monitör'üne yazar.

// Gerekli Kütüphaneler
#include "SHT31_SWW.h" // Sizin de kullandığınız kütüphane
#include "Wire.h"      // Donanımsal I2C için (bu kodda kullanılmıyor ama kalsın)

// SHT sensörünün standart I2C adresi
#define SHT_ADDRESS 0x44

// Pin Tanımlamaları (Sizin kodunuzdan aynen alındı)
#define SHT1_SDA 2
#define SHT1_SCL 3
#define SHT2_SDA 4
#define SHT2_SCL 5
#define SHT3_SDA 6
#define SHT3_SCL 7
#define SHT4_SDA 8
#define SHT4_SCL 9

// Her SHT hattı için ayrı bir yazılımsal I2C nesnesi oluştur
SoftwareWire sw1(SHT1_SDA, SHT1_SCL);
SoftwareWire sw2(SHT2_SDA, SHT2_SCL);
SoftwareWire sw3(SHT3_SDA, SHT3_SCL);
SoftwareWire sw4(SHT4_SDA, SHT4_SCL);

// Belirli bir yazılımsal I2C hattını tarayan fonksiyon
void scan_bus(SoftwareWire &wire, const char* bus_name) {
  
  Serial.print(bus_name);
  Serial.print(" taranıyor... ");

  wire.beginTransmission(SHT_ADDRESS);
  byte error = wire.endTransmission();

  if (error == 0) {
    Serial.print("Adres 0x44'te cihaz BULUNDU!");
  } else {
    Serial.print("Cihaz bulunamadı. (Hata Kodu: ");
    Serial.print(error);
    Serial.print(")");
  }
  Serial.println();
}

void setup() {
  // Seri haberleşmeyi başlat (sonuçları görmek için)
  Serial.begin(9600);
  while (!Serial) {
    ; // Seri portun bağlanmasını bekle
  }
  Serial.println("\n--- Yerel SHT I2C Tarayıcı Başlatıldı ---");

  // Tüm yazılımsal I2C hatlarını başlat
  sw1.begin();
  sw2.begin();
  sw3.begin();
  sw4.begin();
}

void loop() {
  Serial.println("\n--- Yeni Tarama Başlatılıyor ---");
  
  // Her bir SHT hattını sırayla tara
  scan_bus(sw1, "SHT-1 Hattı (Pin 2,3)");
  scan_bus(sw2, "SHT-2 Hattı (Pin 4,5)");
  scan_bus(sw3, "SHT-3 Hattı (Pin 6,7)");
  scan_bus(sw4, "SHT-4 Hattı (Pin 8,9)");

  // Taramayı 5 saniyede bir tekrarla
  delay(5000);
}