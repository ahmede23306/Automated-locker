#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

#define buzzer 11

// Initialize the LCD (I2C address 0x27, 20 columns, 4 rows)
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myservo;

// Keypad code from library///////////////////////////////////
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
/////////////////////////////////////////////////////////////

// Password 
String password = "1234";
String input = "";
byte maxPasswordLength = 4;
byte currentPasswordLength = 0;
byte wrongAttempts = 0;
bool doorlocked = true; // true = locked, false = unlocked
byte cursorPosition = 5;
unsigned long doorOpenTime = 0; // when door was opened for the alarm 
const unsigned long OPENALARM = 120000; 

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  myservo.attach(10);
  myservo.write(0);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void loop() {
  //timer to beep if door is open more than 2 minutes
  if (!doorlocked && (millis() - doorOpenTime >= OPENALARM)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Close Door!");
    lcd.setCursor(0, 1);
    lcd.print("Press # to lock");

    while (!doorlocked) {
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);

      char key = keypad.getKey();
      if (key == '#') {
        closeDoor();
    }
  }
  }

char key = keypad.getKey();
  if (key != NO_KEY) {
    delay(60);
    if (key == '#') {
      if (!doorlocked) {
        closeDoor();
      }
    } else {
      processKey(key);
    }
  }
}

void processKey(char key) {
  if (currentPasswordLength < maxPasswordLength && doorlocked) { 
    lcd.setCursor(cursorPosition, 1);
    lcd.print(key);
    delay(500);
    lcd.setCursor(cursorPosition, 1);
    lcd.print("*");
    cursorPosition++;
    if (cursorPosition > 10) cursorPosition = 5;

    input += key;
    currentPasswordLength++;

    if (currentPasswordLength == maxPasswordLength) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Checking...");
      delay(1000);
      checkPassword();
    }
  }
}

void checkPassword() {
  if (input == password) {
    digitalWrite(buzzer, HIGH);
    delay(200); 
    digitalWrite(buzzer, LOW);
    openDoor();
    wrongAttempts = 0;
  } else {
    digitalWrite(buzzer, HIGH);
    delay(1000); 
    digitalWrite(buzzer, LOW);
    wrongAttempts++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong Password!");
    lcd.setCursor(0, 1);
    lcd.print("Try Again...");
    delay(2000);

    if (wrongAttempts >= 3) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too many fails!");
      lcd.setCursor(0, 1);
      lcd.print("Wait 10 seconds");
      digitalWrite(buzzer, HIGH);
      delay(10000); 
      digitalWrite(buzzer, LOW);
      delay(10000);
      wrongAttempts = 0;
    }
    resetPassword();
    showEnterPassword();
  }
}

void openDoor() {
  myservo.write(100); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Opened!");
  lcd.setCursor(0, 1);
  lcd.print("Press # to lock");
  doorlocked = false;
  doorOpenTime = millis(); //start timer
}

void closeDoor() {
  myservo.write(0); 
  lcd.setCursor(0, 0);
  lcd.print("Door Locked!");
  delay(1000); 
  doorlocked = true;
  resetPassword();
  showEnterPassword();
}

void resetPassword() {
  input = "";
  currentPasswordLength = 0;
  cursorPosition = 5;
}


void showEnterPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}