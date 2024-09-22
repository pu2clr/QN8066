# Controlling QN8066 via Python 

This folder contains a Python application that can be run on Windows, Linux, or Mac OS operating systems and allows the user to control a transmitter based on the QN8066. The idea behind this application is similar to the one presented in the "07_WEB_WIFI_CONTROL" folder of this repository, but instead of using a web browser and HTML, this application uses sockets to establish the connection between the client application (Python) and the ESP32.


The ESP32 is an excellent choice for socket-based applications due to its robust networking capabilities, high performance, and ease of integration with Python. Whether you're building a home automation system, a remote control interface, or a data monitoring tool, the combination of ESP32 and Python provides a flexible and powerful solution for IoT and real-time communication projects.



### Setting Up Python and Tkinter on Windows, Linux (Debian-based), and Mac OS

To run the Python application with a graphical user interface (GUI) using Tkinter, it's essential to ensure that both Python and Tkinter are installed and properly configured. Tkinter is the standard Python library for creating GUIs, and it's typically included with most Python installations. Below are the instructions for configuring Python and Tkinter on different platforms.

### 1. Python Version
- Ensure that you are using **Python 3.7** or higher. Tkinter is included with most modern versions of Python, but you may need to install it separately on some platforms.

---

### A. Windows Setup

#### Step 1: Install Python
1. Download Python: Go to the [official Python website](https://www.python.org/downloads/) and download the latest Python 3.x version.
2. Install Python:
   - Run the installer.
   - Ensure you check the box that says **"Add Python to PATH"**.
   - Click on **Install Now** and follow the prompts.

#### Step 2: Verify Python and Tkinter Installation
1. Open **Command Prompt** and type:
   ```bash
   python --version
   ```
   This should display the installed version of Python (e.g., `Python 3.9.5`).
   
2. To verify if Tkinter is installed, open a Python prompt by typing `python` and run:
   ```python
   import tkinter
   tkinter._test()
   ```
   This should open a small Tkinter window if everything is set up correctly.

#### Step 3: Install Tkinter (if needed)
- Tkinter is included with Python on Windows. However, if Tkinter is not installed, you can add it manually:
   - Open **Command Prompt** and run:
     ```bash
     pip install tk
     ```

---

### B. Linux (Debian-based, such as Ubuntu) Setup

#### Step 1: Install Python
1. Open a terminal and ensure Python 3 is installed by running:
   ```bash
   sudo apt update
   sudo apt install python3
   ```
2. Check the installed version by running:
   ```bash
   python3 --version
   ```

#### Step 2: Install Tkinter
Tkinter is not always installed by default on Linux systems. To install it, run the following command:
```bash
sudo apt install python3-tk
```

#### Step 3: Verify Tkinter Installation
Once Tkinter is installed, verify it by running the following command in a Python environment:
```bash
python3 -m tkinter
```
This should open a small Tkinter window if the installation was successful.

---

### C. Mac OS Setup

#### Step 1: Install Python
Mac OS often includes a version of Python 2.x, so you will need to install Python 3:
1. **Download Python** from the [official Python website](https://www.python.org/downloads/).
2. Run the installer and follow the prompts.

Alternatively, you can install Python using **Homebrew** (a package manager for Mac OS):
1. Install Homebrew by running:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
2. Use Homebrew to install Python 3:
   ```bash
   brew install python3
   ```

#### Step 2: Verify Python Installation
Open a terminal and check the Python version:
```bash
python3 --version
```

#### Step 3: Verify Tkinter Installation
Tkinter usually comes with Python on Mac OS. To verify Tkinter, run:
```bash
python3 -m tkinter
```
This will open a small window if Tkinter is installed correctly.

#### Step 4: Install Tkinter (if needed)
If Tkinter is not installed, you can add it using Homebrew:
```bash
brew install python-tk
```

---

### Common Steps for All Platforms

#### **Installing Additional Dependencies with pip**
You may need to install additional Python libraries, such as `socket` or `ttk` (part of Tkinter). To install these, use **pip**, which is included with Python 3.

To install libraries, open a terminal (Command Prompt on Windows) and run:
```bash
pip install some_package_name
```

For example, to install the **socket** library (though it's included with Python):
```bash
pip install socket
```

---

### Running the Application

Once Python and Tkinter are installed and verified, you can run your Python application by navigating to the folder where your script is located and running:

```bash
python3 your_script.py
```

- On **Windows**, you can use `python your_script.py`.
- On **Linux (Debian-based)** and **Mac OS**, use `python3 your_script.py`.

This will launch your GUI application built with Tkinter and communicate with the ESP32.

