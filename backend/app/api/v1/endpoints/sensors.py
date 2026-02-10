from fastapi import APIRouter, Depends
from sqlalchemy.orm import Session
from app.core.database import SessionLocal
from app.models.sensor import SensorData

router = APIRouter()

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@router.get("/sensors/latest")
def get_latest_sensor(db: Session = Depends(get_db)):
    data = (
        db.query(SensorData)
        .order_by(SensorData.created_at.desc())
        .first()
    )

    if not data:
        return {}

    return {
        "device_id": data.device_id,
        "temperature": data.temperature,
        "humidity": data.humidity,
        "luminosity": data.luminosity,
        "timestamp": data.created_at.isoformat()
    }