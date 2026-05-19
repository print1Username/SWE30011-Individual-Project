#define BUTTON 2
#define RELAY_MAIN 3
#define RELAY_FAN 7
#define TILT 6

#define LED 8
#define BUZZER 9
#define TRIG 11
#define ECHO 12
#define LIGHT A0

// ================= SYSTEM STATE =================
bool systemState = false;

// ================= BUTTON DEBOUNCE =================
bool buttonState = HIGH;
bool lastStableState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// ================= ULTRASONIC =================
unsigned long lastUltraTrigger = 0;
bool echoWaiting = false;
float distance = 0;

// ================= FAN STATE =================
bool fanState = false;

// ================= TILT =================
bool buzzerActive = false;
unsigned long buzzerTimer = 0;

// ================= SERIAL =================
unsigned long lastPrintTime = 0;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(RELAY_MAIN, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);

  pinMode(TILT, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  digitalWrite(RELAY_MAIN, LOW);
  digitalWrite(RELAY_FAN, LOW);
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.begin(9600);
}

void loop() {

  unsigned long now = millis();

  // ================= LIGHT SENSOR =================
  int lightValue = analogRead(LIGHT);

  // ================= BUTTON DEBOUNCE =================
  bool reading = digitalRead(BUTTON);

  if (reading != buttonState) {
    lastDebounceTime = now;
  }

  if (now - lastDebounceTime > debounceDelay) {
    if (reading != lastStableState) {
      lastStableState = reading;

      if (lastStableState == LOW) {
        systemState = !systemState;
        digitalWrite(RELAY_MAIN, systemState ? HIGH : LOW);
      }
    }
  }

  buttonState = reading;

  // ================= ULTRASONIC (NON-BLOCKING SIMPLE) =================
  if (now - lastUltraTrigger > 60) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH, 20000);

    if (duration > 0) {
      float newDistance = duration * 0.034 / 2;

      if (newDistance > 2 && newDistance < 200) {
        distance = distance * 0.6 + newDistance * 0.4;
      }
    }

    lastUltraTrigger = now;
  }

  // ================= FAN CONTROL (REAL-TIME ONLY) =================
  float d = distance;

  if (d < 25) {
    if (!fanState) {
      digitalWrite(RELAY_FAN, HIGH);
      fanState = true;
      Serial.println("FAN ON");
    }
  } 
  else if (d > 35) {
    if (fanState) {
      digitalWrite(RELAY_FAN, LOW);
      fanState = false;
      Serial.println("FAN OFF");
    }
  }

  // ================= TILT SENSOR =================
  static int lastTilt = HIGH;
  int tilt = digitalRead(TILT);

  if (lastTilt == HIGH && tilt == LOW && !systemState) {
    tone(BUZZER, 500);
    buzzerTimer = now;
    buzzerActive = true;
  }

  lastTilt = tilt;

  if (buzzerActive && now - buzzerTimer > 200) {
    noTone(BUZZER);
    buzzerActive = false;
  }

  // ================= LED CONTROL =================
  if (!systemState) {
    digitalWrite(LED, lightValue < 500 ? HIGH : LOW);
  } else {
    digitalWrite(LED, LOW);
  }

  // ================= SERIAL DEBUG =================
  if (now - lastPrintTime > 300) {
    Serial.print("Light: ");
    Serial.print(lightValue);

    Serial.print(" | Distance: ");
    Serial.print(distance);

    Serial.print(" | FAN: ");
    Serial.print(fanState);

    Serial.print(" | System: ");
    Serial.println(systemState);

    lastPrintTime = now;
  }
}