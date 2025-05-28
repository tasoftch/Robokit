from fastapi import FastAPI, Request
from pydantic import BaseModel
from typing import Dict, List
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import sys
import os
import time  # Importiere das Modul für die Wartezeit
import logging
from fastapi.exceptions import RequestValidationError
from fastapi.responses import JSONResponse


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
    ledstring: str  # Liste von LEDs 
    blue: int # 0-255
    green: int# 0-255
    red: int # 0-255
    
class LEDMenuRequest(BaseModel):
     ledstring: str  # Liste von LEDs 
     color : str
    
    
    
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
    
class MeldoyRequest(BaseModel):
    melody: str


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
    # Convert string to int list
    leds = list(map(int, req.ledstring.split(",")))
    red = round((req.red / 100) * 60)  # Dreisatz: 0% = 0, 100% = 60, gerundet
    blue = round((req.blue / 100) * 60)
    green = round((req.green / 100) * 60)
    print(red, green, blue)
    print(leds)

    robot.leds_setup(leds, red, green, blue)
    robot.led_flush()

    return {"status": "ok", "leds": leds, "red": red, "green": green, "blue": blue}





@app.post("/leds_manu")
def leds(req: LEDMenuRequest):
    #string to int list
    leds = list(map(int, req.ledstring.split(",")))
    
    # Map color names to RGB values (0-100 scale)
    color_map = {
        'Rot':    (100, 0, 0),
        'Grün':   (0, 100, 0),
        'Blau':   (0, 0, 100),
        'Weiß':   (100, 100, 100),
        'Lila':   (100, 0, 100),
        'Türkis': (50, 100, 50),
        'Orange': (100, 50, 0),
        'Pink':   (100, 0, 50),
        'hellblau': (100, 50, 50),
        'Aus':    (0, 0, 0),
    }
    color = req.color
    red, green, blue = color_map.get(color, (0, 0, 0))
    red = round((red / 100) * 60)  # Dreisatz: 0% = 0, 100% = 60, gerundet
    blue = round((blue / 100) * 60)
    green = round((green / 100) * 60)
    
    print(red, green, blue)
    print(leds)

    robot.leds_setup(leds, red, green, blue)
    robot.led_flush()
    return {"status": "ok", "leds": leds, "red": red, "green": green, "blue": blue}



@app.post("/ledclear")
def ledclear(req: ledclearRequest):
    robot.led_clear()
    return {"status": "ok"} 

@app.post("/melody")
def ledclear(req: MeldoyRequest):
    notes = [
        (0,0)
    ]
    
    if req.melody == 'Alle meine Entschen':
        notes = [
            (280, 0.4), (312, 0.4), (348, 0.4), (367, 0.4), (410, 0.4), (410, 0.4), (410, 0.8),
            (458, 0.4), (458, 0.4), (458, 0.8),
            (410, 0.4), (410, 0.4), (410, 0.8),
            (348, 0.4), (348, 0.4), (348, 0.8),
            (280, 0.4), (312, 0.4), (348, 0.4), (367, 0.4), (410, 0.4), (410, 0.4), (410, 0.8),
            (458, 0.4), (458, 0.4), (458, 0.8),
            (410, 0.4), (348, 0.4), (280, 0.8)
        ]

    elif req.melody == 'Happy Birthday':
        # Happy Birthday Melodie (angepasste Frequenzen, tiefste ca. 280 Hz)
        notes = [
            (348, 0.3), (348, 0.3), (392, 0.5), (348, 0.5), (466, 0.5), (440, 1.0),
            (348, 0.3), (348, 0.3), (392, 0.5), (348, 0.5), (523, 0.5), (466, 1.0),
            (348, 0.3), (348, 0.3), (698, 0.5), (587, 0.5), (466, 0.5), (440, 0.5), (392, 0.5),
            (587, 0.3), (587, 0.3), (523, 0.5), (466, 0.5), (523, 0.5), (466, 1.0)
        ]
    for freq, dur in notes:
        intfreq = int(freq)
        robot.buzzer(intfreq)
        time.sleep(dur)
        robot.buzzer(0)
        time.sleep(0.05)
    return {"status": "ok", "Melody": req.melody} 




@app.post("/approximate")
def approximate(req: distanceRequest,):
    status = "NOT MOVING"
    if robot.status_drive_vector()["speed"] > 0:
        status = "ok"
        robot.approximate(req.distance)
        
    return {"status": status , "distance": req.distance}


@app.get("/distance")
def get_distance():
    distance = robot.status_distance()
    return {"status": "ok", "distance": distance}

@app.get("/battery")
def get_battery():
    battery = robot.status_battery()
    # Status:
    # 0: Emergency (Roboter does not respond to anything.)
    # 1: Critical
    # 2: Warning
    # 3: OK
    status_map = {
        0: "emergency",
        1: "critical",
        2: "warning",
        3: "ok"
    }
    status_str= status_map.get(battery["status"], str(battery["status"]))
    return {"status": "ok", "Voltag": battery["voltage"], "Batterie_status": status_str, "charge": battery["charge"]}

@app.get("/drive_angle")
def get_drive_vector():
    drive_vector = robot.status_drive_vector()
    return {"status": "ok", "angle": drive_vector["angle"]}

@app.get("/drive_speed")
def get_speed():
    drive_vector = robot.status_drive_vector()
    return {"status": "ok", "speed": drive_vector["speed"]} 

@app.get("/drive_vector")
def get_drive_vector():
    drive_vector= robot.status_drive_vector()
    return {"status": "ok", "drive_vector": drive_vector}



@app.get("/color_current")
def get_color_curent():
    color = robot.fal_read_current_result()
    return {"status": "ok", "Farbe": color['middle']}

@app.get("/color")
def get_color():
    color = robot.fal_read_colors()
    return {"status": "ok", "Farbe": color}


@app.get("/imu")
def get_imu():
    imu = robot.status_imu_read()
    return {"status": "ok","position": imu["position"], "orientation": imu["orientation"],"deviation": imu["deviation"],}




    



@app.get("/ping")
def ping():
    return {"status": "alive"}

@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request: Request, exc: RequestValidationError):
    body = await request.body()
    logging.error(f"Validation error: {exc.errors()} - Body: {body.decode('utf-8')}")
    return JSONResponse(
        status_code=422,
        content={"detail": exc.errors()},
    )

# Nur lokal starten, nicht im Produktionsumfeld
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)





