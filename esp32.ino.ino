#include <Arduino.h>
#include "BluetoothSerial.h"
#include <ESP32Servo.h>

// ---------------- BLUETOOTH ----------------
BluetoothSerial serialBT;

// ---------------- DC MOTOR PINS ----------------
#define ENA 5
#define ENB 23
#define IN1 22
#define IN2 21
#define IN3 19
#define IN4 18

// ---------------- BLDC ESC PINS ----------------
#define ESC1_PIN 25
#define ESC2_PIN 26

Servo esc1;
Servo esc2;

// ---------------- VARIABLES ----------------
int driveSpeed = 180;      // DC motor speed
int escPulse = 1100;       // BLDC RPM pulse
bool suctionON = false;

// ---------------- MOTOR FUNCTIONS ----------------
void stopAll() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  analogWrite(ENA, driveSpeed);
  analogWrite(ENB, driveSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backward() {
  analogWrite(ENA, driveSpeed);
  analogWrite(ENB, driveSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  analogWrite(ENA, driveSpeed);
  analogWrite(ENB, driveSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  analogWrite(ENA, driveSpeed);
  analogWrite(ENB, driveSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);          // helps stable boot
  delay(500);

  serialBT.begin("ESP32 WiFi");

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Attach ESCs
  esc1.attach(ESC1_PIN, 1000, 2000);
  esc2.attach(ESC2_PIN, 1000, 2000);

  // ARM ESCs (VERY IMPORTANT)
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  delay(3000);

  stopAll();
}

// ---------------- LOOP ----------------
void loop() {
  if (serialBT.available()) {
    char cmd = serialBT.read();

    // ---- MOVEMENT ----
    if (cmd == 'F') forward();
    else if (cmd == 'B') backward();
    else if (cmd == 'L') left();
    else if (cmd == 'R') right();
    else if (cmd == 'S') stopAll();

    // ---- RPM PRESETS ----
    else if (cmd == '1') escPulse = 1100;   // F1
    else if (cmd == '2') escPulse = 1500;   // F2
    else if (cmd == '3') escPulse = 1750;   // F3
    else if (cmd == '4') escPulse = 1950;   // F4

    // ---- SUCTION TOGGLE ----
    else if (cmd == 'U') {
      suctionON = !suctionON;

      if (suctionON) {
        esc1.writeMicroseconds(escPulse);
        esc2.writeMicroseconds(escPulse);
      } else {
        esc1.writeMicroseconds(1000);
        esc2.writeMicroseconds(1000);
      }
    }
  }

  delay(5);   // small stability delay
}