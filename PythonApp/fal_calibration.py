
import robokit as RK
import time
import csv
import os
from datetime import datetime

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
Robokit.connect()

output_folder = "/Users/thomas/Library/CloudStorage/OneDrive-FHNW/Privat/pro4E/60 Verification and Validation/Follow A Line Sensor/Fal Report/"
os.makedirs(output_folder, exist_ok=True)

for e in range(65):
    #Robokit.drive_forward(20)
    time.sleep(0.1)
    #Robokit.drive_stop()
    Farben = Robokit.fal_one_shot()

    # Zeitstempel für Dateinamen oder Index verwenden
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    filename = f"fal_{timestamp}.csv"
    filepath = os.path.join(output_folder, filename)

    # CSV schreiben
    with open(filepath, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["sensor", "red", "green", "blue"])  # Header
        for sensor, color_values in Farben.items():
            row = [sensor, color_values["red"], color_values["green"], color_values["blue"]]
            writer.writerow(row)

    print(f"Gespeichert: {filename}")