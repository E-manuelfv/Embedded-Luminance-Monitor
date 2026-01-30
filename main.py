import json
import paho.mqtt.client as mqtt
import firebase_admin
from firebase_admin import credentials, db

# ---------- FIREBASE ----------
cred = credentials.Certificate("firebase-adminsdk.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://monitor-ldr---esp32-default-rtdb.firebaseio.com'
})

def enviar_leitura_ldr(device_id, raw_value, percent_value, status_text):
    ref = db.reference(f'ldr_readings/{device_id}')
    ref.push({
        'raw': raw_value,
        'percent': percent_value,
        'status': status_text,
        'timestamp': db.ServerValue.TIMESTAMP
    })
    print("✔ Dados enviados ao Firebase")

# ---------- MQTT ----------
MQTT_BROKER = "localhost"   # ou IP do PC
MQTT_PORT = 1883
MQTT_TOPIC = "iot/esp32/ldr"

def on_connect(client, userdata, flags, rc):
    print("Conectado ao MQTT com código:", rc)
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print("📥 Mensagem recebida:", msg.payload.decode())

    try:
        data = json.loads(msg.payload.decode())

        enviar_leitura_ldr(
            device_id=data["device_id"],
            raw_value=data["raw"],
            percent_value=data["percent"],
            status_text=data["status"]
        )

    except Exception as e:
        print("Erro ao processar mensagem:", e)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()
