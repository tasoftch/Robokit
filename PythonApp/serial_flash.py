import sys
import subprocess
import re

bin_file = "serial.bin"
SERIAL_OFFSET_ADDR = "0x200000"

SERIAL_LEN = 6
SSID_LEN = 31
PASS_LEN = 63
TOTAL_SIZE = 4096

ALLOWED_CHARS_REGEX = re.compile(r'^[a-zA-Z0-9!@#$%^&*()\-_=\[\]{}<>?,.:;\"\'/\\|~`+ ]*$')


def pad_or_truncate(value, length, label):
    data = value.encode('utf-8')
    if len(data) > length:
        print(f"Fehler: {label} ist zu lang (max {length} Bytes).")
        sys.exit(1)
    return data.ljust(length, b'\x00')

def validate_allowed_chars(value, label):
    if not ALLOWED_CHARS_REGEX.match(value):
        print(f"Fehler: {label} enthält ungültige Zeichen.")
        print("Erlaubt sind: Buchstaben, Zahlen und gängige Sonderzeichen.")
        sys.exit(1)

def main():
    print("Bitte gib die folgenden Informationen ein:")

    serial = input("Seriennummer (6 Zeichen) >> ").strip()
    if len(serial.encode('utf-8')) != SERIAL_LEN:
        print(f"Fehler: Seriennummer muss genau {SERIAL_LEN} Bytes lang sein.")
        sys.exit(1)

    #ssid = input("WLAN SSID (max. 64 Zeichen) >> ").strip()
    #validate_allowed_chars(ssid, "SSID")
    #password = input("WLAN Passwort (max. 64 Zeichen) >> ").strip()
    #validate_allowed_chars(password, "Passwort")

    serial_bytes = serial.encode('utf-8')
    #ssid_bytes = pad_or_truncate(ssid, SSID_LEN, "SSID")
    #pass_bytes = pad_or_truncate(password, PASS_LEN, "Passwort")

    # Gesamt-Binärstruktur: [6B Seriennummer][64B SSID][64B Passwort][Rest = 0]
    combined = serial_bytes # + ssid_bytes + pass_bytes

    if len(combined) > TOTAL_SIZE:
        print("Fehler: Gesamtdaten sind zu groß.")
        sys.exit(1)

    # Datei schreiben
    with open(bin_file, "wb") as f:
        f.write(combined)
        f.write(b'\x00' * (TOTAL_SIZE - len(combined)))

    print(f"Datei '{bin_file}' erfolgreich erstellt.")

    subprocess.run([
        "esptool.py",
        "--chip", "esp32c6",
        "write_flash", SERIAL_OFFSET_ADDR, bin_file
    ])

if __name__ == "__main__":
    main()