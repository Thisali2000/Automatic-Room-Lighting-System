#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Initialize the LCD with the I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Bluetooth module connections
SoftwareSerial bluetooth(10, 11); // RX, TX

#define sensorPin1 7
#define sensorPin2 8
#define ledPin 9

int sensorState1 = 0;
int sensorState2 = 0;
int count = 0;
bool ledManualControl = false;

void setup() {
  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("COUNTER");
  lcd.setCursor(0, 1);
  lcd.print("No Visitors     ");
  delay(2000);

  // Initialize Bluetooth
  bluetooth.begin(9600);
}

void loop() {
  sensorState1 = digitalRead(sensorPin1);
  sensorState2 = digitalRead(sensorPin2);

  if (sensorState1 == LOW) {
    count++;
    while (digitalRead(sensorPin1) == LOW); 
    delay(500); // Debounce delay
  }

  if (sensorState2 == LOW) {
    count--;
    while (digitalRead(sensorPin2) == LOW); 
    delay(500); // Debounce delay
  }

  if (count <= 0) {
    count = 0; 
    lcd.setCursor(0, 1);
    lcd.print("No Visitors     ");
    bluetooth.println("No Visitors");

    if (!ledManualControl) {
      digitalWrite(ledPin, LOW); 
    }
  } else if (count > 0 && count < 10) {
    lcd.setCursor(0, 1);
    lcd.print("Visitors:   ");
    lcd.setCursor(12, 1);
    lcd.print(count);
    lcd.print(" ");
    bluetooth.print("Visitors: ");
    bluetooth.println(count);

    if (!ledManualControl) {
      digitalWrite(ledPin, HIGH); 
    }
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Visitors:   ");
    lcd.setCursor(12, 1);
    lcd.print(count);
    bluetooth.print("Visitors: ");
    bluetooth.println(count);

    if (!ledManualControl) {
      digitalWrite(ledPin, HIGH); 
    }
  }

  // Check for incoming Bluetooth commands
  if (bluetooth.available()) {
    char command = bluetooth.read();
    if (command == 'r') { // Reset command
      count = 0;
      lcd.setCursor(0, 1);
      lcd.print("No Visitors     ");
      bluetooth.println("Counter reset");
    } else if (command == 'o') { // Turn LED on manually
      digitalWrite(ledPin, LOW);
      ledManualControl = true; // Enable manual control
      bluetooth.println("LED turned on manually");
    } else if (command == 'f') { // Turn LED off manually
      digitalWrite(ledPin, HIGH);
      ledManualControl = true; // Enable manual control
      bluetooth.println("LED turned off manually");
    } else if (command == 'a') { // Auto control
      ledManualControl = false; // Disable manual control
      bluetooth.println("LED auto control enabled");
    }
  }
}
