#include <Adafruit_GFX.h>    // Include Adafruit graphics library
#include <Adafruit_ST7735.h> // Include Adafruit ST7735 TFT library
#include <SPI.h>

#define TFT_CLK 13  // Set TFT_CLK to digital pin 13
#define TFT_MISO 12 // Set TFT_MISO to digital pin 12
#define TFT_MOSI 11 // Set TFT_MOSI to digital pin 11
#define TFT_CS   10 // Set TFT_CS to digital pin 10
#define TFT_RST  9  // Set TFT_RST to digital pin 9
#define TFT_DC   8  // Set TFT_DC to digital pin 8

#define LED_PIN  7  // LED on pin 7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB); // Initialize TFT display
  tft.fillScreen(ST7735_BLACK); // Clear screen

  pinMode(LED_PIN, OUTPUT);
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

    tft.fillScreen(ST7735_BLACK); // Clear screen

    tft.setCursor(20, 50); // Adjust these values to center text
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);  // Increase text size
    tft.print("RPM: ");
    tft.println(engine_rpm);

    tft.setCursor(20, 70); // Adjust these values to center text
    if (is_rev_limiter_engaged) {
      tft.println("Rev Limiter: ON");
      digitalWrite(LED_PIN, HIGH);
    } else {
      tft.println("Rev Limiter: OFF");
      digitalWrite(LED_PIN, LOW);
    }
  }
}
