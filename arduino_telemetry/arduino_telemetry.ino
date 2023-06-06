#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 9, 10, 11, 12);

const int ledPin = 13; // LED connected to digital pin 13
const int buttonUp = 7; // Button to go to next page
const int buttonDown = 8; // Button to go to previous page

int currentPage = 0;
int totalPages = 3; // Total number of pages

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonUp, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(buttonDown, INPUT_PULLUP); // Enable internal pull-up resistor
}

void loop() {
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim();

    int colonIndex1 = receivedData.indexOf(':');
    int commaIndex = receivedData.indexOf(',');
    int colonIndex2 = receivedData.lastIndexOf(':');

    String rpmString = receivedData.substring(colonIndex1 + 1, commaIndex);
    int engine_rpm = rpmString.toInt();

    String revLimiterStatus = receivedData.substring(colonIndex2 + 1);
    revLimiterStatus.trim();

    bool is_rev_limiter_engaged = (revLimiterStatus == "True");

    lcd.clear();
    if (currentPage == 0) {
      lcd.setCursor(0, 0);
      lcd.print("RPM: ");
      lcd.print(engine_rpm);
      lcd.setCursor(0, 1);
      if (is_rev_limiter_engaged) {
        lcd.print("Rev Limiter: ON");
      } else {
        lcd.print("Rev Limiter: OFF");
      }
    } else if (currentPage == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Demo Page 2");
    } else if (currentPage == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Demo Page 3");
    }
  }

  // Check if buttons are pressed
  if (digitalRead(buttonUp) == LOW) {
    currentPage = (currentPage + 1) % totalPages;
    delay(200); // Debounce delay
  } else if (digitalRead(buttonDown) == LOW) {
    currentPage = (currentPage - 1 + totalPages) % totalPages;
    delay(200); // Debounce delay
  }
}
