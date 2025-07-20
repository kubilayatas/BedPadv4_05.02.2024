#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

#define MQTT_MAX_PACKET_SIZE 2048

const char* BedNumber = "PAD#01";

WiFiUDP udp;
char incomingPacket[255];
IPAddress brokerIP;

// MQTT Broker (Jetson IP adresi)
const char* mqtt_server = "192.168.1.50";  // Jetson IP'sini buraya yaz

WiFiMulti wifiMulti;
WiFiClient espClient;
PubSubClient client(espClient);


// I2C Ayarları
const int NUM_NODES = 32;
const int BYTES_PER_NODE = 24;  // Her Arduino'dan okunan veri
int Bed[NUM_NODES][BYTES_PER_NODE/2];
//uint8_t sensorData[NUM_NODES * BYTES_PER_NODE];

void setup_wifi() {
  delay(100);
  wifiMulti.addAP("CEKU", "180821ck");
  wifiMulti.addAP("TUSEB_PROJE#22606_2.4GHZ", "*TUSEB*2023");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi bağlantısı kuruldu!");
  Serial.print("Bağlantı IP adresi: ");
  Serial.println(WiFi.localIP());
  udp.begin(1991);
  client.setBufferSize(MQTT_MAX_PACKET_SIZE);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT Broker'a bağlanılıyor...");
    if (client.connect(BedNumber)) {
      Serial.println("bağlandı!");
    } else {
      Serial.print("Hata, tekrar denenecek: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void udp_loop(){
  while(1){
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;

    Serial.print("UDP Mesajı: ");
    Serial.println(incomingPacket);

    if (strstr(incomingPacket, BedNumber) != NULL) {
      String dataStr = String(incomingPacket);
      int idx = dataStr.indexOf(":") + 1;
      String ipStr = dataStr.substring(idx);
      brokerIP = udp.remoteIP();
      Serial.print(BedNumber);
      Serial.print(" icin ");
      Serial.print("MQTT Broker IP tespit edildi: ");
      Serial.println(brokerIP);
      break;
      // Burada brokerIP'yi mqtt bağlantısı için kullanabilirsin
    }
  }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();  // I2C master olarak başla (default: SDA=8, SCL=9 on ESP32-C3)
  setup_wifi();
  udp_loop();
  client.setServer(brokerIP, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wi-Fi bağlantısı kesildi!");
    while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  }
  client.loop();
  byte lowbyte=0;
  byte highbyte=0;
  // I2C'den tüm node'ları sırayla oku
  int index = 0;
  for (int addr = 1; addr <= NUM_NODES; addr++) {
    uint8_t received = Wire.requestFrom(addr + 7, BYTES_PER_NODE);
    if (received != BYTES_PER_NODE) {
      Serial.print("Eksik veri addr: "); Serial.println(addr + 7);
      continue; // bu node'u atla
      }
    if (Wire.available() >= BYTES_PER_NODE) {
      for (int i = 0; i < 12; i++) {
        highbyte = Wire.read();
        lowbyte = Wire.read();
        Bed[addr - 1][i] = (highbyte << 8) | lowbyte;
        Serial.print(Bed[addr - 1][i]); Serial.print(";");
        }
    }

    /*int count = 0;
    while (Wire.available() && count < BYTES_PER_NODE) {
      sensorData[index++] = Wire.read();
      count++;
    }*/
    delayMicroseconds(200);  // Çok kısa gecikme, ardışık okumalarda bus çakışmasın
  }

  // JSON nesnesi hazırla
  const size_t capacity = JSON_ARRAY_SIZE(384) + 384 * sizeof(int) + 128;
  StaticJsonDocument<capacity> doc;
  JsonArray data = doc.createNestedArray("sensors");
  for (int addr = 1; addr <= NUM_NODES; addr++) {
     for(int i=0;i<12;i++){
      data.add(Bed[addr-1][i]);
      }
  }
  /*for (int i = 0; i < NUM_NODES * BYTES_PER_NODE; i++) {
    data.add(sensorData[i]);
  }*/

  // Timestamp ekle
  doc["timestamp"] = millis();

  // JSON string'e çevir
  char payload[1024];
  serializeJson(doc, payload);

  // MQTT mesajını gönder
  char topic[64];
  snprintf(topic, sizeof(topic), "%s/sensors", BedNumber);
  bool sent = client.publish(topic, payload);
  if (!sent) {
    Serial.println("MQTT gönderim başarısız!");
  }

  Serial.println(payload);

  delay(100);  // Saniyede bir gönderim
}
