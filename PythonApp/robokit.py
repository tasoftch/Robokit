

import socket
import struct
import time

# IP und Port des ESP32
ESP32_IP = "192.168.4.1"
ESP32_PORT = 5210

class Robokit(object):
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port
        self.sock = None
        self.test_signal = False

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(1.0)
        self.sock.connect((self.ip, self.port))
        print(f"Connected to {self.ip}:{self.port}")

        info = self.__send_raw(b"\xF3\x00\x00\x00\x00\x00\x00\x00")
        if info[0] != 0xF3:
            print(f" Illegal Connection. Header missmatch: {info}")
            self.sock = None

    def __send_raw(self, data: bytes):
        if self.sock is None:
            print("No connection. Please call connect() first.")
            return

        if len(data) != 8:
            print(" Unexpected data length. Please provide data with length of 8 bytes.")
            return

        try:
            self.sock.sendall(data)
            return self.sock.recv(1024)
        except Exception as e:
            print(f" sending failed: {e}")

# Fahren
    def drive_forward(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([1, 0, speed, 0, 0, 0, 0, 0]))

    def drive_backwards(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([1, 60, speed, 0, 0, 0, 0, 0]))
    def drive_curve(self, angle, speed):
        if -180 <= angle <= 180 and 0 <= speed <= 100:
            self.__send_raw(bytes([1, struct.pack("b", angle // 3)[0], speed, 0, 0, 0, 0, 0]))

    def drive_stop(self):
        self.__send_raw(bytes([1, 0, 0, 0, 0, 0, 0, 0]))

# IMU Konzept:
# Die IMU muss laufen (enable) wenn:
# - Nach Winkel gefahren werden will.
# - über status_imu_read Werte verlangt werden

    def imu_enable(self):
        self.__send_raw(bytes([5, 0x1, 0, 0, 0, 0, 0, 0]))

    def imu_disable(self):
        self.__send_raw(bytes([5, 0x0, 0, 0, 0, 0, 0, 0]))

    def drive_imu_straight_forward(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([5, 0x7, 0x1 | speed<<1, 0, 0, 0, 0, 0]))

    def drive_imu_reset_orientation(self):
        self.__send_raw(bytes([5, 0x3, 0, 0, 0, 0, 0, 0]))

    def drive_imu_orientation_deviate(self, deviation):
        if -100 <= deviation <= 100:
            self.__send_raw(bytes([5, 0x13, 0, struct.pack("b", deviation)[0], 0, 0, 0, 0]))

    def status_imu_read(self):
        self.imu_enable()
        info = self.__send_raw(bytes([0xF4, 0, 0, 0, 0, 0, 0, 0]))
        return {
            "position": struct.unpack('>h', bytes([info[0], info[1]]))[0] / 16.0,
            "orientation": struct.unpack('>h', bytes([info[2], info[3]]))[0] / 16.0,
            "deviation": struct.unpack('>h', bytes([info[4], info[5]]))[0] / 16.0,
        }

# Follow A Line
    def fal_calibrate(self):
        self.__send_raw(bytes([4, 0x1, 0, 0, 0, 0, 0, 0]))

    def fal_start(self, speed):
        self.__send_raw(bytes([4, 0x2, speed % 101, 0, 0, 0, 0, 0]))

    def fal_one_shot(self):
        self.__send_raw(bytes([4, 0x3, 0, 0, 0, 0, 0, 0]))

        for e in range(4):
            time.sleep(0.1)
            info = self.__send_raw(bytes([0xF6, 0, 0, 0, 0, 0, 0, 0]))
            if info[0] == 0x02:
                return {
                    "left": {
                        "red":   info[1] << 8 | info[2],
                        "green": info[3] << 8 | info[4],
                        "blue":  info[5] << 8 | info[6],
                    },
                    "mleft": {
                        "red":   info[7] << 8 | info[8],
                        "green": info[9] << 8 | info[10],
                        "blue":  info[11] << 8 | info[12],
                    },
                    "middle": {
                        "red":   info[13] << 8 | info[14],
                        "green": info[15] << 8 | info[16],
                        "blue":  info[17] << 8 | info[18],
                    },
                    "mright": {
                        "red":   info[19] << 8 | info[20],
                        "green": info[21] << 8 | info[22],
                        "blue":  info[23] << 8 | info[24],
                    },
                    "right": {
                        "red":   info[25] << 8 | info[26],
                        "green": info[27] << 8 | info[28],
                        "blue":  info[29] << 8 | info[30],
                    },
                }
        return False

# LEDS
    def led_setup(self, led_number, red, green, blue):
        if 0 <= led_number <= 31:
            self.leds_setup([led_number], red, green, blue)

    def leds_setup(self, leds, red, green, blue):
        TheLEDs = 0
        for led in leds:
            if 0 <= led <= 31:
                TheLEDs |= 1 << led
        self.__send_raw(bytes([2, red, green, blue, TheLEDs  & 0xFF, (TheLEDs >> 8) & 0xFF, (TheLEDs >> 16) & 0xFF, (TheLEDs >> 24) & 0xFF]))

    def led_flush(self):
        self.__send_raw(bytes([3, 0, 0, 0, 0, 0, 0, 0]))

    def led_clear(self):
        self.__send_raw(bytes([3, 1, 0, 0, 0, 0, 0, 0]))

# Buzzer, Hupe und Töne
    def buzzer(self, frequency):
        self.__send_raw(bytes([6, 0, 0, 0, frequency & 0xFF, frequency >> 8 & 0xFF, 0, 0]))

# Ultraschall Sensor
    def approximate(self, distance_in_cm):
        if distance_in_cm > 400:
            distance_in_cm = 400
        if distance_in_cm < 5:
            distance_in_cm = 5
        self.__send_raw(bytes([7, 0, distance_in_cm & 0xFF, distance_in_cm >> 8 & 0xFF, 0, 0, 0, 0]))



    def status_battery(self):
        info = self.__send_raw(bytes([0xF5, 0, 0, 0, 0, 0, 0, 0]))
        return {
            # Voltage in Volts
            "voltage": (info[0]<<8 | info[1]) / 1000,
            # Charge in percent
            "charge": info[2] / 100.0,
            # Status:
            # 0: Emergency (Roboter does not respond to anything.
            # 1: Critical
            # 2: Warning
            # 3: OK
            "status": info[3],
        }

    def status_drive_vector(self):
        info = self.__send_raw(bytes([0xF0, 0, 0, 0, 0, 0, 0, 0]))
        angle = struct.unpack('b', bytes([info[0]]))[0]
        return {
            "angle": angle * 3,
            "speed": info[1],
        }

    def status_distance(self):
        info = self.__send_raw(bytes([0xF1, 0, 0, 0, 0, 0, 0, 0]))
        return info[0] << 8 | info[1]
