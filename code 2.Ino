#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "M14";
const char* password = "12345678";

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Web server on port 80
WebServer server(80);

// Servo pins
const int entranceServoPin = 25;
const int exitServoPin = 33;

Servo entranceServo;
Servo exitServo;

// Parking info variables
int slot1 = 1;
int slot2 = 1;
bool carAtEntrance = false;
bool carAtExit = false;

void updateDisplay() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.printf("Slot1: %s\n", slot1 == 0 ? "Occupied" : "Free");
  display.setCursor(0,15);
  display.printf("Slot2: %s\n", slot2 == 0 ? "Occupied" : "Free");

  display.setTextSize(2);
  display.setCursor(0, 40);

  if (slot1 == 0 && slot2 == 0) {
    display.println("Full");
  } else if ((slot1 == 0 && slot2 == 1) || (slot1 == 1 && slot2 == 0)) {
    display.println("1 Slot");
  } else {
    display.println("Empty");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed to init");
    while(1);
  }
  display.clearDisplay();
  display.display();

  // Servo setup
  entranceServo.setPeriodHertz(50);
  entranceServo.attach(entranceServoPin, 500, 2400);
  entranceServo.write(0);

  exitServo.setPeriodHertz(50);
  exitServo.attach(exitServoPin, 500, 2400);
  exitServo.write(0);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  // Setup webserver route
  server.on("/update_data", HTTP_GET, []() {
    if (server.hasArg("slot1")) slot1 = server.arg("slot1").toInt();
    if (server.hasArg("slot2")) slot2 = server.arg("slot2").toInt();
    if (server.hasArg("entrance")) carAtEntrance = server.arg("entrance").toInt() != 0;
    if (server.hasArg("exit")) carAtExit = server.arg("exit").toInt() != 0;

    Serial.printf("Received slot1=%d slot2=%d entrance=%d exit=%d\n", slot1, slot2, carAtEntrance, carAtExit);

    updateDisplay();

    if (carAtEntrance) {
      Serial.println("Opening entrance gate...");
      entranceServo.write(90);
      delay(2000);
      entranceServo.write(0);
    }

    if (carAtExit) {
      Serial.println("Opening exit gate...");
      exitServo.write(90);
      delay(2000);
      exitServo.write(0);
    }

    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}