#include <LiquidCrystal.h>
// Initialize the library with the numbers of the interface pins
// (D7=12, D6=11, D5=10, D4=9, Enable=3, RS=2)
LiquidCrystal lcd(2, 3, 9, 10, 11, 12);

const int ledPin = 13; // LED connected to digital pin 13

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.setCursor(0, 0); // move cursor to beginning of first line
  lcd.print("TACH Initialized.");
  lcd.setCursor(0, 1); // move cursor to beginning of first line
  lcd.print("Waiting for data...");
  pinMode(ledPin, OUTPUT); // sets the digital pin as output
}

void loop() {
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim();  // remove any leading/trailing whitespace

    int colonIndex1 = receivedData.indexOf(':');  // index of the first colon
    int commaIndex = receivedData.indexOf(',');  // index of the comma
    int colonIndex2 = receivedData.lastIndexOf(':');  // index of the second colon

    String rpmString = receivedData.substring(colonIndex1 + 1, commaIndex);
    int engine_rpm = rpmString.toInt();

    String revLimiterStatus = receivedData.substring(colonIndex2 + 1);
    revLimiterStatus.trim();  // remove any leading/trailing whitespace

    bool is_rev_limiter_engaged = (revLimiterStatus == "True");

    lcd.clear();
    lcd.setCursor(0, 0);  // move cursor to beginning of first line
    lcd.print("RPM: ");
    lcd.print(engine_rpm);

    lcd.setCursor(0, 1); // move cursor to beginning of second line
    if (is_rev_limiter_engaged) {
      lcd.print("Rev Limiter: ON");
      digitalWrite(ledPin, HIGH); // turn the LED on
      delay(15); // wait for half a second
      digitalWrite(ledPin, LOW); // turn the LED off
      delay(15); // wait for half a second
    } else {
      lcd.print("Rev Limiter: OFF");
      digitalWrite(ledPin, LOW); // turn the LED off
    }
  }
}
