import robokit as RK
import time

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
Robokit.connect()

if Robokit.fal_calibration_status() != 2:
    Robokit.fal_calibrate(20, 5000)
    time.sleep(2)
    Robokit.drive_backwards(30)
    time.sleep(1)

Robokit.drive_forward(20)
Robokit.fal_enable()

farben_mapping = ['SW', 'BL', 'GR', 'CY', 'RD', 'MA', 'YE', 'WE']

start_zeit = time.perf_counter()

for e in range(65):
    time.sleep(0.1)
    Farben = Robokit.fal_read_current_result()
    if Farben:
        print(f"{farben_mapping[Farben['left']]} {farben_mapping[Farben['mleft']]} {farben_mapping[Farben['middle']]} {farben_mapping[Farben['mright']]} {farben_mapping[Farben['right']]}")
    else:
        print(Farben)

end_zeit = time.perf_counter()
Robokit.drive_stop()
time.sleep(1)
Robokit.drive_backwards(20)
time.sleep(end_zeit-start_zeit)
Robokit.drive_stop()
