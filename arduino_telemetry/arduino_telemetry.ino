#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#define TFT_WIDTH 128
#define TFT_HEIGHT 160
#define TFT_CENTER_X TFT_WIDTH / 2
#define TFT_CENTER_Y (TFT_HEIGHT / 2) - 14

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int prev_engine_rpm = -1;
int prev_redline_rpm = -1;
int prev_max_rpm = -1;
int prev_needle_x = 0;
int prev_needle_y = 0;

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_GREENTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 60, ST7735_WHITE);
  tft.setTextWrap(false);
}

void drawTicksAndLabels(int redline_rpm, int max_rpm) {
  if (redline_rpm != prev_redline_rpm || max_rpm != prev_max_rpm) {
    tft.fillScreen(ST7735_BLACK);
    tft.drawCircle(TFT_CENTER_X, TFT_CENTER_Y, 60, ST7735_WHITE);
    prev_redline_rpm = redline_rpm;
    prev_max_rpm = max_rpm;

    for (int rpm = 0; rpm <= max_rpm; rpm += 1000) {
      float angle = map(rpm, max_rpm, 0, 135, -135) - 90;
      float radian_angle = angle * PI / 180.0;

      int start_x = TFT_CENTER_X + 60 * cos(radian_angle);
      int start_y = TFT_CENTER_Y + 60 * sin(radian_angle);

      int end_x = TFT_CENTER_X + 55 * cos(radian_angle);
      int end_y = TFT_CENTER_Y + 55 * sin(radian_angle);

      tft.drawLine(start_x, start_y, end_x, end_y, rpm > redline_rpm ? ST7735_RED : ST7735_WHITE);

      int text_x = TFT_CENTER_X + 48 * cos(radian_angle);
      int text_y = TFT_CENTER_Y + 48 * sin(radian_angle);

      tft.setCursor(text_x, text_y);
      tft.setTextColor(rpm > redline_rpm ? ST7735_RED : ST7735_WHITE);
      tft.setTextSize(1);
      tft.print(rpm / 1000);
    }
  }
}

void drawNeedle(int engine_rpm, int max_rpm) {
  if (engine_rpm != prev_engine_rpm) {
    // Erase the old needle by drawing over it with black color
    tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, prev_needle_x, prev_needle_y, ST7735_BLACK);

    // Draw the new needle
    float angle = map(engine_rpm, max_rpm, 0, 135, -135) - 90;
    float radian_angle = angle * PI / 180.0;
    int needle_length = 50;
    int needle_x = TFT_CENTER_X + needle_length * cos(radian_angle);
    int needle_y = TFT_CENTER_Y + needle_length * sin(radian_angle);

    tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, needle_x, needle_y, ST7735_RED);

    // Remember the new needle position for next time
    prev_needle_x = needle_x;
    prev_needle_y = needle_y;
    prev_engine_rpm = engine_rpm;
  }
}

void loop() {
  while (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    int firstCommaIndex = data.indexOf(",");
    String engine_rpm_string = data.substring(0, firstCommaIndex);
    int engine_rpm = engine_rpm_string.toInt();
    String redline_rpm_string = data.substring(firstCommaIndex+1, data.indexOf(",", firstCommaIndex+1));
    int redline_rpm = redline_rpm_string.toInt();
    int max_rpm = ceil((redline_rpm + 2000) / 1000) * 1000;

    drawTicksAndLabels(redline_rpm, max_rpm);
    drawNeedle(engine_rpm, max_rpm);
  }
}
