
import csv
import os
import matplotlib.pyplot as plt
import pandas as pd

colors_folder = "/Users/thomas/Library/CloudStorage/OneDrive-FHNW/Privat/pro4E/60 Verification and Validation/Follow A Line Sensor/Fal Report/"
voltage_folger = "/Users/thomas/Library/CloudStorage/OneDrive-FHNW/Privat/pro4E/60 Verification and Validation/Follow A Line Sensor/Osc Report/"

data = []
results = []

# Nach Dateinamen sortieren (z. B. zeitlich korrekt)
csv_files = sorted(f for f in os.listdir(colors_folder) if f.endswith(".csv"))
spg_files = sorted(f for f in os.listdir(voltage_folger) if f.endswith(".csv"))

if len(csv_files) != len(spg_files):
    print(
        f"Error: Number of files in {colors_folder} and {voltage_folger} do not match."
    )

for filename in csv_files:
    filepath = os.path.join(colors_folder, filename)
    with open(filepath, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        frame = {}
        for row in reader:
            sensor = row['sensor']
            frame[sensor] = {
                'red': int(row['red']),
                'green': int(row['green']),
                'blue': int(row['blue'])
            }
        data.append(frame)

for filename in spg_files:
    filepath = os.path.join(voltage_folger, filename)

    try:
        # Datei einlesen, ab Zeile 16 (skiprows=15)
        df = pd.read_csv(filepath, skiprows=15, header=None)

        # Spannung = Spalte 3 (Index 2), Text ignorieren
        voltages = pd.to_numeric(df.iloc[:, 2], errors='coerce').dropna()

        # Minimum & Maximum bestimmen
        v_min = voltages.min()
        v_max = voltages.max()

        results.append([filename, round(v_min, 3), round(v_max, 3)])
    except Exception as e:
        print(f"Fehler bei Datei {filename}: {e}")

sensors = ['left', 'mleft', 'middle', 'mright', 'right']
colors = ['red', 'green', 'blue']
color_sequence = ["white", "black", "white", "red", "white", "green", "white", "blue", "white"]

# Für jeden Farbkanal ein Plot
fig, axs = plt.subplots(5, 1, figsize=(12, 10),
                        height_ratios=[3, 3, 3, 3, 0.5],
                        sharex=True)

ax = axs[0]

v_min_list = [r[1] for r in results]
v_max_list = [r[2] for r in results]

ax.plot(v_min_list, label="V_min", color='blue', marker='o')
ax.plot(v_max_list, label="V_max", color='red', marker='o')

ax.set_ylabel("Voltage (V)")
ax.set_title("Power Supply +3.3v Monitoring")
ax.legend(loc="lower right", fontsize="small")
ax.grid(True)

# Für jeden Farbkanal ein eigenes Plotfenster (ax)
for idx, color in enumerate(colors):
    ax = axs[idx+1]
    for sensor in sensors:
        values = [entry[sensor][color] for entry in data]
        ax.plot(values, label=sensor)
    ax.set_title(f"{color.capitalize()} values over time")
    ax.set_ylabel("Intensity")
    ax.legend(loc="upper right", fontsize="small")
    ax.grid(True)

# Letzter Plot: Farbbalken
ax_colorbar = axs[4]
bar_width = len(data) / len(color_sequence)
for i, col in enumerate(color_sequence):
    ax_colorbar.axvspan(i * bar_width, (i + 1) * bar_width, color=col)

ax_colorbar.set_yticks([])
ax_colorbar.set_xticks([])
ax_colorbar.set_xlim(0, len(data))
ax_colorbar.set_frame_on(False)

plt.tight_layout()
plt.show()