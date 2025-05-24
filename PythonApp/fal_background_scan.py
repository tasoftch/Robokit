import robokit as RK
import time

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
Robokit.connect()

Robokit.fal_calibrate(20, 5000)

time.sleep(2)

Robokit.drive_backwards(30)
time.sleep(1)

Robokit.fal_drive(20, 5000)


for e in range(65):
    time.sleep(0.1)
    Farben = Robokit.fal_read_current_result()
    print(Farben)

Robokit.drive_stop()