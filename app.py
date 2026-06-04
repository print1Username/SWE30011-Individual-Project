from flask import Flask, jsonify, render_template, request
import threading
import time

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    serial = None

app = Flask(__name__)

# =====================================================
# GLOBAL STATE
# =====================================================

ser = None
arduino_connected = False

data = {
    "light": 0,
    "distance": 0.0,

    "fan": False,
    "fan_mode": "AUTO",

    "led": False,
    "led_mode": "AUTO",

    "buzzer": True,

    "system": False,

    "arduino": False
}

lock = threading.Lock()

# =====================================================
# ARDUINO DETECTION
# =====================================================

def find_arduino():

    if serial is None:
        return None

    ports = serial.tools.list_ports.comports()

    for port in ports:

        desc = str(port.description).lower()

        keywords = [
            "arduino",
            "ch340",
            "usb serial",
            "cp210"
        ]

        if any(k in desc for k in keywords):
            return port.device

    return None


def connect_arduino():
    global ser, arduino_connected

    while True:

        if arduino_connected:
            time.sleep(5)
            continue

        try:
            port = find_arduino()

            if port:
                print(f"[Arduino] Trying {port}")

                ser = serial.Serial(
                    port=port,
                    baudrate=9600,
                    timeout=1,
                    write_timeout=1
                )

                time.sleep(2)

                arduino_connected = True
                with lock:
                    data["arduino"] = True

                print("[Arduino] Connected")

        except Exception as e:
            print("[Arduino] Connect Error:", e)

            arduino_connected = False
            with lock:
                data["arduino"] = False

            time.sleep(3)

# =====================================================
# SERIAL READER
# =====================================================

def serial_reader():

    global arduino_connected

    while True:

        try:

            if not arduino_connected or ser is None:
                time.sleep(1)
                continue

            line = ser.readline().decode(
                errors="ignore"
            ).strip()

            if not line:
                continue

            parts = line.split("|")

            parsed = {}

            for item in parts:

                if ":" not in item:
                    continue

                key, value = item.split(":", 1)

                parsed[key] = value

            with lock:

                data["light"] = int(
                    parsed.get("Light", 0)
                )

                data["distance"] = float(
                    parsed.get("Distance", 0)
                )

                data["fan"] = (
                    parsed.get("Fan", "0") == "1"
                )

                data["fan_mode"] = parsed.get(
                    "FanMode",
                    "AUTO"
                )

                data["led"] = (
                    parsed.get("LED", "0") == "1"
                )

                data["led_mode"] = parsed.get(
                    "LEDMode",
                    "AUTO"
                )

                data["buzzer"] = (
                    parsed.get("Buzzer", "1") == "1"
                )

                data["system"] = (
                    parsed.get("System", "0") == "1"
                )

                data["arduino"] = True

        except Exception as e:

            print("[Serial Error]", e)

            arduino_connected = False

            with lock:
                data["arduino"] = False

            time.sleep(2)

# =====================================================
# COMMAND SENDER
# =====================================================

def send_command(cmd):

    global arduino_connected

    print("[CMD]", cmd)

    try:

        if arduino_connected and ser:

            ser.write(
                (cmd + "\n").encode()
            )

    except Exception as e:

        print("[Send Error]", e)

        arduino_connected = False

        with lock:
            data["arduino"] = False

# =====================================================
# SIMULATION MODE
# =====================================================

def update_simulation(cmd):

    with lock:

        if cmd == "SYSTEM_ON":
            data["system"] = True

        elif cmd == "SYSTEM_OFF":
            data["system"] = False

        elif cmd == "FAN_ON":
            data["fan_mode"] = "ON"
            data["fan"] = True

        elif cmd == "FAN_OFF":
            data["fan_mode"] = "OFF"
            data["fan"] = False

        elif cmd == "FAN_AUTO":
            data["fan_mode"] = "AUTO"

        elif cmd == "LED_ON":
            data["led_mode"] = "ON"
            data["led"] = True

        elif cmd == "LED_OFF":
            data["led_mode"] = "OFF"
            data["led"] = False

        elif cmd == "LED_AUTO":
            data["led_mode"] = "AUTO"

        elif cmd == "BUZZER_ENABLE":
            data["buzzer"] = True

        elif cmd == "BUZZER_DISABLE":
            data["buzzer"] = False

# =====================================================
# ROUTES
# =====================================================

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/data")
def get_data():

    with lock:
        return jsonify(data)


@app.route("/command", methods=["POST"])
def command():

    payload = request.json

    cmd = payload.get("command")

    send_command(cmd)

    if not arduino_connected:
        update_simulation(cmd)

    return jsonify({
        "success": True
    })

# =====================================================
# START THREADS
# =====================================================

threading.Thread(
    target=connect_arduino,
    daemon=True
).start()

threading.Thread(
    target=serial_reader,
    daemon=True
).start()

# =====================================================
# MAIN
# =====================================================

if __name__ == "__main__":

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False,
        use_reloader=False
    )