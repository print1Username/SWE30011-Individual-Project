# SWE30011 Individual Project

> Project description here.

---

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
.venv/bin/activate
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
python main.py
```

---

## Exit Virtual Environment

```bash
deactivate
```


## Arduino Setup

1. Open Arduino IDE
2. Open Arduino project / `.ino` file
3. Select board and COM port
4. Upload firmware

---

## Project Structure

```text
project-folder/
│
├── .venv/
├── arduino/
│   └── main.ino
├── main.py
├── requirements.txt
└── README.md
```