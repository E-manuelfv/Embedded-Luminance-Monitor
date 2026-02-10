from sqlalchemy import Column, String, Float, DateTime
from backend.app.core.database import Base
from datetime import datetime

class SensorData(Base):
    __tablename__ = "sensor_data"

    device_id = Column(String, primary_key=True, index=True)
    temperature = Column(Float)
    humidity = Column(Float)
    luminosity = Column(Float)
    timestamp = Column(DateTime, default=datetime.now)