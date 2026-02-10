from pydantic import BaseModel
from datetime import datetime

class SensorRead(BaseModel):
    device_id: str
    temperature: float
    humidity: float
    luminosity: float
    timestamp: datetime
