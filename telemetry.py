import tkinter as tk
import math
from PIL import Image, ImageDraw, ImageTk, ImageFont
import irsdk
import time
import serial
import serial.tools.list_ports

class EngineMonitor:
    def __init__(self):
        self.sdk = irsdk.IRSDK()
        self.serial_port = self.find_arduino_port()

        # Create a new Tkinter window
        self.window = tk.Tk()

        # Create a canvas to draw on
        self.canvas = tk.Canvas(self.window, width=500, height=500)
        self.canvas.pack()

    def find_arduino_port(self):
        ports = list(serial.tools.list_ports.comports())
        for p in ports:
            # if p.vid == 0x2341 and p.pid == 0x0042:  # replace with your board's VID and PID
            if p.vid == 0x2341 and p.pid == 0x8057:  # VID and PID for Arduino Nano 33 IoT
                return p.device
        return None

    def get_engine_data(self):
        if not self.sdk.startup():
            print('Could not start communication with iRacing')
            return

        while True:
            try:
                if self.serial_port is None:
                    print("No Arduino found")
                    time.sleep(1)
                    self.serial_port = self.find_arduino_port()
                    continue
                else:
                    ser = serial.Serial(self.serial_port, 9600)  # replace 9600 with your baud rate if different
                    time.sleep(1)  # waiting for the initialization...

                while True:
                    redline_rpm = self.sdk['DriverInfo']['DriverCarSLBlinkRPM']
                    engine_rpm = int(self.sdk['RPM'])
                    is_rev_limiter_engaged = engine_rpm >= redline_rpm

                    # Send data to Arduino
                    data_to_send = f"Engine RPM: {engine_rpm}, Rev Limiter Engaged: {is_rev_limiter_engaged}\n"
                    ser.write(data_to_send.encode())
                    ser.flush()

                    # Generate new tachometer image with current engine RPM
                    self.draw_tachometer(engine_rpm, redline_rpm)

                    # time.sleep(0.01)  # Delay for stability

            except serial.SerialException:
                print("Connection lost... Attempting to reconnect.")
                time.sleep(1)
                self.serial_port = self.find_arduino_port()
                continue

    def calculate_needle_position(self, rpm, max_rpm, offset=200):
        # Convert the rpm to an angle (6 o'clock is 270 degrees, 3 o'clock is 0 degrees)
        angle = ((rpm / max_rpm) * 300) + 150

        # Convert angle to radians
        radian_angle = math.radians(angle)

        # Calculate the end point of the needle
        end_x = 250 + offset * math.cos(radian_angle)
        end_y = 250 + offset * math.sin(radian_angle)

        return end_x, end_y

    def draw_tachometer(self, current_rpm, redline_rpm):
        # Create a new image with PIL
        image = Image.new("RGB", (500, 500))
        draw = ImageDraw.Draw(image)

        # Create font object
        font = ImageFont.truetype("arial", 12)

        # Draw a circle for the tachometer
        draw.ellipse((50, 50, 450, 450), outline="white")

        # Calculate max RPM for gauge scale, round up to the nearest thousand
        max_rpm = math.ceil((redline_rpm + 2000) / 1000) * 1000

        # Draw tick marks and labels
        for rpm in range(0, max_rpm + 1, 1000):
            start_x, start_y = self.calculate_needle_position(rpm, max_rpm, 200)
            end_x, end_y = self.calculate_needle_position(rpm, max_rpm, 175)
            draw.line((start_x, start_y, end_x, end_y), fill="red" if rpm > redline_rpm else "white")
            label_x, label_y = self.calculate_needle_position(rpm, max_rpm, 165)  # adjust offset here
            draw.text((label_x, label_y), str(rpm), fill="white", font=font)

        # Draw shorter tick marks
        for rpm in range(0, max_rpm + 1, 250):  # adjust this value for different tick spacing
            if rpm % 1000 == 0:  # we already drew these ticks
                continue
            start_x, start_y = self.calculate_needle_position(rpm, max_rpm, 200)
            end_x, end_y = self.calculate_needle_position(rpm, max_rpm, 187.5)  # adjust this value for different tick length
            draw.line((start_x, start_y, end_x, end_y), fill="red" if rpm > redline_rpm else "white")

        # Calculate needle position
        needle_x, needle_y = self.calculate_needle_position(current_rpm, max_rpm)

        # Draw a line for the needle
        draw.line((250, 250, needle_x, needle_y), fill="red", width=2)

        # Convert PIL Image to Tkinter PhotoImage and add it to the canvas
        photo = ImageTk.PhotoImage(image)
        self.canvas.create_image(0, 0, image=photo, anchor=tk.NW)

        # Update the Tkinter window
        self.window.update()



if __name__ == "__main__":
    monitor = EngineMonitor()
    monitor.get_engine_data()
