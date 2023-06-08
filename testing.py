import serial
import serial.tools.list_ports

def find_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "ACM" in p.device or "USB" in p.device:
            return p.device
    return None

if __name__ == "__main__":
    port = find_port()
    if port is None:
        print("No suitable serial port found.")
    else:
        # Open the serial port. Replace 9600 with your baud rate if different
        ser = serial.Serial(port, 9600)
        print("Opened port", port)

        # Main loop
        while True:
            try:
                # Read data from the serial port
                data = ser.readline()

                # Decode the data and remove trailing white spaces
                data = data.decode().strip()

                # Print the data
                print(data)

            except serial.SerialException:
                print("Connection lost... Attempting to reconnect.")
                ser = serial.Serial(port, 9600)
