import threading
from dashboard.app import app
from worker.mqtt_worker import start_worker

def run_worker():
    start_worker()

if __name__ == "__main__":
    t = threading.Thread(target=run_worker)
    t.daemon = True
    t.start()

    app.run(host="0.0.0.0", port=5000)
