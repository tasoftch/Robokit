
import robokit as RK
import time
import matplotlib.pyplot as plt

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
Robokit.connect()

data = []

for e in range(65):
    #Robokit.drive_forward(20)
    time.sleep(0.1)
    #Robokit.drive_stop()
    Farben = Robokit.fal_one_shot()
    data.append(Farben)

sensors = ['left', 'mleft', 'middle', 'mright', 'right']
colors = ['red', 'green', 'blue']
color_sequence = ["white", "black", "white", "red", "white", "green", "white", "blue", "white"]

# Für jeden Farbkanal ein Plot
for color in colors:
    fig, (ax, ax_colorbar) = plt.subplots(2, 1, figsize=(10, 5), height_ratios=[4, 0.4], sharex=True)

    # Linienplot
    for sensor in sensors:
        values = [entry[sensor][color] for entry in data]
        ax.plot(values, label=sensor)
    ax.set_title(f"{color.capitalize()} values over time")
    ax.set_ylabel("Intensity")
    ax.legend()
    ax.grid(True)

    bar_width = len(data) / len(color_sequence)
    for i, col in enumerate(color_sequence):
        ax_colorbar.axvspan(i * bar_width, (i + 1) * bar_width, color=col)

    ax_colorbar.set_yticks([])
    ax_colorbar.set_xticks([])
    ax_colorbar.set_xlim(0, len(data))
    ax_colorbar.set_frame_on(False)

    plt.tight_layout()

plt.show()
