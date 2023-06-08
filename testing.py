import serial
import serial.tools.list_ports
import time

def find_host_system_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        # Assuming the host system shows up as a generic USB or ACM port
        if "ACM" in p.device or "USB" in p.device:
            return p.device
    return None

# This function opens the serial port and listens for incoming data
def read_serial_data(serial_port):
    ser = serial.Serial(serial_port, 9600, timeout=0)  # replace 9600 with your baud rate if different
    while True:
        try:
            data = ser.readline().decode('utf-8').strip()  # read data from serial
            # port and strip line endings
            if data:
                print(data)
        except KeyboardInterrupt:
            print("Interrupted, closing...")
            break
        except serial.SerialException:
            print("Serial connection lost. Reconnecting...")
            time.sleep(1)
            continue

if __name__ == "__main__":
    host_port = find_host_system_port()
    if host_port:
        print(f"Found host system on port {host_port}")
        read_serial_data(host_port)
    else:
        print("No suitable serial port found.")
