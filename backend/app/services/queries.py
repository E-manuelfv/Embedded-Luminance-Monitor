from backend.app.models.sensor import SensorData
from backend.app.core.database import SessionLocal

def get_last_readings():
    db = SessionLocal()
    data = (
        db.query(SensorData)
        .order_by(SensorData.timestamp.desc())
        .limit(50)
        .all()
    )

    return data