import os
import json
import time
import paho.mqtt.client as mqtt
import firebase_admin
from firebase_admin import credentials, db

# ======================
# Firebase (Realtime DB)
# ======================
if not firebase_admin._apps:
    cred_path = os.getenv(
        "FIREBASE_CREDENTIALS",
        "/etc/secrets/serviceAccountKey.json"
    )

    cred = credentials.Certificate(cred_path)

    firebase_admin.initialize_app(cred, {
        "databaseURL": "https://monitor-ldr---esp32-default-rtdb.firebaseio.com"
    })

# Referência base no Realtime Database
ldr_ref = db.reference("ldr_data")

# ======================
# MQTT CONFIG
# ======================
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "esp32/ldr"

def on_connect(client, userdata, flags, rc):
    print("✅ Conectado ao MQTT | Código:", rc)
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        print("📩 Mensagem recebida:", payload)

        ldr_ref.push({
            "value": payload.get("ldr"),
            "device": payload.get("device", "esp32-c3"),
            "timestamp": int(time.time() * 1000)
        })

    except Exception as e:
        print("❌ Erro ao processar mensagem:", e)

def start_worker():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()

if __name__ == "__main__":
    start_worker()
