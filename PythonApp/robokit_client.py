
import robokit as RK

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)
old_speed = 0

def print_help():
    print("Command     | Description")
    print("------------|---------------------------------------------")
    print("            | DRIVE COMMANDS")
    print("fwd <speed> | Drive forward with specified speed <speed>")
    print("bwd <speed> | Drive backwards with specified speed <speed>")
    print("crv <angle> | Makes a curve maintaining speed before")
    print("            | <angle> in degrees of 3° from -180 to 180")
    print("stop        | Stops the drive immediately")
    print("            | ")
    print("            | IMU DRIVE COMMANDS")
    print("imu <speed> | Drive straight forward using the IMU")
    print("imud <deg_u>| Adjusts the fixed imu orientation by deg_u * 16 degrees.")
    print("            | ")
    print("            | LED AND MULTIMEDIA COMMANDS")
    print("led <n> <cl>| n is the selected led and cl the color as 0xRGB")
    print("flush       | Flushes the LED setting to the drive")
    print("clear       | Clears the LED.")
    print("buz <freq>  | Launches the buzzer with given frequency. ")
    print("            | All < 250Hz will disable.")
    print("            | ")
    print("            | FOLLOW A LINE COMMANDS")
    print("falc <speed>| Starts calibration. The device must pass the calibration sheet.")
    print("fal <speed> | Start driving straight on for 5s. If a line is detected, it will follow.")
    print("falr        | Reads current background. Only available after calibration.")
    print("            | All five sensors read one bit colors: RGB (0b000)")

    print("            | ")
    print("            | CONDITIONAL COMMANDS")
    print("dist <cm>   | Stopps if distance to any object is below given distance.")
    print("            | Example: fwd 50;dist 130")
    print("            | ")
    print("            | FETCH ACTUAL VALUES")
    print("info        | Reads device version, firmware and serial number.")
    print("bat         | Reads the battery status.")
    print("imur        | Reads IMU's current position, orientation and deviation.")
    print("vec         | Fetches the drive vector. Angle in degrees ")
    print("            | (0 straight, -... left +... right), speed in %.")
    print("abs         | Reads the current distance [cm] in front of Roboter")
    print("            | to the next obstacle. (Scattering +/- 30°")


def print_formatted_fal_colors(color_set):
    print(f"Sensor |  RD  |  GN  |  BL  |")
    print(f"-------|------|------|------|")
    print(f"LEFT   | %04d | %04d | %04d |" % (color_set["left"]["red"], color_set["left"]["green"], color_set["left"]["blue"]))
    print(f"MLEFT  | %04d | %04d | %04d |" % (color_set["mleft"]["red"], color_set["mleft"]["green"], color_set["mleft"]["blue"]))
    print(f"MIDDLE | %04d | %04d | %04d |" % (color_set["middle"]["red"], color_set["middle"]["green"], color_set["middle"]["blue"]))
    print(f"MRIGHT | %04d | %04d | %04d |" % (color_set["mright"]["red"], color_set["mright"]["green"], color_set["mright"]["blue"]))
    print(f"RIGHT  | %04d | %04d | %04d |" % (color_set["right"]["red"], color_set["right"]["green"], color_set["right"]["blue"]))


def perform_command(cmd, args):
    global old_speed, Robokit

    try:
        response = ""

        if cmd == "fwd":
            old_speed = speed = int(args[0])
            Robokit.drive_forward(speed)
        elif cmd == "bwd":
            old_speed = speed = int(args[0])
            Robokit.drive_backwards(speed)
        elif cmd == "stop":
            old_speed = 0
            Robokit.drive_stop()
        elif cmd == "crv":
            angle = int(args[0])
            if old_speed > 0:
                Robokit.drive_curve(angle, old_speed)
            else:
                print("Drive is not moving.")
        elif cmd == "imu":
            speed = int(args[0])
            Robokit.drive_imu_straight_forward(speed)
        elif cmd == "imuc":
            Robokit.drive_imu_reset_orientation()
        elif cmd == "imud":
            Robokit.drive_imu_orientation_deviate(int(args[0]))
        elif cmd == 'imur':
            angles = Robokit.status_imu_read()
            print(angles)
        elif cmd == 'imu-':
            Robokit.imu_disable()
        elif cmd == "led":
            num = int(args[0])
            red = int(args[1])
            green = int(args[2])
            blue = int(args[3])
            Robokit.led_setup(num, red, green, blue)
        elif cmd == "flush":
            Robokit.led_flush()
        elif cmd == "clear":
            Robokit.led_clear()
        elif cmd == "buz":
            num = int(args[0])
            Robokit.buzzer(num)
        elif cmd == 'dist':
            num = int(args[0])
            Robokit.approximate(num)
        elif cmd == 'bat':
            bat = Robokit.status_battery()
            print(bat)
        elif cmd == 'vec':
            vec = Robokit.status_drive_vector()
            print(vec)
        elif cmd == 'abs':
            d = Robokit.status_distance()
            print(d)
        elif cmd == 'falc':
            num = int(args[0])
            Robokit.fal_calibrate(num, 5000)
        elif cmd == 'fald':
            num = int(args[0])
            Robokit.fal_drive(num, 5000)
        elif cmd == 'fal':
            Robokit.fal_enable()
        elif cmd == 'falr':
            result = Robokit.fal_read_current_result()
            print(result)
        elif cmd == 'falshot':
            c = Robokit.fal_read_colors()
            print_formatted_fal_colors(c)
        else:
            print("Unknown command: "+cmd)
    except Exception as e:
        print(e)

    return response

try:
    Robokit.connect()
    print("Welcome to Robokit control unit!")
    print("Please enter commands to send to the roboter.")
    print("Commands can be joined using ; eg: fwd 50;crv 30")
    print("Type help to see available commands.")



    while True:
        msg = input(">> ")
        if msg.lower() in ("exit", "quit"):
            print("Disconnect now.")
            print("Goodbye.")
            break

        if msg.lower() == "help":
            print_help()
            continue

        cmds = msg.split(";")
        response = ""
        for cmd in cmds:
            args = cmd.strip().split()
            cmd = args[0]
            args = args[1:]

            response += perform_command(cmd, args)



        if len(response):
            print(f"<< Antwort: {response}")
except Exception as e:
    print(f"Fehler: {e}")
