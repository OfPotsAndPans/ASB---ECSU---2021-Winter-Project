#include <ESP8266WiFi.h>
WiFiServer server(80);
IPAddress IP(192, 168, 4, 15);
IPAddress mask = (255, 255, 255, 0);
byte ledPin = 2;// the onboard led

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SALAM_AP", "");
  WiFi.softAPConfig(IP, IP, mask);
  server.begin();//begin
  pinMode(ledPin, OUTPUT);
  Serial.println();
  Serial.println("accesspoint_1.ino");
  Serial.println("Server started.");
  Serial.print("IP: "); Serial.println(WiFi.softAPIP()); //get ip
}
void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  digitalWrite(ledPin, LOW); //off
  String request = client.readStringUntil('\r');
  Serial.println("********************************");
  Serial.println("From the station: " + request);
  client.flush();
  Serial.print("Byte sent to the station: ");
  Serial.println(client.println(request + "fizz" + "\r"));
  digitalWrite(ledPin, HIGH);//on
}
