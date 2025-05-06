

import socket

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
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.ip, self.port))
            print(f"✅ Connected to {self.ip}:{self.port}")

            info = self.__send_raw(b"\xF3\x00\x00\x00\x00\x00\x00\x00")
            if info[0] != 0xF3:
                print(f"❌ Illegal Connection. Header missmatch: {info}")
                self.sock = None
        except Exception as e:
            print(f"❌ Connection failed: {e}")
            self.sock = None

    def __send_raw(self, data: bytes):
        if self.sock is None:
            print("⚠️ No connection. Please call connect() first.")
            return

        if len(data) != 8:
            print("❌ Unexpected data length. Please provide data with length of 8 bytes.")
            return

        try:
            self.sock.sendall(data)
            return self.sock.recv(1024)
        except Exception as e:
            print(f"❌ sending failed: {e}")

    def drive_forward(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([1, 0, speed, 0, 0, 0, 0, 0]))

    def drive_backwards(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([1, 60, speed, 0, 0, 0, 0, 0]))
    def drive_curve(self, angle, speed):
        if -180 <= angle <= 180 and 0 <= speed <= 100:
            self.__send_raw(bytes([1, angle // 3, speed, 0, 0, 0, 0, 0]))

    def drive_stop(self):
        self.__send_raw(bytes([1, 0, 0, 0, 0, 0, 0, 0]))

    def drive_imu_straight_forward(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([5, 0x7, 0x80 | speed, 0, 0, 0, 0, 0]))

    def drive_imu_straight_backward(self, speed):
        if 0 <= speed <= 100:
            self.__send_raw(bytes([5, 0x7, 0x00 | speed, 0, 0, 0, 0, 0]))

    def test(self):
        if self.test_signal:
            self.__send_raw(bytes([31, 0, 0, 0, 0, 0, 0, 0]))
            self.test_signal = False
        else:
            self.__send_raw(bytes([31, 1, 0, 0, 0, 0, 0, 0]))
            self.test_signal = True

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

    def buzzer(self, frequency):
        self.__send_raw(bytes([6, 0, 0, 0, frequency & 0xFF, frequency >> 8 & 0xFF, 0, 0]))