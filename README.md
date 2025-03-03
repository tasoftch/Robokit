# Robokit MINT pro3E

Die FHNW engagiert sich stark in der Nachwuchsförderung, organisiert Camps, Workshops
und Nachwuchstage. Oftmals werden dabei externe Bausätze/Plattformen eingesetzt. Warum
nicht ein interdisziplinäres Studierendenprojekt-Team ins Leben rufen, um eigene FHWN MINT-
Bausätze zu konzipieren, entwickeln, fertigen und das Ganze als Marketing-Plattform zu nutzen?
Low-Cost-Bausätze im MINT-Bereich gibt es viele, teils von Spiele-Firmen oder direkt aus China.
Konzept, Mechanik, Qualität und Datenzugang sind praktisch immer ungenügend. Im High-
End Bereich gibt es z.B. mobile Lern-Roboter, die mit Anwendung und Konzept überzeugen,
aber diese sind nicht oder kaum erweiterbar, teuer und HW/SW Daten nicht verfügbar. Soweit
bekannt, bietet keine andere FH solche Bausätze an.

## Organisation

Dieses Repository enthält die benötigte und entwickelte Software zur Ansteuerung des Roboters.

Es ist unterteilt in
- Software, zu finden im [```main```](main) Verzeichnis  
    Alles, was mit der Kommunikation nach aussen zu tun hat.
- Firmware, zu finden im [```components/hw_drive```](components/hw_drive) Verzeichnis  
  Steuert die Hardwarekomponenten an.

### Installation

Wir empfehlen die offizielle IDE von espressif zu installieren. [Espressif IDE Getting started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html)  
Danach clonen Sie dieses Repository auf Ihren Rechner und starten Sie die espressif IDE:

```bin
git clone https://github.com/tasoftch/Robokit.git
cd Robokit
. ../path/to/esp-idf/export.sh
```
Danach steht Ihnen die IDF von espressif zur Verfügung:

```bin
idf.py flash
```
Baut das project und lädt das Programm auf den Flash.

```bin
idf.py monitor
```
Baut das project auch und lädt das Programm auf den Flash.  
Danach startet die Konsole und alle printf() ANweisungen werden auf dem Terminal ausgegeben.