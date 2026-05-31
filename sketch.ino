#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <Keypad.h>

// ======================================================
// SMART WATER LEVEL INDICATOR USING MICROCONTROLLER
// ESP32 + LCD + ULTRASONIC + PUMP + VALVE + ALARM
// ======================================================

// ---------------- LCD I2C ----------------
// LCD wiring:
// LCD SDA -> ESP32 D21
// LCD SCL -> ESP32 D22
#define SDA_PIN 21
#define SCL_PIN 22

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ---------------- Ultrasonic Sensor ----------------
#define TRIG_PIN 27
#define ECHO_PIN 26

// ---------------- Status LEDs ----------------
// D21 and D22 are reserved for LCD I2C
#define LED_RED 18
#define LED_YEL 19
#define LED_GRN 23

// ---------------- Output Devices ----------------
#define BUZ_PIN 33
#define RELAY_PIN 32
#define SERVO_PIN 13

// ---------------- Sensors and Inputs ----------------
#define DHT_PIN 15
#define POT_PIN 34
#define MODE_SWITCH_PIN 35

// ---------------- DHT Object ----------------
DHTesp dht;

// ---------------- Keypad ----------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {14, 12, 4, 16};
byte colPins[COLS] = {17, 5, 2, 25};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- System Variables ----------------
int lowThreshold = 30;
int highThreshold = 80;

bool pumpState = false;
bool valveOpen = false;
bool manualMode = false;

String inputBuffer = "";

float temperature = 0.0;
float humidity = 0.0;

unsigned long lastDHTRead = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastBuzzerToggle = 0;

bool buzzerState = false;

// Full tank alarm rings only for a short time
bool fullAlarmPlayed = false;
unsigned long fullAlarmStart = 0;
const unsigned long FULL_ALARM_DURATION = 5000;

// Ultrasonic smoothing
const int SAMPLE_COUNT = 5;

// ---------------- Servo PWM Settings ----------------
// ESP32 Arduino Core 3.x syntax
#define SERVO_FREQ 50
#define SERVO_RESOLUTION 16

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);

  // LCD I2C initialisation
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YEL, OUTPUT);
  pinMode(LED_GRN, OUTPUT);

  pinMode(BUZ_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(MODE_SWITCH_PIN, INPUT);

  dht.setup(DHT_PIN, DHTesp::DHT22);

  // New ESP32 Arduino Core 3.x PWM syntax
  // This replaces ledcSetup() and ledcAttachPin()
  ledcAttach(SERVO_PIN, SERVO_FREQ, SERVO_RESOLUTION);

  setPump(false);
  closeValve();

  startupScreen();
}

// ======================================================
// MAIN LOOP
// ======================================================
void loop() {
  int tankDepthCM = readTankDepthFromPot();

  float distanceCM = readAverageDistanceCM();
  int level = calculateWaterLevel(distanceCM, tankDepthCM);

  manualMode = digitalRead(MODE_SWITCH_PIN);

  updateKeypad();
  readDHTSensor();

  updatePumpAndValve(level);
  updateIndicators(level);

  if (millis() - lastLCDUpdate >= 500) {
    updateLCD(level, distanceCM, tankDepthCM);
    lastLCDUpdate = millis();
  }

  delay(50);
}

// ======================================================
// TANK DEPTH CALIBRATION
// ======================================================
int readTankDepthFromPot() {
  int potValue = analogRead(POT_PIN);

  // Simulated tank depth range: 60 cm to 150 cm
  int tankDepthCM = map(potValue, 0, 4095, 60, 150);

  return tankDepthCM;
}

// ======================================================
// ULTRASONIC DISTANCE READING
// ======================================================
float readSingleDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return 999.0;
  }

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

float readAverageDistanceCM() {
  float total = 0;
  int valid = 0;

  for (int i = 0; i < SAMPLE_COUNT; i++) {
    float d = readSingleDistanceCM();

    if (d > 0 && d < 400) {
      total += d;
      valid++;
    }

    delay(10);
  }

  if (valid == 0) {
    return 999.0;
  }

  return total / valid;
}

