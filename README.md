# SWE30011 Individual Project

## Install Environment
### Windows
[Python](https://www.python.org/)
\
[Arduino IDE](https://www.arduino.cc/en/software/)

---

### Linux
1. Update package:
```bash
sudo apt update
sudo apt upgrade
```

2. Install Python:
```bash
sudo apt install python3
```

## Python virtual Environment deploy / activate
1. Create virtual environment
```bash
python -m venv .venv
```

2. Activate virtual environment
```bash
# Windows
.venv/bin/activate

# Linux
source .venv/bin/activate
```

3. Install pip requirements
```bash
pip install flask
```

4. Run python file
```bash
python main.py
```