from fastapi import FastAPI
from pydantic import BaseModel
from typing import Dict
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

app = FastAPI()

# CORS aktivieren
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # oder z. B. ["http://localhost:8601"] für Scratch GUI
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Simulierter Speicher der Roboterpositionen
axis_positions: Dict[int, float] = {}

class MoveRequest(BaseModel):
    id: int
    angle: float

@app.post("/move")
def move_axis(req: MoveRequest):
    axis_positions[req.id] = req.angle
    # Hier würdest du echte Robotersteuerung machen!
    return {"status": "ok", "id": req.id, "angle": req.angle}

@app.get("/position")
def get_position(id: int):
    if id not in axis_positions:
        return {"status": "error", "message": "unknown axis"}
    return {"status": "ok", "id": id, "angle": axis_positions[id]}

@app.get("/ping")
def ping():
    return {"status": "alive"}

# Nur lokal starten, nicht im Produktionsumfeld
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8123)



    
