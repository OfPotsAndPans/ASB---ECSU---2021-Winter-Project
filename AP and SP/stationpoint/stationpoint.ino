#include <ESP8266WiFi.h>
byte ledPin = 2; // the onboard led
char ssid[] = "SALAM_AP";           // SSID of your AP
char pass[] = "";         // no password for the AP
IPAddress server(192,168,4,15);     // IP address of the AP
WiFiClient client;
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  Serial.println();
  Serial.println("Connecting to the AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");//while trying
    delay(2000);
  }
  Serial.println();
  Serial.println("Connected");
  Serial.println("station_1.ino");
  Serial.print("LocalIP:"); Serial.println(WiFi.localIP());
  pinMode(ledPin, OUTPUT);
}
void loop() {
  client.connect(server, 80);
  digitalWrite(ledPin, LOW);//off
  Serial.println("********************************");
  Serial.print("Bytes sent to the AP: ");
  Serial.println(client.print("buzz\r"));
  String answer = client.readStringUntil('\r');
  Serial.println("From the AP: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);//on
  client.stop();
  delay(2000);
}
