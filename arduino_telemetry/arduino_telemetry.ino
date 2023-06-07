#include <Adafruit_GFX.h>    // Include Adafruit graphics library
#include <Adafruit_ST7735.h> // Include Adafruit ST7735 TFT library
#include <SPI.h>

#define TFT_CLK 13  // Set TFT_CLK to digital pin 13
#define TFT_MISO 12 // Set TFT_MISO to digital pin 12
#define TFT_MOSI 11 // Set TFT_MOSI to digital pin 11
#define TFT_CS   10 // Set TFT_CS to digital pin 10
#define TFT_RST  9  // Set TFT_RST to digital pin 9
#define TFT_DC   8  // Set TFT_DC to digital pin 8

#define TFT_WIDTH  128
#define TFT_HEIGHT 160
#define TFT_CENTER_X TFT_WIDTH / 2
#define TFT_CENTER_Y (TFT_HEIGHT / 2) - 10 // Move tachometer up by 20 pixels
#define TFT_MIN_X 10
#define TFT_MIN_Y 10
#define TFT_MAX_X TFT_WIDTH - 10
#define TFT_MAX_Y TFT_HEIGHT - 10

#define REDLINE_RPM 6000
#define MAX_RPM 8000

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 60, ST7735_WHITE); // Draw tachometer circle
  tft.setTextWrap(false); // Disable text wrapping

  // Draw tick marks and labels
  for (int rpm = 0; rpm <= MAX_RPM; rpm += 1000) {
    float angle = map(rpm, MAX_RPM, 0, 135, -135) - 90; // Map RPM to angle
    float radian_angle = angle * PI / 180.0;

    int start_x = TFT_CENTER_X + 60 * cos(radian_angle);
    int start_y = TFT_CENTER_Y + 60 * sin(radian_angle);

    int end_x = TFT_CENTER_X + 55 * cos(radian_angle);
    int end_y = TFT_CENTER_Y + 55 * sin(radian_angle);

    tft.drawLine(start_x, start_y, end_x, end_y, rpm > REDLINE_RPM ? ST7735_RED : ST7735_WHITE);

    int text_x = TFT_CENTER_X + 48 * cos(radian_angle);
    int text_y = TFT_CENTER_Y + 48 * sin(radian_angle);

    // Draw number for tick mark
    tft.setCursor(text_x, text_y);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.print(rpm / 1000);
  }
}

void loop() {
  while (Serial.available()) {
    int engine_rpm = Serial.parseInt();

    // Calculate needle position
    float angle = map(engine_rpm, MAX_RPM, 0, 135, -135) - 90; // Map RPM to angle
    float radian_angle = angle * PI / 180.0;
    int needle_length = 50;
    int needle_x = TFT_CENTER_X + needle_length * cos(radian_angle);
    int needle_y = TFT_CENTER_Y + needle_length * sin(radian_angle);

    // Draw needle
    tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, needle_x, needle_y, ST7735_RED);
    delay(100); // Wait to see the needle
    tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, needle_x, needle_y, ST7735_BLACK); // Erase the needle
  }
}
