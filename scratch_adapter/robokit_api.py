from fastapi import FastAPI
from pydantic import BaseModel
from typing import Dict
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import sys
import os
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
robot.connect()

class BuzzerRequest(BaseModel):
    enable: bool
    frequency: int

@app.post("/buzzer")
def buzzer(req: BuzzerRequest):

    if req.enable:
        robot.buzzer(req.frequency)
    else:
        robot.buzzer(0)
    
    return {"status": "ok", "enable": req.enable, "frequency": req.frequency}

@app.get("/ping")
def ping():
    return {"status": "alive"}

# Nur lokal starten, nicht im Produktionsumfeld
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)




