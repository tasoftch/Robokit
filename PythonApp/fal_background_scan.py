import robokit as RK
import stop_watch as SW

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
Robokit.connect()

StopWatch = SW.Stopwatch()

# Follow A Line threshold auf 60% einstellen
Robokit.param_set(1, 60)

StopWatch.start()
cal_done = Robokit.fal_calibrate(30, 5000, True)
StopWatch.stop()

StopWatch.wait(1)
Robokit.drive_backwards(30)
StopWatch.wait( StopWatch.result )

if not cal_done:
    print("Calibration failed.")
    exit(1)

Robokit.drive_forward(22)
Robokit.fal_enable()

farben_mapping = ['SW', 'BL', 'GR', 'CY', 'RD', 'MA', 'YE', 'WE']

StopWatch.start()

for e in range(65):
    StopWatch.wait(0.1)
    Farben = Robokit.fal_read_current_result()
    if Farben:
        print(f"{farben_mapping[Farben['left']]} {farben_mapping[Farben['mleft']]} {farben_mapping[Farben['middle']]} {farben_mapping[Farben['mright']]} {farben_mapping[Farben['right']]}")
    else:
        print(Farben)

dauer = StopWatch.stop()
Robokit.drive_stop()
StopWatch.wait(1)
Robokit.drive_backwards(22)
StopWatch.wait( StopWatch.result )
Robokit.drive_stop()
