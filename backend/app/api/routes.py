from fastapi import APIRouter
from backend.app.services.queries import get_last_readings

router = APIRouter()

@router.get("/sensor/last")
def read_last_data():
    return get_last_readings

@router.get("/health")
def health():
    return {"status": "ok"}