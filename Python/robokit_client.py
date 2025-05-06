
import robokit as RK

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

# ich bin jetzt do
Robokit = RK.Robokit(ESP32_IP, ESP32_PORT)

def print_help():
    print("Command     | Description")
    print("------------|---------------------------------------------")
    print("fwd <speed> | Drive forward with specified speed <speed>")
    print("bwd <speed> | Drive backwards with specified speed <speed>")
    print("crv <angle> | Makes a curve maintaining speed before")
    print("            | <angle> in degrees of 3° from -180 to 180")
    print("imu <speed> | Drive straight forward using the IMU")
    print("imur <speed>| Drive straight backwards using the IMU")
    print("stop        | Stops the drive immediately")
    print("fal start   | Starts the FAL sensor's calibration")
    print("fal <speed> | Follow a line with specified speed <speed>")
    print("test        | The test command toggles an LED on the board")
    print("led <n> <cl>| n is the selected led and cl the color as 0xRGB")
    print("flush       | Flushes the LED setting to the drive")
    print("clear       | Clears the LED.")

try:
    Robokit.connect()
    print("Welcome to Robokit control unit!")
    print("Please enter commands to send to the roboter.")
    print("Type help to see available commands.")

    old_speed = 0

    while True:
        msg = input(">> ")
        if msg.lower() in ("exit", "quit"):
            print("🔌 Disconnect now.")
            print("Goodbye.")
            break

        if msg.lower() == "help":
            print_help()
            continue

        args = msg.split()
        cmd = args[0]
        args = args[1:]

        try:
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
            elif cmd == "imur":
                speed = int(args[0])
                Robokit.drive_imu_straight_backward(speed)
            elif cmd == "test":
                Robokit.test()
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
        except Exception as e:
            print(e)
            continue

        response = ""

        if len(response):
            print(f"<< Antwort: {response}")
except Exception as e:
    print(f"❌ Fehler: {e}")
