#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

#define PIN_EN 2 // GPIO2

WiFiUDP Udp;
IPAddress ipAddressMulticast(224, 1, 2, 3);
uint16_t port = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet


void setup() {
  // put your setup code here, to run once:  
  digitalWrite(PIN_EN, LOW);
  pinMode(PIN_EN, OUTPUT);
  
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
  
  Udp.beginMulticast(WiFi.localIP(), ipAddressMulticast, port);
}

void loop() {
  // put your main code here, to run repeatedly:
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); 
   
    digitalWrite(PIN_EN, HIGH);
    Serial.write(packetBuffer, n);
    Serial.flush();
    digitalWrite(PIN_EN, LOW);
  }

  if (Serial.available()) {   
    char *ptr = packetBuffer;
    while (Serial.available()) {
      while (Serial.available()) {
        *ptr++ = Serial.read();
      }
      delay(4);
    }

    Udp.beginPacketMulticast(ipAddressMulticast, port, WiFi.localIP());
    Udp.write(packetBuffer, ptr - packetBuffer);
    Udp.endPacket();
  }
}