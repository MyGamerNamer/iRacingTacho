import tkinter as tk
import math
from PIL import Image, ImageDraw, ImageTk

# Tachometer characteristics
redline_rpm = 6000  # RPM where redline starts
max_rpm = 7000  # Maximum RPM gauge can display
current_rpm = 3000  # Current engine RPM

# Create a new Tkinter window
window = tk.Tk()

# Create a canvas to draw on
canvas = tk.Canvas(window, width=500, height=500)
canvas.pack()

# Function to calculate needle end point
def calculate_needle_position(rpm, offset=200):
    # Convert the rpm to an angle (7 o'clock is 210 degrees, 5 o'clock is 330 degrees)
    angle = ((rpm / max_rpm) * 120) - 30

    # Convert angle to radians
    radian_angle = math.radians(angle)

    # Calculate the end point of the needle
    end_x = 250 + offset * math.cos(radian_angle)
    end_y = 250 + offset * math.sin(radian_angle)

    return end_x, end_y

# Create a new image with PIL
image = Image.new("RGB", (500, 500))
draw = ImageDraw.Draw(image)

# Draw a circle for the tachometer
draw.ellipse((50, 50, 450, 450), outline="white")

# Draw tick marks
for rpm in range(0, max_rpm + 1, 1000):
    # start_x, start_y = calculate_needle_position(rpm)
    # end_x, end_y = calculate_needle_position(rpm, 220)
    # draw.line((start_x, start_y, end_x, end_y), fill="white")

    # Draw extra ticks reflected over a line tangent to the circle
    start_x, start_y = calculate_needle_position(rpm, 180)
    end_x, end_y = calculate_needle_position(rpm, 160)
    draw.line((start_x, start_y, end_x, end_y), fill="white")

# Draw min and max RPM labels
draw.text(calculate_needle_position(0, 240), "0", fill="white")
draw.text(calculate_needle_position(max_rpm, 240), str(max_rpm), fill="white")

# Calculate needle position
needle_x, needle_y = calculate_needle_position(current_rpm)

# Draw a line for the needle
draw.line((250, 250, needle_x, needle_y), fill="red", width=2)

# Save the image to a file (you can remove this line if you don't need to save the image)
image.save("tachometer.png")

# Convert PIL Image to Tkinter PhotoImage and add it to the canvas
photo = ImageTk.PhotoImage(image)
canvas.create_image(0, 0, image=photo, anchor=tk.NW)

# Run the Tkinter event loop
window.mainloop()