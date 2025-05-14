from fastapi import FastAPI
from pydantic import BaseModel
from typing import Dict, List
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import sys
import os
import time  # Importiere das Modul für die Wartezeit
import logging


sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'PythonApp')))
from robokit import Robokit

logging.basicConfig(level=logging.debug, format='%(asctime)s - %(levelname)s - %(message)s')

ESP32_IP = "192.168.4.1"
ESP32_PORT = 8080

TEST_IP = "127.0.0.1"
TEST_PORT = 7000




app = FastAPI()

# CORS aktivieren
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # oder z. B. ["http://localhost:8601"] für Scratch GUI
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

robot = Robokit(ESP32_IP, ESP32_PORT)
#robot = Robokit(TEST_IP, TEST_PORT)
#robot.connect()


class BuzzerRequest(BaseModel):
    enable: bool
    frequency: int

class LEDRequest(BaseModel):
    leds: list[int]  # Liste von LEDs 
    blue: int; # 0-255
    green: int; # 0-255
    red: int; # 0-255
    
    
class ledclearRequest(BaseModel):
    pass
    

class DriveRequest(BaseModel):
    direction: bool  # "forward" = 1 oder "backward" = 0
    power: int
    duration: float  # Dauer in Sekunden

class drive_noTimeRequest(BaseModel):
    direction: bool  # "forward" = 1 oder "backward" = 0
    power: int
    

class DriveforwardRequest(BaseModel):
    power: int
    duration: float 
    
class DrivebackwardRequest(BaseModel):
    power: int
    duration: float 
    
class DrivestopRequest(BaseModel):
    pass

class driveforward_noTimeRequest(BaseModel):
    power: int
    
class drivebackward_noTimeRequest(BaseModel):
    power: int

class DrivecircleRequest(BaseModel):
    angle: int
    power: int
    duration: float
    turn: str # left  and Right 
    direction: bool # Vorwärts und Rückwärts
    
class DrivecircleRequest_notime(BaseModel):
    angle: int
    power: int
    turn: str # left  and Right 
    direction: bool # Vorwärts und Rückwärts
    
class distanceRequest(BaseModel):
    distance: int

@app.post("/buzzer")
def buzzer(req: BuzzerRequest):

    if req.enable:
        robot.buzzer(req.frequency)
    else:
        robot.buzzer(0)
    
    return {"status": "ok", "enable": req.enable, "frequency": req.frequency}


@app.post("/drive")
def drive(req: DriveRequest):
    if req.direction:
        
        if req.power >= 50 and req.power <= 100:
            duration_differenz = req.duration
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_forward(i)  # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
                duration_differenz-= 0.02  # Verringere die verbleibende Zeit um 0.02 Sekunden
                print(i)
            time.sleep(duration_differenz)  # Warte für die angegebene Dauer
            robot.drive_stop()
        else:
            robot.drive_forward(req.power,)
            time.sleep(req.duration)  # Warte für die angegebene Dauer
            robot.drive_stop()
    else:
        if req.power >= 50 and req.power <= 100:
            duration_differenz = req.duration
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_backwards(i)  # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
                duration_differenz-= 0.02  # Verringere die verbleibende Zeit um 0.02 Sekunden
                print(i)
            time.sleep(duration_differenz)  # Warte für die angegebene Dauer
            robot.drive_stop()
    
        else:
            robot.drive_backwards(req.power)
            time.sleep(req.duration)  # Warte für die angegebene Dauer
            robot.drive_stop()

    return {"status": "ok", "direction": req.direction, "power": req.power, "time": req.duration}

@app.post("/drive_notime")
def drive(req: drive_noTimeRequest):
    if req.direction:
        if req.power >= 50 and req.power <= 100:
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_forward(i)  # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
        else:
            robot.drive_forward(req.power,)

    else:
        if req.power >= 50 and req.power <= 100:
  
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_backwards(i)  # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
    
        else:
            robot.drive_backwards(req.power)
            robot.drive_stop()

    return {"status": "ok", "direction": req.direction, "power": req.power}

#Fahren vorwärts mit Zeit
@app.post("/driveforward")
def drivevorward_noTime(req: DriveRequest):
    if req.power >= 50 and req.power <= 100:
        duration_differenz = req.duration
        for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
            robot.drive_forward(i)  # Fahre mit der aktuellen Leistung
            time.sleep(0.02)
            duration_differenz-= 0.02  # Verringere die verbleibende Zeit um 0.02 Sekunden
            print(i)
        time.sleep(duration_differenz)  # Warte für die angegebene Dauer
        robot.drive_stop()
    else:
        robot.drive_forward(req.power,)
        time.sleep(req.duration)  # Warte für die angegebene Dauer
        robot.drive_stop()
        
    return {"status": "ok", "power": req.power, "time": req.duration}



