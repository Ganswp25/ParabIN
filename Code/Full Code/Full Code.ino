#define BLYNK_TEMPLATE_ID "TMPL6di8nqQjZ"
#define BLYNK_TEMPLATE_NAME "ParabIN"
#define BLYNK_AUTH_TOKEN "iYqD7n9p-vwJogKEVJFGUbpPlKLi-BT-"

#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

#define PHPin A0
#define sensorPin A4
#define BLYNK_PRINT Serial

char auth[] = "iYqD7n9p-vwJogKEVJFGUbpPlKLi-BT-"; 
char ssid[] = "Ganswp";
char pass[] = "12345678";

Servo myservo;
Servo myservo2;

int servoPin = 4;
int servoPin2 = 16;
int currentPosition = 0;
int currentPosition2 = 0;
int button = 2;
int manual = 3;
int buf[10];
unsigned long previousMillis = 0;
unsigned long lastServoMoveMillis = 0;
const long interval = 900000;
const long readInterval = 1000;
const long timePrintInterval = 30000;

float ph(float voltage) {
  return 7 + ((2.5 - voltage) / 0.18); 
}

String fuzzyStatus(float voltage) {
  if (voltage < 500.0) {
    return "Kotor";
  } else if (voltage >= 500.0 && voltage <= 1000.0) {
    return "Kusam";
  } else {
    return "Jernih";
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PHPin, INPUT);

  myservo.attach(servoPin);
  myservo2.attach(servoPin2);

  myservo.write(currentPosition);
  myservo2.write(currentPosition2);

  Serial.println("Setup selesai");

  Blynk.begin(auth, ssid, pass);
  configTime(7 * 3600, 0, "pool.ntp.org");

  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWaktu tersinkronisasi");
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mendapatkan waktu");
    return;
  }
  Serial.printf("Waktu saat ini: %02d:%02d:%02d WIB\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void sendData() {
  int read_button = digitalRead(button);
  int read_manual = digitalRead(manual);
  if (read_button == 1 && read_manual == 0) {
    myservo2.write(45);
  } else if (read_button == 0 && read_manual == 0) {
    myservo2.write(0);
  }
}

BLYNK_WRITE(V1) {
  int data = param.asInt();
  myservo2.write(data);
  Blynk.virtualWrite(V1, data);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= readInterval) {
    previousMillis = currentMillis;

    for (int i = 0; i < 10; i++) {
      buf[i] = analogRead(PHPin);
      delay(10);
    }

    float avgValue = 0;
    for (int i = 0; i < 10; i++) {
      avgValue += buf[i];
    }
    float pHVol = (float)avgValue * 5.0 / 4096 / 10;
    float pHValue = -3.109 * pHVol + 19.54; // regresi linear

    Serial.print("pH: ");
    Serial.println(pHValue);

    int sensorValue = analogRead(sensorPin);
    float voltage = sensorValue * (5000.0 / 4096.0);

    String status = fuzzyStatus(voltage);

    Serial.print("Nilai sensor: ");
    Serial.print(sensorValue);
    Serial.print("\t Tegangan (mV): ");
    Serial.print(voltage);
    Serial.print("\t Status: ");
    Serial.println(status);

    Blynk.virtualWrite(V0, pHValue);
    Blynk.virtualWrite(V2, status);

    if (pHValue < 5.1 && (currentMillis - lastServoMoveMillis >= interval)) {
      currentPosition = 45;
      myservo.write(currentPosition);
      Serial.println("Servo dibuka ke 45 derajat");

      delay(1000);

      currentPosition = 0;
      myservo.write(currentPosition);
      Serial.println("Servo ditutup ke 0 derajat");

      lastServoMoveMillis = currentMillis;
    }
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mendapatkan waktu");
    return;
  }

  static unsigned long lastTimePrintMillis = 0;
  if (currentMillis - lastTimePrintMillis >= timePrintInterval) {
    printLocalTime();
    lastTimePrintMillis = currentMillis;
  }

  if ((timeinfo.tm_hour == 8 || timeinfo.tm_hour == 16) && timeinfo.tm_min == 0 && timeinfo.tm_sec < 10) {
    currentPosition2 = 45;
    myservo2.write(currentPosition2);
    Serial.println("Servo 2 dibuka ke 45 derajat");

    delay(1000);

    currentPosition2 = 0;
    myservo2.write(currentPosition2);
    Serial.println("Servo 2 ditutup ke 0 derajat");
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "makan") {
      currentPosition2 = 45;
      myservo2.write(currentPosition2);
      Serial.println("Makan Wak");

      delay(1000);

      currentPosition2 = 0;
      myservo2.write(currentPosition2);
      Serial.println("Mantap");
      Blynk.virtualWrite(V3, currentPosition2);
    } else {
      Serial.println("Perintah tidak dikenal");
    }
  }

  sendData();
  Blynk.run();
}
