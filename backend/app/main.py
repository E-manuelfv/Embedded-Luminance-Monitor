from fastapi import FastAPI
from .api.routes import router

app = FastAPI(
    title = "IoT Monitoring API",
    version="1.0.0",
    description="API para leitura de dados IoT"
)

app.include_router(router, prefix="/api/v1")