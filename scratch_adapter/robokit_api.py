from fastapi import FastAPI
from pydantic import BaseModel
from typing import Dict
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import sys
import os
import time  # Importiere das Modul für die Wartezeit

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'PythonApp')))
from robokit import Robokit

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
robot.connect()


class BuzzerRequest(BaseModel):
    enable: bool
    frequency: int

class LEDRequest(BaseModel):
    led_nr: int
    blue: int; # 0-255
    green: int; # 0-255
    red: int; # 0-255
    

class DriveRequest(BaseModel):
    power: int
    duration: float  # Dauer in Sekunden

class drive_noTimeRequest(BaseModel):
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
    direction: str # left  and Right 
    
    

@app.post("/buzzer")
def buzzer(req: BuzzerRequest):

    if req.enable:
        robot.buzzer(req.frequency)
    else:
        robot.buzzer(0)
    
    return {"status": "ok", "enable": req.enable, "frequency": req.frequency}

# Fahren vorwärts mit Zeit
@app.post("/driveforward")
def driveforward(req: DriveRequest):

    robot.drive_forward(req.power,)
    time.sleep(req.duration)  # Warte für die angegebene Dauer
    robot.drive_stop()
          
    return {"status": "ok", "power": req.power, "time": req.duration}

#Fahren vorwärts ohne Zeit
@app.post("/driveforward_noTime")
def drivevorward_noTime(req: drive_noTimeRequest):
    robot.drive_forward(req.power,)
    return {"status": "ok", "power": req.power}

#Fahren rückwärts mit Zeit
@app.post("/drivebackward")
def drivebackward(req: DriveRequest):

    robot.drive_backwards(req.power,)
    time.sleep(req.duration)
    robot.drive_stop()
  
    return {"status": "ok", "power": req.power, "time": req.duration}

#Fahren rückwärts ohne Zeit
@app.post("/drivebackward_noTime")
def drivebackward_noTime(req: drive_noTimeRequest):
    robot.drive_backwards(req.power,)
    return {"status": "ok", "power": req.power}

# Fahren stoppen
@app.post("/Driving_stop")
def driving_stop(drive_stop: DrivestopRequest):
    
    robot.drive_stop()
    return {"status": "ok"}



@app.post("/drivecircle")
def post_drivecurve(req: DrivecircleRequest):
    
    #robot.drive_forward(req.power,)
    if req.direction == "right":
        robot.drive_curve(req.angle, req.power,)
    
    if req.direction == "left":
        robot.drive_curve(-req.angle, req.power,)

    time.sleep(req.duration)
    robot.drive_stop()    
    
    return {"status": "ok", "angle": req.angle, "power": req.power, "time": req.duration, "direction": req.direction}
       

@app.post("/led")
def led(req: LEDRequest):
    robot.led_setup(req.led_nr, req.red, req.green, req.blue)
    return {"status": "ok", "led_nr": req.led_nr, "red": req.red, "green": req.green, "blue": req.blue}

@app.post("/leds")

@app.get("/ping")
def ping():
    return {"status": "alive"}

# Nur lokal starten, nicht im Produktionsumfeld
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)




