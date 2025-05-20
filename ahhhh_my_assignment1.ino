#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {2, 3, 6, 7};
byte colPins[COLS] = {8, 9, 10, 13};

const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const String correctPassword = "666";
String inputPassword = "";
bool systemActive = false;

enum SensorType {
  TEMPERATURE,
  SOIL_MOISTURE,
  LIGHT_INTENSITY,
  SENSOR_COUNT
};

const byte TEMP_PIN = A3;
const byte SOIL_PIN = A0;
const byte LDR_PIN  = A1;
const byte BUZZER_PIN = A2;

const unsigned int displayInterval = 3000;
unsigned long lastDisplayTime = 0;
byte currentState = 0;

const int SOIL_THRESHOLD  = 200;
const int LIGHT_DARK      = 150;
const int LIGHT_MEDIUM    = 500;

void playFailTune() {
  tone(BUZZER_PIN, 300, 250); delay(300);
  tone(BUZZER_PIN, 250, 250); delay(300);
  tone(BUZZER_PIN, 200, 250); delay(300);
  noTone(BUZZER_PIN);
}

void playChillTune() {
  tone(BUZZER_PIN, 440, 100); delay(120);
  tone(BUZZER_PIN, 520, 100); delay(120);
  noTone(BUZZER_PIN);
}

void playClickSound() {
  tone(BUZZER_PIN, 900, 40);
  delay(50);
  noTone(BUZZER_PIN);
}

void playWelcomeTone() {
  tone(BUZZER_PIN, 700, 150);
  delay(150);
  tone(BUZZER_PIN, 800, 150);
  delay(150);
  tone(BUZZER_PIN, 900, 150);
  delay(150);
  noTone(BUZZER_PIN);
}

void animateWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String welcomeText = "Welcome...";
  for (int i = 0; i < welcomeText.length(); i++) {
    lcd.print(welcomeText[i]);
    delay(150);
  }
  delay(700);
  lcd.clear();
}

void showTemperature() {
  lcd.print("Temp: ");
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * 5.0 / 1023.0;
  float temperature = (voltage - 0.5) * 100.0;
  lcd.print(temperature, 1);
  lcd.print(" C");
}

void showSoilMoisture() {
  int value = analogRead(SOIL_PIN);
  lcd.print("Soil Moisture:");
  lcd.setCursor(0, 1);
  lcd.print(value < SOIL_THRESHOLD ? "Dry" : "Wet");
}

void showLighting() {
  int value = analogRead(LDR_PIN);
  lcd.print("Lighting:");
  lcd.setCursor(0, 1);
  if (value < LIGHT_DARK)
    lcd.print("Dark");
  else if (value < LIGHT_MEDIUM)
    lcd.print("Medium Lit");
  else
    lcd.print("Lit");
}

void handlePasswordInput() {
  char key = keypad.getKey();
  if (!key) return;

  playClickSound();

  if (key == '#') {
    if (inputPassword == correctPassword) {
      animateWelcome();
      playWelcomeTone();
      lcd.clear();
      lcd.print("Welcome a8kj");
      delay(1500);
      lcd.clear();
      systemActive = true;
    } else {
      inputPassword = "";
      lcd.clear();
      lcd.print("Wrong password");
      playFailTune();
      delay(2000);
      lcd.clear();
      lcd.print("Enter password:");
    }
  } else if (key == '*') {
    inputPassword = "";
    lcd.clear();
    lcd.print("Enter password:");
  } else if (inputPassword.length() < correctPassword.length() && isDigit(key)) {
    inputPassword += key;
    lcd.setCursor(inputPassword.length() - 1, 1);
    lcd.print('*');
  }
}

void displaySensorData() {
  lcd.clear();
  playChillTune();
  switch (currentState) {
    case TEMPERATURE:
      showTemperature();
      break;
    case SOIL_MOISTURE:
      showSoilMoisture();
      break;
    case LIGHT_INTENSITY:
      showLighting();
      break;
  }
  currentState = (currentState + 1) % SENSOR_COUNT;
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.begin(16, 2);
  animateWelcome();
  lcd.print("Enter password:");
}

void loop() {
  if (!systemActive) {
    handlePasswordInput();
    return;
  }

  if (millis() - lastDisplayTime >= displayInterval) {
    lastDisplayTime = millis();
    displaySensorData();
  }
}
