#include <Adafruit_GFX.h>    // Include Adafruit graphics library
#include <Adafruit_ST7735.h> // Include Adafruit ST7735 TFT library
#include <SPI.h>

#define TFT_CLK 13  // Set TFT_CLK to digital pin 13
#define TFT_MISO 12 // Set TFT_MISO to digital pin 12
#define TFT_MOSI 11 // Set TFT_MOSI to digital pin 11
#define TFT_CS   10 // Set TFT_CS to digital pin 10
#define TFT_RST  9  // Set TFT_RST to digital pin 9
#define TFT_DC   8  // Set TFT_DC to digital pin 8

#define MAX_BUFFER 64 // maximum size of the expected message

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

int prev_engine_rpm = -1;
String prev_rev_limiter_status = "";

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_GREENTAB); // Initialize TFT display
  tft.fillScreen(ST7735_BLACK); // Clear screen

  // Initialize the text on the screen
  tft.setCursor(40, 20); // Move cursor higher
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(3);
  tft.println("RPM: ");
}

void loop() {
  while (Serial.available()) {
    char receivedData[MAX_BUFFER];
    int size = Serial.readBytesUntil('\n', receivedData, sizeof(receivedData) - 1);

    if (size > 0) {
      receivedData[size] = '\0'; // null terminate the C string

      String strReceivedData = String(receivedData);
      strReceivedData.trim();

      int colonIndex1 = strReceivedData.indexOf(':');
      int commaIndex = strReceivedData.indexOf(',');
      int colonIndex2 = strReceivedData.lastIndexOf(':');

      String rpmString = strReceivedData.substring(colonIndex1 + 1, commaIndex);
      int engine_rpm = rpmString.toInt();

      if (engine_rpm != prev_engine_rpm) {
        tft.fillRect(40, 50, 100, 20, ST7735_BLACK); // clear RPM text
        tft.setCursor(40, 50); // Move cursor higher
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(3);
        tft.println(engine_rpm);
        prev_engine_rpm = engine_rpm;
      }

      String revLimiterStatus = strReceivedData.substring(colonIndex2 + 1);
      revLimiterStatus.trim();

      if (revLimiterStatus != prev_rev_limiter_status) {
        tft.fillRect(0, 90, 128, 68, ST7735_BLACK); // clear rev limiter text
        tft.setCursor(10, 100); // Adjust position for "REV LIMITER"
        tft.setTextSize(1.8);
        tft.println("REV LIMITER");
        if (revLimiterStatus == "True") {
          tft.fillRect(0, 90, 128, 68, ST7735_RED); // Fill bottom half of the screen with red
          tft.setCursor(10, 100); // Adjust position for "REV LIMITER"
          tft.setTextColor(ST7735_WHITE);
          tft.println("REV LIMITER");
        }
        prev_rev_limiter_status = revLimiterStatus;
      }
    }
  }
}
