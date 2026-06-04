# SWE30011 Individual Project

This project presents a basic smart home system developed using an Arduino Uno and several sensors and actuators to improve convenience, security, and energy efficiency.
\
The system integrates components such as an LDR, ultrasonic sensor, tilt sensor, LED, cooling fan, buzzer, pushbutton, and relay to perform automated tasks based on environmental conditions. 
\
The Arduino Uno acts as the main controller, processing sensor inputs and controlling the connected devices in real time. 
\
Features of the system include automatic lighting, smart ventilation, and basic security alerts, demonstrating the fundamental concepts of IoT-based home automation.

## Requirements

- [Python](https://www.python.org/)
- [Arduino IDE](https://www.arduino.cc/en/software/)

Recommended Python Version:

```text
Python 3.12.x
```

---

## Environment Installation

### Windows

1. Install Python
2. Install Arduino IDE

Verify installation:

```bash
python --version
```

---

### Linux (Ubuntu / Debian)

#### 1. Update packages

```bash
sudo apt update
sudo apt upgrade
```

#### 2. Install Python and required packages

```bash
sudo apt install python3 python3-venv python3-pip
```

Verify installation:

```bash
python3 --version
```

---

## Python Virtual Environment

### 1. Create virtual environment

```bash
python -m venv .venv
```


### 2. Activate virtual environment

#### Windows

```bash
.venv/Scripts/activate
```

#### Linux

```bash
source .venv/bin/activate
```

After activation, your terminal should display:

```bash
(.venv) C:\...
```


## 3. Install dependencies (using requirements.txt)

```bash
pip install -r requirements.txt
```


## Run Project

### Windows

```bash
python app.py
```


## Exit Virtual Environment

```bash
deactivate
```


## Arduino Setup
1. Open Arduino IDE
2. Open Arduino project / `.ino` file
3. Select board and COM port
4. Upload firmware

## Project Structure

```text
project-folder/
├── .venv/
├── arduino/
│   └── arduino.ino
├── templates/
│   └── index.html
├── main.py
├── requirements.txt
└── README.md
```