#Fahren vorwärts ohne Zeit
@app.post("/driveforward_noTime")
def drivevorward_noTime(req: drive_noTimeRequest):
    if req.power >= 50 and req.power <= 100:
       
        for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
            robot.drive_forward(i)  # Fahre mit der aktuellen Leistung
            time.sleep(0.02)
            duration_differenz-= 0.02  # Verringere die verbleibende Zeit um 0.02 Sekunden
            print(i)
        robot.drive_stop()
    
    else:
        robot.drive_forward(req.power)
    
        
    return {"status": "ok", "power": req.power}
    

#Fahren rückwärts mit Zeit
@app.post("/drivebackward")
def drivebackward(req: DriveRequest):
    
    if req.power >= 50 and req.power <= 100:
        duration_differenz = req.duration
        for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
            robot.drive_backwards(i)  # Fahre mit der aktuellen Leistung
            time.sleep(0.02)
            duration_differenz-= 0.02  # Verringere die verbleibende Zeit um 0.02 Sekunden
            print(i)
        time.sleep(duration_differenz)  # Warte für die angegebene Dauer
        robot.drive_stop()
    
    else:
        robot.drive_backwards(req.power)
        time.sleep(req.duration)  # Warte für die angegebene Dauer
        robot.drive_stop()
        
    return {"status": "ok", "power": req.power, "time": req.duration}

#Fahren rückwärts ohne Zeit
@app.post("/drivebackward_noTime")
def drivebackward_noTime(req: drive_noTimeRequest):
    
    if req.power >= 50 and req.power <= 100:
       
        for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
            robot.drive_backwards(i)  # Fahre mit der aktuellen Leistung
            time.sleep(0.02)
            print(i)
        robot.drive_stop()
    
    else:
        robot.drive_backwards(req.power)
    
        
    return {"status": "ok", "power": req.power}

# Fahren stoppen
@app.post("/Driving_stop")
def driving_stop(drive_stop: DrivestopRequest):
    
    robot.drive_stop()
    return {"status": "ok"}



@app.post("/drivecircle")
def post_drivecurve(req: DrivecircleRequest):
    
    winkl = req.angle
    if req.direction == False:
        winkl = 180 - req.angle
    
    
    #robot.drive_forward(req.power,)
    if req.turn == "Rechts":
        if req.power >= 50 and req.power <= 100:
            duration_differenz = req.duration
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_curve(winkl, i) # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
                duration_differenz-= 0.02 
            time.sleep(duration_differenz)
            robot.drive_stop()
                 
        else: 
            robot.drive_curve(winkl, req.power)
            time.sleep(req.duration)
            robot.drive_stop()
            
            
            
    if req.turn == "Links":
        if req.power >= 50 and req.power <= 100:
            duration_differenz = req.duration
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_curve(-winkl, i) # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
                duration_differenz-= 0.02 
            time.sleep(duration_differenz)
            robot.drive_stop()
                
        else: 
            robot.drive_curve(-winkl, req.power)
            time.sleep(req.duration)
            robot.drive_stop()
      
    
    return {"status": "ok", "angle": winkl, "power": req.power, "time": req.duration, "direction": req.direction}
      

@app.post("/drivecircle_notime")
def post_drivecurve(req: DrivecircleRequest_notime):
    
    winkl = req.angle
    if req.direction == False:
        winkl = 180 - req.angle
    
    #robot.drive_forward(req.power,)
    if req.turn == "Rechts":
        if req.power >= 50 and req.power <= 100:
            
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_curve(winkl, i) # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
    
                 
        else: 
            robot.drive_curve(winkl, req.power)

            
            
            
    if req.turn == "Links":
        if req.power >= 50 and req.power <= 100:
  
            for i in range(50, req.power + 1, 20):  # Beginne bei 50, erhöhe in Schritten von 20, aber überschreite req.power nicht
                robot.drive_curve(-winkl, i) # Fahre mit der aktuellen Leistung
                time.sleep(0.02)
                
        else: 
            robot.drive_curve(-winkl, req.power)

      
    
    return {"status": "ok", "angle": winkl, "power": req.power, "direction": req.direction} 

@app.post("/leds_manuelle")
def leds(req: LEDRequest):
    #string to int list
    red = round((req.red / 100) * 60)  # Dreisatz: 0% = 0, 100% = 60, gerundet
    blue = round((req.blue / 100) * 60)
    green = round((req.green / 100) * 600)
    robot.leds_setup(req.leds, red, green, blue)
    robot.led_flush()
    return {"status": "ok", "leds": leds, "red": red, "green": green, "blue": blue}




@app.post("/leds_manu")
def leds(req: LEDRequest):
    #string to int list
    leds = list(map(int, req.leds.split(",")))
    
    robot.leds_setup(leds, req.red, req.green, req.blue)
    robot.led_flush()
    return {"status": "ok", "leds": leds, "red": req.red, "green": req.green, "blue": req.blue}



@app.post("/ledclear")
def ledclear(req: ledclearRequest):
    robot.led_clear()
    return {"status": "ok"} 



@app.post("/approximate")
def approximate(req: distanceRequest,):
  
  robot.approximate(req.distance)
  
  return {"status": "ok", "distance": req.distance}






@app.get("/ping")
def ping():
    return {"status": "alive"}



# Nur lokal starten, nicht im Produktionsumfeld
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)



