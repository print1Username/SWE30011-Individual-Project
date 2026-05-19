from flask import Flask, jsonify, render_template_string
import serial
import threading

app = Flask(__name__)

port = 'COM5'

# If have connect port, uncomment below
# ser = serial.Serial(port, 9600, timeout=1)

data = {
    "light": 0,
    "distance": 0,
    "fan": False,
    "system": False
}

# ================= SERIAL READER =================
def read_serial():
    global data
    while True:
        try:
            line = ser.readline().decode().strip()

            if "Light:" in line:
                parts = line.split("|")

                light = int(parts[0].split(":")[1])
                distance = float(parts[1].split(":")[1])
                fan = parts[2].split(":")[1].strip() == "1"
                system = parts[3].split(":")[1].strip() == "1"

                data = {
                    "light": light,
                    "distance": distance,
                    "fan": fan,
                    "system": system
                }

        except:
            pass

threading.Thread(target=read_serial, daemon=True).start()

# ================= WEB PAGE =================
@app.route('/')
def index():
    return render_template_string("""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Smart Home Dashboard</title>
        <style>
            body {
                font-family: Arial;
                background: #111;
                color: white;
                text-align: center;
            }
            .card {
                background: #222;
                margin: 20px auto;
                padding: 20px;
                border-radius: 15px;
                width: 300px;
                box-shadow: 0 0 15px rgba(0,0,0,0.5);
            }
            h1 {
                color: #00ffcc;
            }
            .value {
                font-size: 30px;
                margin: 10px;
            }
            .on { color: #00ff00; }
            .off { color: #ff4444; }
        </style>
    </head>
    <body>

        <h1>Smart Home Dashboard</h1>

        <div class="card">
            <h2>Light</h2>
            <div class="value" id="light">0</div>
        </div>

        <div class="card">
            <h2>Distance (cm)</h2>
            <div class="value" id="distance">0</div>
        </div>

        <div class="card">
            <h2>Fan</h2>
            <div class="value" id="fan">OFF</div>
        </div>

        <div class="card">
            <h2>System</h2>
            <div class="value" id="system">OFF</div>
        </div>

        <script>
            async function fetchData() {
                const res = await fetch('/data');
                const d = await res.json();

                document.getElementById('light').innerText = d.light;
                document.getElementById('distance').innerText = d.distance.toFixed(1);

                let fan = document.getElementById('fan');
                fan.innerText = d.fan ? "ON" : "OFF";
                fan.className = "value " + (d.fan ? "on" : "off");

                let system = document.getElementById('system');
                system.innerText = d.system ? "ON" : "OFF";
                system.className = "value " + (d.system ? "on" : "off");
            }

            setInterval(fetchData, 500);
        </script>

    </body>
    </html>
    """)

# ================= API =================
@app.route('/data')
def get_data():
    return jsonify(data)

app.run(host='0.0.0.0', port=5000)