// ======================================================
// WATER LEVEL CALCULATION
// ======================================================
int calculateWaterLevel(float distanceCM, int tankDepthCM) {
  // When water is close to the sensor, tank is full.
  // 5 cm is treated as the full tank distance.
  const int fullDistanceCM = 5;

  int level = map((int)distanceCM, tankDepthCM, fullDistanceCM, 0, 100);
  level = constrain(level, 0, 100);

  return level;
}

// ======================================================
// DHT22 SENSOR
// ======================================================
void readDHTSensor() {
  if (millis() - lastDHTRead >= 2000) {
    TempAndHumidity data = dht.getTempAndHumidity();

    if (!isnan(data.temperature) && !isnan(data.humidity)) {
      temperature = data.temperature;
      humidity = data.humidity;
    }

    lastDHTRead = millis();
  }
}

// ======================================================
// KEYPAD FUNCTIONS
// A = Set LOW threshold
// B = Set HIGH threshold
// C = Toggle pump in manual mode
// D = Reset thresholds
// * = Clear typed number
// # = Print typed number to Serial Monitor
// ======================================================
void updateKeypad() {
  char key = keypad.getKey();

  if (!key) return;

  if (key >= '0' && key <= '9') {
    if (inputBuffer.length() < 3) {
      inputBuffer += key;
    }
  }

  else if (key == '*') {
    inputBuffer = "";
    Serial.println("Input cleared");
  }

  else if (key == '#') {
    Serial.print("Typed value: ");
    Serial.println(inputBuffer);
  }

  else if (key == 'A') {
    int value = inputBuffer.toInt();

    if (value >= 5 && value <= 70) {
      lowThreshold = value;
      Serial.print("LOW threshold set to ");
      Serial.println(lowThreshold);
    } else {
      Serial.println("Invalid LOW threshold. Use 5 to 70.");
    }

    inputBuffer = "";
  }

  else if (key == 'B') {
    int value = inputBuffer.toInt();

    if (value >= 40 && value <= 95 && value > lowThreshold) {
      highThreshold = value;
      Serial.print("HIGH threshold set to ");
      Serial.println(highThreshold);
    } else {
      Serial.println("Invalid HIGH threshold. Use 40 to 95 and greater than LOW.");
    }

    inputBuffer = "";
  }

  else if (key == 'C') {
    if (manualMode) {
      pumpState = !pumpState;

      Serial.print("Manual pump changed to ");
      Serial.println(pumpState ? "ON" : "OFF");
    } else {
      Serial.println("Switch to MANUAL mode first");
    }
  }

  else if (key == 'D') {
    lowThreshold = 30;
    highThreshold = 80;
    inputBuffer = "";

    Serial.println("Thresholds reset to LOW=30%, HIGH=80%");
  }
}

// ======================================================
// PUMP AND VALVE CONTROL
// ======================================================
void updatePumpAndValve(int level) {
  if (manualMode) {
    // In manual mode, keypad C controls pump state
    setPump(pumpState);

    if (pumpState) {
      openValve();
    } else {
      closeValve();
    }

    return;
  }

  // Automatic mode with hysteresis
  if (level <= lowThreshold) {
    pumpState = true;
  }

  if (level >= highThreshold) {
    pumpState = false;
  }

  setPump(pumpState);

  if (pumpState) {
    openValve();
  } else {
    closeValve();
  }
}

void setPump(bool state) {
  pumpState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
}

// ======================================================
// SERVO VALVE CONTROL WITHOUT ESP32Servo LIBRARY
// ======================================================
void writeServoAngle(int angle) {
  angle = constrain(angle, 0, 180);

  // Standard servo pulse range
  // 0 degrees   -> about 500 microseconds
  // 180 degrees -> about 2400 microseconds
  int pulseUS = map(angle, 0, 180, 500, 2400);

  // Servo frequency = 50 Hz
  // Period = 20,000 microseconds
  uint32_t duty = (uint32_t)((pulseUS * 65535UL) / 20000UL);

  // New ESP32 Arduino Core 3.x syntax uses pin, not channel
  ledcWrite(SERVO_PIN, duty);
}

