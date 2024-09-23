# This program uses a socket connection to communicate with the ESP32 (running the 
# SOCKETS_ESP32_QN8066.ino sketch) in order to control the QN8066-based transmitter.
# The socket connection uses the IP provided by your WiFi network's DHCP and obtained 
# by the ESP32. Check the IP in the Arduino IDE console (Serial Monitor). The port 
# defined for the connection is 8066.
# Author: Ricardo Lima Caratti - Sep. 2024

import tkinter as tk
from tkinter import ttk
import socket

# Function to send data via socket to the ESP32.
# Change the IP below to the address indicated in the Arduino sketch linked to this application.
def send_to_esp32(field, value):
    try:
        esp32_ip = '192.168.18.155'  # ESP32 IP - Check it in the Arduino IDE Serial Monitor (console)
        esp32_port = 8066  # Defined in the ESP 32 Arduino Sketch 
        message = f"{field}={value}\n"
        
        # Connects to the ESP32 and sends the message.
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((esp32_ip, esp32_port))
            s.sendall(message.encode())
            response = s.recv(1024)
            print(f'Received from ESP32 ({field}):', response.decode())
    except Exception as e:
        print(f'Error sending {field} to ESP32: {e}')

# Specific functions for each field.
def send_frequency():
    frequency = frequency_var.get()
    send_to_esp32("frequency", frequency)

def send_rds_pty():
    rds_pty = rds_pty_var.get()
    send_to_esp32("rds_pty", rds_pty)

def send_rds_ps():
    rds_ps = rds_ps_var.get()
    send_to_esp32("rds_ps", rds_ps)

def send_rds_rt():
    rds_rt = rds_rt_var.get()
    send_to_esp32("rds_rt", rds_rt)

def send_stereo_mono():
    stereo_mono = stereo_mono_var.get()
    send_to_esp32("stereo_mono", stereo_mono)

def send_pre_emphasis():
    pre_emphasis = pre_emphasis_var.get()
    send_to_esp32("pre_emphasis", pre_emphasis)

def send_impedance():
    impedance = impedance_var.get()
    send_to_esp32("impedance", impedance) 

def send_buffer_gain():
    buffer_gain = buffer_gain_var.get()
    send_to_esp32("buffer_gain", buffer_gain)  


# Creating the main window with Tkinter.
root = tk.Tk()
root.title("ESP32 QN8066 FM Transmitter Control")
root.configure(bg='#006400')  # Green

# Fields
frequency_var = tk.StringVar(value = "106.9")
rds_pty_var = tk.StringVar(value = "0 No program")
rds_ps_var = tk.StringVar(value="PU2CLR")
rds_rt_var = tk.StringVar(value="QN8066 Arduino Library")
stereo_mono_var = tk.StringVar(value = "0 Stereo")
pre_emphasis_var = tk.StringVar(value = "1 70us")
buffer_gain_var = tk.StringVar(value = "1 6dB")
impedance_var = tk.StringVar(value = "1 20K")

label_fg = '#FFFF00'  
entry_bg = '#004d00'  
entry_fg = '#FFFF00' 

# Forms Layout 
tk.Label(root, text="Transmission Frequency (MHz):", bg='#006400', fg=label_fg).grid(row=0, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=frequency_var).grid(row=0, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_frequency).grid(row=0, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PTY:",bg='#006400', fg=label_fg).grid(row=1, column=0, sticky=tk.E, padx=10, pady=5)

# Combobox
rds_pty_combobox = ttk.Combobox(root, textvariable=rds_pty_var)
rds_pty_combobox['values'] = [
    ('0', 'No program'),
    ('1', 'News'),
    ('3', 'Information'),
    ('4', 'Sport'),
    ('5', 'Education'),
    ('7', 'Culture'),
    ('8', 'Science'),
    ('10', 'Pop Music'),
    ('16', 'Weather'),
    ('20', 'Religion'),
    ('29', 'Documentary'),
    ('30', 'Alarm')
]
rds_pty_combobox.grid(row=1, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_pty).grid(row=1, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PS:", bg='#006400', fg=label_fg).grid(row=2, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_ps_var).grid(row=2, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_ps).grid(row=2, column=2, padx=10, pady=5)

tk.Label(root, text="RDS RT:", bg='#006400', fg=label_fg).grid(row=3, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_rt_var).grid(row=3, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_rt).grid(row=3, column=2, padx=10, pady=5)

tk.Label(root, text="Stereo/Mono:", bg='#006400', fg=label_fg).grid(row=4, column=0, sticky=tk.E, padx=10, pady=5)
stereo_mono_combobox = ttk.Combobox(root, textvariable=stereo_mono_var)
stereo_mono_combobox['values'] = [(0,'Stereo'),(1,'Mono')]
stereo_mono_combobox.grid(row=4, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_stereo_mono).grid(row=4, column=2, padx=10, pady=5)

tk.Label(root, text="Pre-Emphasis:", bg='#006400', fg=label_fg).grid(row=5, column=0, sticky=tk.E, padx=10, pady=5)
pre_emphasis_combobox = ttk.Combobox(root, textvariable=pre_emphasis_var)
pre_emphasis_combobox['values'] = [(0,'50us'),(1,'70us')]
pre_emphasis_combobox.grid(row=5, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_pre_emphasis).grid(row=5, column=2, padx=10, pady=5)

tk.Label(root, text="Impedance:", bg='#006400', fg=label_fg).grid(row=6, column=0, sticky=tk.E, padx=10, pady=5)
impedance_combobox = ttk.Combobox(root, textvariable=impedance_var)
impedance_combobox['values'] = [(0,'10K'),
                                  (1,'20K'),
                                  (2,'40K'),
                                  (3,'80K')]
impedance_combobox.grid(row=6, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_impedance).grid(row=6, column=2, padx=10, pady=5)


tk.Label(root, text="Buffer Gain:", bg='#006400', fg=label_fg).grid(row=7, column=0, sticky=tk.E, padx=10, pady=5)
buffer_gain_combobox = ttk.Combobox(root, textvariable=buffer_gain_var)
buffer_gain_combobox['values'] = [ (0, '3dB'),
                                   (1, '6dB'), 
                                   (2, '9dB'), 
                                   (3, '12dB'),  
                                   (4, '15dB'),  
                                   (5, '18dB') ]
buffer_gain_combobox.grid(row=7, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_buffer_gain).grid(row=7, column=2, padx=10, pady=5)


# Start interface
root.mainloop()