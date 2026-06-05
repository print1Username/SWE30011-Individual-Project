#define BUTTON 2
#define RELAY_MAIN 3
#define RELAY_FAN 7
#define TILT 6

#define LED 8
#define BUZZER 9
#define TRIG 11
#define ECHO 12
#define LIGHT A0

// ================= SYSTEM =================
bool systemState = false;

// ================= MODES =================
enum DeviceMode {
  MODE_OFF,
  MODE_ON,
  MODE_AUTO
};

DeviceMode fanMode = MODE_AUTO;
DeviceMode ledMode = MODE_AUTO;

bool buzzerEnabled = true;

// ================= BUTTON =================
bool buttonState = HIGH;
bool lastStableState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// ================= ULTRASONIC =================
float distance = 0;
unsigned long lastUltraTrigger = 0;

// ================= FAN =================
bool fanOutput = false;

// ================= LED =================
bool ledOutput = false;

// ================= BUZZER =================
bool buzzerOutput = false;
unsigned long buzzerTimer = 0;
bool buzzerActive = false;

// ================= SERIAL =================
unsigned long lastPrintTime = 0;

String modeToString(DeviceMode mode) {
  switch (mode) {
    case MODE_ON: return "ON";
    case MODE_OFF: return "OFF";
    default: return "AUTO";
  }
}

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

  Serial.begin(9600);
}

// =====================================================
// SERIAL COMMANDS
// =====================================================

void handleCommand(String cmd) {

  cmd.trim();

  // SYSTEM

  if (cmd == "SYSTEM_ON") {
    systemState = true;
    return;
  }

  if (cmd == "SYSTEM_OFF") {
    systemState = false;
    return;
  }

  // FAN

  if (cmd == "FAN_ON") {
    fanMode = MODE_ON;
    return;
  }

  if (cmd == "FAN_OFF") {
    fanMode = MODE_OFF;
    return;
  }

  if (cmd == "FAN_AUTO") {
    fanMode = MODE_AUTO;
    return;
  }

  // LED

  if (cmd == "LED_ON") {
    ledMode = MODE_ON;
    return;
  }

  if (cmd == "LED_OFF") {
    ledMode = MODE_OFF;
    return;
  }

  if (cmd == "LED_AUTO") {
    ledMode = MODE_AUTO;
    return;
  }

  // BUZZER

  if (cmd == "BUZZER_ENABLE") {
    buzzerEnabled = true;
    return;
  }

  if (cmd == "BUZZER_DISABLE") {
    buzzerEnabled = false;
    noTone(BUZZER);
    buzzerActive = false;
    return;
  }
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  unsigned long now = millis();

  // ==========================================
  // READ SERIAL COMMAND
  // ==========================================

  while (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    handleCommand(cmd);
  }

  // ==========================================
  // BUTTON DEBOUNCE
  // ==========================================

  bool reading = digitalRead(BUTTON);

  if (reading != buttonState) {
    lastDebounceTime = now;
  }

  if (now - lastDebounceTime > debounceDelay) {

    if (reading != lastStableState) {

      lastStableState = reading;

      if (lastStableState == LOW) {

        systemState = !systemState;
      }
    }
  }

  buttonState = reading;

  // ==========================================
  // MAIN RELAY
  // ==========================================

  digitalWrite(RELAY_MAIN, systemState ? HIGH : LOW);

  // ==========================================
  // LIGHT SENSOR
  // ==========================================

  int lightValue = analogRead(LIGHT);

  // ==========================================
  // ULTRASONIC
  // ==========================================

  if (now - lastUltraTrigger > 60) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH, 20000);

    if (duration > 0) {

      float newDistance = duration * 0.034 / 2.0;

      if (newDistance > 2 && newDistance < 200) {

        distance =
          distance * 0.6 +
          newDistance * 0.4;
      }
    }

    lastUltraTrigger = now;
  }

  // ==========================================
  // SYSTEM OFF
  // ==========================================

  if (!systemState) {

    digitalWrite(RELAY_FAN, LOW);
    digitalWrite(LED, LOW);

    noTone(BUZZER);

    fanOutput = false;
    ledOutput = false;
    buzzerOutput = false;
  }

  // ==========================================
  // SYSTEM ON
  // ==========================================

  else {

    // ================= FAN =================

    switch (fanMode) {

      case MODE_ON:
        fanOutput = true;
        break;

      case MODE_OFF:
        fanOutput = false;
        break;

      case MODE_AUTO:

        if (distance < 25) {
          fanOutput = true;
        }
        else if (distance > 35) {
          fanOutput = false;
        }

        break;
    }

    digitalWrite(RELAY_FAN,
      fanOutput ? HIGH : LOW);

    // ================= LED =================

    switch (ledMode) {

      case MODE_ON:
        ledOutput = true;
        break;

      case MODE_OFF:
        ledOutput = false;
        break;

      case MODE_AUTO:
        ledOutput =
          (lightValue < 500);
        break;
    }

    digitalWrite(LED,
      ledOutput ? HIGH : LOW);
    // ================= BUZZER =================

    if (!buzzerEnabled) {
      noTone(BUZZER);
      buzzerOutput = false;
    }
    else {
      static unsigned long tiltHighSince = 0;
      static bool tiltTriggered = false;
      static unsigned long lastBuzzerTime = 0;

      int tilt = digitalRead(TILT);

      if (tilt == HIGH) {
        if (tiltHighSince == 0) {
          tiltHighSince = now;
        }
        else if (!tiltTriggered &&
                now - tiltHighSince >= 50 &&
                now - lastBuzzerTime >= 500) {
          tone(BUZZER, 500);
          buzzerTimer = now;
          buzzerActive = true;
          buzzerOutput = true;
          tiltTriggered = true;
          lastBuzzerTime = now;
        }
      }
      else {
        tiltHighSince = 1;
        tiltTriggered = false;
      }

      if (buzzerActive && now - buzzerTimer > 200) {
        noTone(BUZZER);
        buzzerActive = false;
        buzzerOutput = false;
      }
    }
  }

  // ==========================================
  // SEND STATUS TO PYTHON
  // ==========================================

  if (now - lastPrintTime > 300) {

    Serial.print("Light:");
    Serial.print(lightValue);

    Serial.print("|Distance:");
    Serial.print(distance, 1);

    Serial.print("|Fan:");
    Serial.print(fanOutput ? 1 : 0);

    Serial.print("|FanMode:");
    Serial.print(modeToString(fanMode));

    Serial.print("|LED:");
    Serial.print(ledOutput ? 1 : 0);

    Serial.print("|LEDMode:");
    Serial.print(modeToString(ledMode));

    Serial.print("|Buzzer:");
    Serial.print(buzzerEnabled ? 1 : 0);

    Serial.print("|System:");
    Serial.println(systemState ? 1 : 0);

    lastPrintTime = now;
  }
}