void openValve() {
  valveOpen = true;
  writeServoAngle(90);
}

void closeValve() {
  valveOpen = false;
  writeServoAngle(0);
}

// ======================================================
// LED AND BUZZER STATUS LOGIC
// ======================================================
void updateIndicators(int level) {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YEL, LOW);
  digitalWrite(LED_GRN, LOW);

  // ---------------- LOW WATER ----------------
  // Red LED ON and buzzer continues warning
  if (level <= lowThreshold) {
    digitalWrite(LED_RED, HIGH);

    fullAlarmPlayed = false;

    if (millis() - lastBuzzerToggle >= 400) {
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZ_PIN, 1000);
      } else {
        noTone(BUZ_PIN);
      }

      lastBuzzerToggle = millis();
    }

    return;
  }

  // ---------------- FULL TANK ----------------
  // Green LED ON. Pump OFF. Valve closed.
  // Buzzer rings for short time, then stops.
  if (level >= highThreshold) {
    digitalWrite(LED_GRN, HIGH);

    if (!fullAlarmPlayed) {
      fullAlarmStart = millis();
      fullAlarmPlayed = true;
    }

    if (millis() - fullAlarmStart <= FULL_ALARM_DURATION) {
      if (millis() - lastBuzzerToggle >= 200) {
        buzzerState = !buzzerState;

        if (buzzerState) {
          tone(BUZ_PIN, 1800);
        } else {
          noTone(BUZ_PIN);
        }

        lastBuzzerToggle = millis();
      }
    } else {
      noTone(BUZ_PIN);
    }

    return;
  }

  // ---------------- NORMAL LEVEL ----------------
  digitalWrite(LED_YEL, HIGH);
  noTone(BUZ_PIN);

  fullAlarmPlayed = false;
}

// ======================================================
// LCD DISPLAY
// ======================================================
void updateLCD(int level, float distanceCM, int tankDepthCM) {
  String line0 = "";
  String line1 = "";
  String line2 = "";
  String line3 = "";

  line0 = "SMART WATER SYSTEM";

  line1 += "L:";
  line1 += String(level);
  line1 += "% ";

  line1 += "D:";
  if (distanceCM >= 999) {
    line1 += "ERR";
  } else {
    line1 += String((int)distanceCM);
    line1 += "cm";
  }

  line1 += " T:";
  line1 += String((int)temperature);
  line1 += "C";

  line2 += manualMode ? "MODE:MAN " : "MODE:AUTO";
  line2 += " P:";
  line2 += pumpState ? "ON " : "OFF";
  line2 += " V:";
  line2 += valveOpen ? "OPN" : "CLS";

  line3 += "L:";
  line3 += String(lowThreshold);
  line3 += " H:";
  line3 += String(highThreshold);
  line3 += " ";

  if (level <= lowThreshold) {
    line3 += "LOW-FILL";
  }
  else if (level >= highThreshold) {
    line3 += "FULL-STOP";
  }
  else {
    line3 += "NORMAL";
  }

  printLCDLine(0, line0);
  printLCDLine(1, line1);
  printLCDLine(2, line2);
  printLCDLine(3, line3);
}

void printLCDLine(int row, String text) {
  lcd.setCursor(0, row);

  if (text.length() > 20) {
    text = text.substring(0, 20);
  }

  while (text.length() < 20) {
    text += " ";
  }

  lcd.print(text);
}

// ======================================================
// STARTUP SCREEN
// ======================================================
void startupScreen() {
  printLCDLine(0, "SMART WATER LEVEL");
  printLCDLine(1, "MICROCONTROLLER");
  printLCDLine(2, "PUMP VALVE ALARM");
  printLCDLine(3, "LCD I2C D21 D22");

  delay(2000);
  lcd.clear();
}
