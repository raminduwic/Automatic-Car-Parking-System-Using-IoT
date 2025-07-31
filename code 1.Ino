#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "M14";
const char* password = "12345678";

// Receiver IP
const char* serverName = "http://192.168.120.160/update_data";

// IR sensor pins
const int ir1 = 34;
const int ir2 = 35;

// Ultrasonic sensor pins
const int trig1 = 12; // Entrance
const int echo1 = 14;
const int trig2 = 27; // Exit
const int echo2 = 26;

void setup() {
  Serial.begin(115200);

  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);

  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);

  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  if (duration == 0) return 999; // no echo received

  long distance = duration * 0.034 / 2; // cm
  return distance;
}

void loop() {
  int slot1 = digitalRead(ir1); // 0 = occupied, 1 = free
  int slot2 = digitalRead(ir2);

  long distEntrance = getDistance(trig1, echo1);
  long distExit = getDistance(trig2, echo2);

  bool carAtEntrance = distEntrance < 15;
  bool carAtExit = distExit < 15;

  Serial.printf("Slot1=%d Slot2=%d EntranceDist=%ld ExitDist=%ld\n", slot1, slot2, distEntrance, distExit);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverName;
    url += "?slot1=" + String(slot1);
    url += "&slot2=" + String(slot2);
    url += "&entrance=" + String(carAtEntrance ? 1 : 0);
    url += "&exit=" + String(carAtExit ? 1 : 0);

    http.begin(url);
    int code = http.GET();
    if (code > 0) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000); // Send every 2 seconds
}
