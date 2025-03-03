
# Komponente hw_drive (Firmware)

- [Allgemein](#allgemein)
- [Verwendung](#verwendung)
- [Vorgehen in Software](#vorgehen-software)
- [Module](#module)
- [Modul Test](#modul-test)
- [Modul Fahren](#modul-fahren)
- [Modul LED Streifen](#modul-led-stripe)
- [Modul Follow A Line](#modul-follow-a-line-sensor)


## Allgemein
Die Firmware des Roboters wird als espressif component hw_drive programmiert.

Die Komponente ```hw_drive``` muss deshalb im Main Programm in der CMake Datei verlangt werden:

```cmake
idf_component_register(SRCS "main.c"
        ...
        INCLUDE_DIRS "."
        REQUIRES hw_drive
)
```

## Verwendung
Durch den include der device.h Datei wird die Firmware zur Verfügung gestellt und kann benutzt werden.

```c++
#include <device.h>
```

Folgender Code muss in der ```app_main()```- Funktion platziert sein:

```c++
#include "device.h"

void app_main() {
    // Initialize Firmware HW Drive
    device_init();
    ...
}
```
``device_init();`` muss immer ausgeführt werden und startet die Firmware des Fahrzeugs.  


### Vorgehen Software

Wenn die Software Kommandos an den Roboter schicken will, müssen folgende Schritte eingehalten werden.

1. Es wird Speicherpplatz for ein Kommando angelegt.
1. Das Kommando wird konfiguriert
1. Das Kommando wird in die Kommando-Kette eingeschleust.

```c++
#include "device.h"

void _what_ever_function(void) {
    // Punkt 1
    S_command cmd;
    
    ... what ever code
    
    // Punkt 2
    // Erzeuge ein Kommando, damit das Fz mit 75% Geschwindigkeit vorwärts fährt.
    if(!robokit_make_drive_command_fwd(&cmd, 75)) {
        perror("Could not create command.");
        return;
    }
    
    // Punkt 3
    if(!robokit_push_command(&cmd, 0)) {
        perror("Could not push command to drive stack.");
        return;
    }
    
    ... furthermore code
}
```

### Debug und Logging

Die HW Komponente verfügt über eine Direktive im ```hw_drive/include/config.h``` File.

```c++
#define ROBOKIT_DEBUG 1
// Debugging ist eingeschaltet.
// Komponente schreibt die HW Aktionen in die Konsole.

#define ROBOKIT_DEBUG 0
// Schaltet alle Logging-Funktionen aus.
```

## Module
Die Firmware ist in Module eingeteilt, welche Hardware und Software zusammenfassen. Jedes Modul kann sein Interesse am Geschehen in der Firmware wie folgt anmelden:

#### Kommando Callback
```c++
#include <modules/robokit_module.h>

enum {
    // Beispiel:
    E_COMMAND_MEIN_KOMMANDO = 15
    // Die Kommandos müssen Zahlen sein zwischen 1 und 30.
    // Kein Kommando darf zweimal definiert werden!
}

/**
 * Dieses Makro definiert eine Callback Funktion welche von der Firmware aufgerufen wird.
 * 
 * Der Aufruf geschieht zweimal. Vor dem Einschleuden mit dem Modus E_SCHEDULE_MODE_PRECHECK und
 * nach dem Einschleuden beim Ausführen mit E_SCHEDULE_MODE_PERFORM
 *
 * @param cmd S_command* Ein Zeiger auf das Kommando, welches behandelt werden muss.
 * @param mode uint8_t Der Modus, was die Funktion zu tun hat. Entweder eine Prüfung (E_SCHEDULE_MODE_PRECHECK)
 *             oder dann das Ausführen des Kommandos (E_SCHEDULE_MODE_PERFORM)
 * @param flags uint8_t* Ein Zeiger auf ein return Flag um zu markieren, ob die Prüfung erfolgreich war.
 */
ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_MEIN_KOMMANDO, S_command) {
    ...
}
```
Die Kommando Handler Funktion wird mit höchster Priorität aufgerufen, wenn das Kommando in der Hardware umgesetzt werden soll.  
Deshalb darf diese Funktion nur Konfigurationen vornehmen, sollte möglichst schnell damit fertig sein und darf keinesfalls blockieren.

Die Kommando Handler Funktion darf niemals selber Kommandos einschleusen, da sie sich sonst im dümmsten Fall selber blockiert (Dead Lock)!

#### Modul Initialisierung

Einige Module müssen Voreinstellungen in der Hardware treffen. Diese Einstellungen können in der Modul-Initialisierung vorgenommen werden.

```c++
#include <modules/robokit_module.h>

ROBOKIT_MODULE_INIT() {
    // Modul initialisieren, GPIOs festlegen, etc
    ...
}
```

#### Sensoren
Ein Modul kann auch Sensoren betreiben und aufgrund diesen Messwerte erheben und verarbeiten.  
Die Sensoren werden in einem separaten Task gesteuert und können folgendermassen angemeldet werden:
```c++
#include <modules/robokit_module.h>

ROBOKIT_MODULE_SENSOR_LOOP() {
    // 100Hz update für den Sensor
    // Der Sensor darf die Loop nicht blockieren und muss möglichst rasch
    // seine Messungen vorgenommen haben.
    ...
}
```

## Vordefinierte Module

### Modul "Test"

Das Test-Modul dient zum sichtbar Machen von Aktionen in der Software.  
Es besitzt nur ein Kommando und schaltet die eingebaute LED auf dem DevKit ein und aus.

```c++
uint8_t robokit_make_test_command(S_command *command);
```

### Modul "Fahren"
Folgende Kommandos stehen zur Verfügung:
```c++
// Vorwärts fahren mit vorgegebener Geschwindigkeit
uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed);

// Rückwärts fahren mit vorgegebener Geschwindigkeit
uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed);

// Vektor (Geschwindigkeit und Richtung) fahren.
uint8_t robokit_make_drive_command_vector(S_command *cmd, S_vector vector);
```
Dabei bedienen sich die Kommandos ```robokit_make_drive_command_fwd``` und ```robokit_make_drive_command_bwd``` ebenfalls des Vektorkommandos.

Die genaue Beschreibung zu den Fahrkommandos finden Sie in der [drive_command.h Datei](include/drive_command.h)

Ein Vektor kann mit folgenden Funktionen erstellt werden:

```c++
#include <device.h>

typedef struct {
    int8_t angle;          // Auf 3° genau
    T_Speed speed;         // Geschwindigkeit von 0 - 100
} S_vector;

// Erstelle einen Vektor mit kartesischen Angaben links, rechts und vor, zurück.
S_vector robokit_make_vector(int8_t left_right, int8_t forward_backward);

// Erstelle einen Vektor mit Winkel und Geschwindigkeit
S_vector robokit_make_vector_polar(int16_t angle, T_Speed speed);
```

Die genaue Beschreibung zu den Vektoren finden Sie in der [vector.h Datei](include/vector.h)

Es ist zu beachten, dass ein Vektor immer als Winkel und Geschwindigkeit an das System übergeben wird.  
Mit der kartesischen Funktion können Vektoren entstehen, welche eine Geschwindigkeit von mehr als 100 hätten.  
In diesem Fall wird ein ```ROBOKIT_INVALID_VECTOR``` zurückgegeben.

#### Drive Konfiguration
Die Konfiguration beschreibt, wie die Motoren angesteuert werden sollen.
```c++
typedef struct {
    // Wählt die Motoren aus.
    uint8_t motor_1:1;
    uint8_t motor_2:1;
    uint8_t motor_3:1;
    uint8_t motor_4:1;
    
    // Definiert, wie die Motoren angesteuert werden:
    // fast_decay=1 : Motortreiber schaltet Motor ein- und aus.
    // fast_decay=0 : Motortreiber schaltet Motor ein und auf hohe Impedanz.
    // Bewährt hat sich fast_decay=0.
    uint8_t fast_decay:1;
    
    // Falls nötig können die Kabel vertauscht werden.
    uint8_t switch_direction:1;
} S_motor_config;

void robokit_motor_left_set_config(S_motor_config config);
void robokit_motor_right_set_config(S_motor_config config);

S_motor_config robokit_motor_left_get_config(void);
S_motor_config robokit_motor_right_get_config(void);
```
Die Konfiguration muss bereit sein, bevor ein Kommando erstellt wird.  
Die Konfiguration ist im Drive Command gespeichert.

### Modul "LED Stripe"
Die LEDs werden auf Pin 13, separat herausgeführt angeschlossen.  
Es gilt, Ausgang auf Eingang. Das Testboard ist der erste Ausgang.  
Es sind zurzeit maximal 32 LEDs verfügbar.

Folgende Kommandos stehen zur Verfügung:
```c++
// Konfiguriert eine einzelne LED für eine Farbe
uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue);

// Konfiguriert eine Liste mit LED für eine Farbe
uint8_t robokit_make_led_command_setup_list(S_command *cmd, uint32_t LED_numbers, uint8_t red, uint8_t green, uint8_t blue);

// Nachdem alle LEDs wie gewünscht konfiguriert sind, muss ein flush Kommando gesendet werden.
// Erst jetzt werden die Farben des LED-Stripes eingestellt.
uint8_t robokit_make_led_command_flush(S_command *cmd);

// Löscht alle LEDs aus.
uint8_t robokit_make_led_command_clear(S_command *cmd);
```

Die LEDs müssen immer zuerst konfiguriert werden auf ihre Farben. Dabei kann eine Farbe einzeln an eine LED übergeben werden
oder an eine Liste von LEDs.  
Die Liste ist eine Bitmasked ```uint32_t```, welche die LEDs identifiziert.  
Folgendes Makro zur Erstellung solcher Listen steht zur Verfügung:

```c++
ROBOKIT_MAKE_LED_LIST(VAR, ...)

void main() {
    device_init();
    
    uint32_t list=0;
    ROBOKIT_MAKE_LED_LIST(list, LED_1, LED_5, LED_8);
    // macht eine Liste mit den LEDs 1, 5 und 8.
}
```

#### Beispiel code
Während 5 Minuten zufällig LEDs von 1 bis 8 mit zufällig Rot, Grün oder Blau ansteuern.
```c++
void app_main()
{
    device_init();

    S_command cmd;

    for(int e = 0; e < 3000;e++) {
        // Alle LEDs ausschalten
        robokit_make_led_command_clear(&cmd);
        robokit_push_command(&cmd, 0);
        
        // Zufällig eine Farbe auswählen
        switch(rand() % 3) {
            case 0:
                robokit_make_led_command_setup(&cmd, e % 8, ROBOKIT_LED_COLOR_RED);
            break;
            case 1:
                robokit_make_led_command_setup(&cmd, e % 8, ROBOKIT_LED_COLOR_GREEN);
            break;
            default:
                robokit_make_led_command_setup(&cmd, e % 8, ROBOKIT_LED_COLOR_BLUE);
            break;
        }

        robokit_push_command(&cmd, 0);

        robokit_make_led_command_flush(&cmd);
        robokit_push_command(&cmd, 0);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
```
LEDs 1, 5 und 8 rot machen und nach 2 Sekunden alles ausschalten
```c++
void app_main()
{
    device_init();

    S_command cmd;
    robokit_make_led_command_clear(&cmd);
    robokit_push_command(&cmd, 0);
    
    uint32_t LEDs=0;
    ROBOKIT_MAKE_LED_LIST(LEDs, LED_1, LED_5, LED_8);
    
    robokit_make_led_command_setup_list(&cmd, LEDs, ROBOKIT_LED_COLOR_RED);
    robokit_push_command(&cmd, 0);
    
     robokit_make_led_command_flush(&cmd);
     robokit_push_command(&cmd, 0);

     vTaskDelay(2000 / portTICK_PERIOD_MS);
    
     robokit_make_led_command_clear(&cmd);
     robokit_push_command(&cmd, 0);
}
```

### Modul "Follow a Line Sensor"
Das Follow A Line System folgt einer schwarzen Linie auf hellem Grund mit maximalen Winkeln von 40°. Die Geschwindigkeit ist auf 50% fix eingestellt.  
Bevor das System funktioniert, muss es kalibriert werden. Dabei soll es von weissem Grund über eine mindestens 3cm breite schwarze Fläche fahren.  
Das Fahrzeug fährt von alleine los, wenn ein Kalibrierungskommando ankommt.
Nach 500ms oder wenn die Kalibrierung abgeschlossen ist, stoppt das Fz und die GUI Software wird informiert.

#### Beispiel Code
```c++
static uint8_t warten = 1;

static void kalibration_ist_beendet(uint8_t ok) {
    // ACHTUNG!
    // Die callback wird nicht vom main Task ausgeführt.
    // Sie soll möglichst schnell wieder zurückkehren.
    if(ok) {
        ROBOKIT_LOGI("kalibration ist beendet");
        warten = 0;
    } else {
        ROBOKIT_LOGE("Kalibrierung hat nicht funktioniert.");
    }
}


void app_main()
{
    device_init();

    S_command cmd;
    // Callback angeben, welche aufgerufen wird, sobald die Kalibration abgeschlossen ist.
    robokit_make_command_fal_calibrate(&cmd, kalibration_ist_beendet);
    robokit_push_command(&cmd, 0);

    // Warten, bis Kalibration fertig ist.
    while (warten) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    robokit_make_command_fal_enable(&cmd);
    robokit_push_command(&cmd, 0);

    // 5 Sekunden einer Linie folgen.
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    robokit_make_command_fal_disable(&cmd);
    robokit_push_command(&cmd, 0);
}
```