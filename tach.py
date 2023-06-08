import serial.tools.list_ports
from PIL import Image, ImageDraw
import pygame
import math
import serial
import time

import math
import time
import serial
from io import BytesIO

ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)
if ports:
    port = ports[0].device
else:
    print("No serial ports available.")
    exit(1)

ser = serial.Serial(port, 9600)

# Other part of your code


# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)

# Screen dimensions
TFT_WIDTH = 128
TFT_HEIGHT = 160
TFT_CENTER_X = TFT_WIDTH // 2
TFT_CENTER_Y = TFT_HEIGHT // 2 - 14

prev_engine_rpm = -1
prev_redline_rpm = -1
prev_max_rpm = -1
prev_needle_x = 0
prev_needle_y = 0
flashState = False
lastUpdateTime = 0

# ser = serial.Serial('/dev/ttyACM0', 9600)

def setup():
    global screen
    pygame.init()
    size = [TFT_WIDTH, TFT_HEIGHT]
    screen = pygame.display.set_mode(size)
    pygame.display.set_caption("RPM Gauge")

def drawTicks(redline_rpm, max_rpm, rev_limiter_engaged):
    global flashState, lastUpdateTime
    if rev_limiter_engaged and (time.time() - lastUpdateTime) > 0.35:
        flashState = not flashState
        lastUpdateTime = time.time()
    elif not rev_limiter_engaged:
        flashState = False

    TEST = RED if flashState else WHITE

    pygame.draw.circle(screen, TEST, (TFT_CENTER_X, TFT_CENTER_Y), 60)
    for rpm in range(0, max_rpm + 1, 1000):
        angle = math.radians(map_values(rpm, max_rpm, 0, 135, -135) - 90)
        start_x = TFT_CENTER_X + 60 * math.cos(angle)
        start_y = TFT_CENTER_Y + 60 * math.sin(angle)
        end_x = TFT_CENTER_X + 55 * math.cos(angle)
        end_y = TFT_CENTER_Y + 55 * math.sin(angle)

        color = RED if rpm >= redline_rpm else TEST
        pygame.draw.line(screen, color, (start_x, start_y), (end_x, end_y), 1)

def drawLabels(redline_rpm, max_rpm):
    global prev_redline_rpm, prev_max_rpm
    prev_redline_rpm = redline_rpm
    prev_max_rpm = max_rpm

    font = pygame.font.Font(None, 25)
    for rpm in range(0, max_rpm + 1, 1000):
        angle = math.radians(map_values(rpm, max_rpm, 0, 135, -135) - 90)
        text_x = TFT_CENTER_X + 48 * math.cos(angle)
        text_y = TFT_CENTER_Y + 48 * math.sin(angle)
        color = RED if rpm > redline_rpm else WHITE
        text = font.render(str(rpm / 1000), True, color)
        screen.blit(text, (text_x, text_y))

def drawNeedle(engine_rpm, max_rpm):
    global prev_engine_rpm, prev_needle_x, prev_needle_y
    if engine_rpm != prev_engine_rpm:
        # Erase the old needle by drawing over it with black color
        pygame.draw.line(screen, BLACK, (TFT_CENTER_X, TFT_CENTER_Y), (prev_needle_x, prev_needle_y), 2)
        # Draw the new needle
        angle = math.radians(map_values(engine_rpm, max_rpm, 0, 135, -135) - 90)
        needle_length = 50
        needle_x = TFT_CENTER_X + needle_length * math.cos(angle)
        needle_y = TFT_CENTER_Y + needle_length * math.sin(angle)
        pygame.draw.line(screen, RED, (TFT_CENTER_X, TFT_CENTER_Y), (needle_x, needle_y), 2)
        # Remember the new needle position for next time
        prev_needle_x = needle_x
        prev_needle_y = needle_y
        prev_engine_rpm = engine_rpm

def map_values(value, in_min, in_max, out_min, out_max):
    # Equivalent of Arduino's map function
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def main():
    setup()
    while True:
        if ser.in_waiting:
            data = ser.readline().decode().strip()
            engine_rpm, redline_rpm, rev_limiter_engaged = data.split(',')
            engine_rpm = int(engine_rpm)
            redline_rpm = int(redline_rpm)
            rev_limiter_engaged = rev_limiter_engaged == "True"
            max_rpm = math.ceil((redline_rpm + 2000) / 1000) * 1000
            drawTicks(redline_rpm, max_rpm, rev_limiter_engaged)
            drawNeedle(engine_rpm, max_rpm)
            drawLabels(redline_rpm, max_rpm)
            pygame.display.flip()

if __name__ == "__main__":
    main()
