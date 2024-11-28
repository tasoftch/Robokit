
# HW_DRIVE COMPONENT

## Allgemein
Alle Firmware des Roboters wird als espressif component hw_drive programmiert.

Die Komponente ```hw_drive``` muss deshalb im Main Programm in der CMake Datei verlangt werden:

```cmake
idf_component_register(SRCS "main.c"
        ...
        INCLUDE_DIRS "."
        REQUIRES hw_drive   
)
```

## Verwendung
Durch den include der device.h Datei kannst du auf die Firmware zugreifen.

```c++
#include <device.h>
```

Ich habe den Betrieb der Firmware sichergestellt.  
Folgender Code ist in der ```app_main()```- Funktion platziert:

```c++
#include "device.h"

void app_main() {
    // Initialize Firmware HW Drive
    device_init();
    ...
}
```
Das muss immer ausgeführt werden und startet die Firmware des Fahrzeugs.  
Zurzeit sind erst die Test Kommandos verfügbar.

### Vorgehen
Das Vorgehen ist wie folgt:
1. Es wird ein Kommando erzeugt.
1. Das Kommando wird konfiguriert
1. Das Kommando wird in die Kommando-Kette eingeschleust.
1. Das Kommando wird ausgeführt.

```c++
#include "device.h"

void _what_ever_function(void) {
    S_command cmd;
    
    ... what ever code
    
    // Erzeuge ein Kommando, damit das Fz mit 75% Geschwindigkeit vorwärts fährt.
    if(!robokit_make_drive_command_fwd(&cmd, 75)) {
        perror("Could not create command.");
        return;
    }
    
    if(!robokit_push_command(&cmd, 0)) {
        perror("Could not push command to drive stack.");
        return;
    }
    
    // Erzeugt gleich noch ein Kommando, welches zum Beispiel einige LEDs festlegt:
    uint32_t LEDs=0;
    ROBOKIT_MAKE_LED_LIST(LEDs, LED_1, LED_5, LED_8, LED_15);
    
    if(!robokit_make_led_command_setup_list(&cmd, LEDs, 255, 0, 0)) {
        perror("Could not create command.");
        return;
    }
    
    robokit_push_command(&cmd, 0);
    
    if(!robokit_make_led_command_flush(&cmd)) {
        perror("Could not create command.");
        return;
    }
    robokit_push_command(&cmd, 0);
    // Ab jetzt leuchten die LEDs 1, 5, 8 und 15 rot.
    
    
    ... furthermore code
}

```

### Debug und Logging

Die HW Komponente verfügt über eine Direktive im ```hw_drive/config.h``` File.

```c++
#define ROBOKIT_DEBUG 1
// Debugging ist eingeschaltet.
// Komponente schreibt die HW Aktionen in die Konsole.

#define ROBOKIT_DEBUG 0
// Schaltet alle Logging-Funktionen aus.
```

### Drive Command
Folgende Kommandos stehen zur Verfügung:
```c++
uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed);
uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed);
uint8_t robokit_make_drive_command_vector(S_command *cmd, S_vector vector);
```

Dabei bedienen sich die Kommandos ```robokit_make_drive_command_fwd``` und ```robokit_make_drive_command_bwd``` ebenfalls des Vektorkommandos.

#### Drive Konfiguration
Die Konfiguration beschreibt, wie die Motoren angesteuert werden sollen.
```c++
typedef struct {
      // Wähle die Motoren aus.
	  uint8_t motor_1:1;
	  uint8_t motor_2:1;
	  uint8_t motor_3:1;
	  uint8_t motor_4:1;
	  // Definiere, ob die Motoren im Low oder High Drive mode sind.
	  // low_drive=0 hat sich bewährt.
	  uint8_t low_drive:1;
	  // Falls nötig kannst du die Kabel tauschen.
	  uint8_t switch_direction:1;
} S_motor_config;

void robokit_motor_left_set_config(S_motor_config config);
void robokit_motor_right_set_config(S_motor_config config);

S_motor_config robokit_motor_left_get_config(void);
S_motor_config robokit_motor_right_get_config(void);
```
Die Konfiguration muss bereit sein, bevor ein Kommando erstellt wird.  
Die Konfiguration ist im Drive Command gespeichert.


### LEDS
Die LEDs werden auf Pin 13, separat herausgeführt angeschlossen.  
Es gilt, Ausgang auf Eingang. Das Testboard ist der erste Ausgang.  
Es sind zurzeit maximal 32 LEDs verfügbar.

#### Beispiel code
```c++
void app_main()
{
    device_init();

    S_command cmd;

    for(int e = 0; e < 5000;e++) {
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

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
```


