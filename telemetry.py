import irsdk
import time
import serial
import serial.tools.list_ports

class EngineMonitor:
    def __init__(self):
        self.sdk = irsdk.IRSDK()
        self.serial_port = self.find_arduino_port()

    def find_arduino_port(self):
        ports = list(serial.tools.list_ports.comports())
        for p in ports:
            if p.vid == 0x2341 and p.pid == 0x0042:  # replace with your board's VID and PID
                return p.device
        return None

    def get_engine_data(self):
        if not self.sdk.startup():
            print('Could not start communication with iRacing')
            return

        if self.serial_port is None:
            print("No Arduino found")
            return
        else:
            ser = serial.Serial(self.serial_port, 9600)  # replace 9600 with your baud rate if different
            # ser = serial.Serial("COM4", 9600)  # replace 9600 with your baud rate if different
            time.sleep(1)  # waiting for the initialization...
            # print(ser.readline().decode().strip())  # print the initialization message from Arduino

        while True:
            redline_rpm = self.sdk['DriverInfo']['DriverCarSLBlinkRPM']
            engine_rpm = int(self.sdk['RPM'])
            is_rev_limiter_engaged = engine_rpm >= redline_rpm

            # print(f"Current Engine Speed: {engine_rpm}, Rev Limiter Engaged: {is_rev_limiter_engaged}")
            is_rev_limiter_engaged = engine_rpm >= redline_rpm

            # print(f"Current Engine Speed: {engine_rpm}, Rev Limiter Engaged: {is_rev_limiter_engaged}")

            # Send data to Arduino
            data_to_send = f"Engine RPM: {engine_rpm}, Rev Limiter Engaged: {is_rev_limiter_engaged}\n"
            ser.write(data_to_send.encode())
            ser.flush()

            # time.sleep(0.01)  # Delay for stability

if __name__ == "__main__":
    monitor = EngineMonitor()
    monitor.get_engine_data()
