/*
 * ESP32 - I2C MUX Master ve MQTT Yayıncısı
 * 1. 8 MUX kanalındaki 32 Arduino'yu tarar (I2C)
 * 2. 768 byte'lık ham veriyi toplar
 * 3. Veriyi Jetson'daki MQTT Broker'a HAM (RAW) byte dizisi olarak yayınlar
 */

#include <WiFi.h>
#include <Wire.h>
#include "TCA9548.h"
#include <PubSubClient.h>

// ---------- AYARLAR ----------
const int I2C_SDA_PIN = 6;
const int I2C_SCL_PIN = 7;
// 1. AĞ AYARLARI (Jetson Hotspot)
const char* WIFI_SSID = "Jetson-Hotspot-DEV01";
const char* WIFI_SIFRESI = "TUSEB_JETSON_DEV01";

// 2. MQTT AYARLARI
const char* JETSON_IP_ADRESI = "10.42.0.1"; // Jetson'un Hotspot IP'si
const int MQTT_PORT = 1883;

// Sizin eski kodunuzdaki MQTT Konu Başlığı (Topic) ve ID
const char* MQTT_TOPIC = "PAD-01/sensors";
const char* MQTT_CLIENT_ID = "PAD-01";

// 3. I2C AYARLARI
// 768 byte'lık verinin JSON'a çevrilmesi RAM'i zorlar.
// 2048'lik buffer, 768 byte'lık ham verinin rahatça gönderilmesini sağlar.
#define MQTT_MAX_PACKET_SIZE 2048 
const int DATA_BUFFER_SIZE = 768; // 32 sensör * 24 byte
byte sensorDataBuffer[DATA_BUFFER_SIZE];

// (Varsa) MUX aktivasyon pini
// const int MUX_ENABLE_PIN = 10;
// -----------------------------

WiFiClient espClient;
PubSubClient client(espClient);
TCA9548 mux(0x70);

// WiFi'ye bağlanma fonksiyonu
void setup_wifi() {
  delay(100);
  Serial.print("Wi-Fi'ye baglaniyor: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_SIFRESI);
  // Kanal 13'ü zorla (Gerekirse)
  // WiFi.begin(WIFI_SSID, WIFI_SIFRESI, 13); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi bağlantısı kuruldu!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());
}

// MQTT'ye yeniden bağlanma fonksiyonu
void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT Broker'a bağlanılıyor...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("bağlandı!");
    } else {
      Serial.print("Hata, RC=");
      Serial.print(client.state());
      Serial.println(" | 2 saniye sonra tekrar denenecek");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // (Varsa) MUX aktivasyon pinini ayarla
  // pinMode(MUX_ENABLE_PIN, OUTPUT);
  // digitalWrite(MUX_ENABLE_PIN, HIGH); // Veya LOW
  
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // I2C'yi başlat
  Wire.setClock(100000); // 100kHz I2C hızı

  // MUX'u başlat
  mux.begin();
  if (!mux.isConnected()) {
    Serial.println("PCA9548A MUX 0x70 adresinde bulunamadı! (isConnected() fail)");
    while (1); // MUX yoksa devam etme
  }
  Serial.println("MUX bulundu. (isConnected() success)");
  
  setup_wifi(); // WiFi'ye bağlan
  
  // MQTT ayarları
  client.setServer(JETSON_IP_ADRESI, MQTT_PORT);
  client.setBufferSize(MQTT_MAX_PACKET_SIZE); // ÇOK ÖNEMLİ!
}

void loop() {
  // Wi-Fi bağlantısı koptuysa, ESP32 otomatik olarak yeniden bağlanır
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi bağlantısı koptu!");
    delay(1000);
    return;
  }
  
  // MQTT bağlantısı koptuysa, yeniden bağlan
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // MQTT için gerekli

  // --- 1. I2C Veri Toplama ---
  int buffer_index = 0;
  
  for (uint8_t mux_kanal = 0; mux_kanal < 8; mux_kanal++) {
    
    if (mux.selectChannel(mux_kanal) == false) {
        // Kanal seçilemezse (olası bir bus hatası)
        Serial.print("HATA: MUX Kanal "); Serial.print(mux_kanal); Serial.println(" secilemedi.");
        for(int i=0; i<96; i++) { sensorDataBuffer[buffer_index++] = 0xFF; }
        continue;
    }

    // Bu kanaldaki 4 Arduino'yu oku (i = 0'dan 3'e)
    for (uint8_t i = 0; i < 4; i++) {
      uint8_t arduino_adresi = (mux_kanal * 4) + i + 8; // Adres 8'den 39'a
      
      if (Wire.requestFrom(arduino_adresi, (uint8_t)24) == 24) {
        // Başarılı: 24 byte'ı buffer'a oku
        for(int j=0; j<24; j++) {
          sensorDataBuffer[buffer_index++] = Wire.read();
        }
      } else {
        // Başarısız: 24 byte'ı hata (0xFF) ile doldur
        for(int j=0; j<24; j++) {
          sensorDataBuffer[buffer_index++] = 0xFF;
        }
      }
    }
  }

  // --- 2. MQTT ile Veri Yayınlama ---
  if (buffer_index == 768) {
    Serial.print("Veri toplandı. ");
    
    // Veriyi JSON olarak DEĞİL, HAM 768 BYTE olarak yayınla
    if (client.publish(MQTT_TOPIC, sensorDataBuffer, DATA_BUFFER_SIZE)) {
      Serial.println("MQTT'ye gönderildi. (768 byte)");
    } else {
      Serial.println("MQTT gönderim başarısız!");
    }
  }
  
  delay(500); // Saniyede 10 kez veri toplama
}