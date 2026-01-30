from flask import Flask, render_template, jsonify
import firebase_admin
from firebase_admin import credentials, firestore
import os


app = Flask(__name__)

# Firebase (evita inicializar duas vezes)

if not firebase_admin._apps:
    cred = credentials.Certificate(
        os.getenv(
            "FIREBASE_CREDENTIALS",
            "/etc/secrets/serviceAccountKey.json"
        )
    )

    firebase_admin.initialize_app(cred, {
        "databaseURL": "https://monitor-ldr---esp32-default-rtdb.firebaseio.com"
    })

db = firestore.client()

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/data")
def get_data():
    docs = db.collection("ldr_data") \
             .order_by("timestamp", direction=firestore.Query.DESCENDING) \
             .limit(10) \
             .stream()

    data = []
    for doc in docs:
        data.append(doc.to_dict())

    return jsonify(data)

@app.route("/health")
def health():
    return {"status": "dashboard online"}

if __name__ == "__main__":
    app.run(debug=True)
