#include <Wire.h>

// ─── Pin Definitions ───────────────────────────
int fsrPin   = 34;
int ledPin   = 25;
int touchPin = 2;

// ─── FSR ───────────────────────────────────────
int threshold = 5;
String lastSerialState = "";

// ─── MPU6050 ───────────────────────────────────
int16_t ax, ay, az;
int16_t prev_ax = 0;
int motionThreshold = 8000;
unsigned long lastMotionTime = 0;
int motionCooldown = 500; // reduced from 800

void setupMPU() {
  Wire.begin(14, 27);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void readMPU() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(touchPin, INPUT);
  setupMPU();
  delay(500);
  Serial.println("Ready!");
}

void loop() {
  // ── FSR Logic ──────────────────────────────
  int sensorValue = analogRead(fsrPin);
  if (sensorValue > threshold) {
    digitalWrite(ledPin, HIGH);
    if (lastSerialState != "PAUSE") {
      Serial.println("PAUSE");
      lastSerialState = "PAUSE";
    }
  } else {
    digitalWrite(ledPin, LOW);
    if (lastSerialState != "PLAY") {
      Serial.println("PLAY");
      lastSerialState = "PLAY";
    }
  }

  // ── Touch Sensor Logic ─────────────────────
  int touchState = digitalRead(touchPin);
  if (touchState == HIGH) {
    Serial.println("SHOT");
    delay(300);
  }

  // ── MPU6050 Forward Only ───────────────────
  readMPU();
  int delta = ax - prev_ax;
  unsigned long now = millis();

  if (now - lastMotionTime > motionCooldown) {
    if (delta > motionThreshold) {
      Serial.println("FORWARD");
      lastMotionTime = now;
    }
  }

  prev_ax = ax;
  delay(50); // reduced from 100
}