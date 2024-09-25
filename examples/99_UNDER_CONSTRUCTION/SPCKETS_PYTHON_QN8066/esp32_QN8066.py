# This program uses a socket connection to communicate with the ESP32 (running the 
# SOCKETS_ESP32_QN8066.ino sketch) in order to control the QN8066-based transmitter.
# The socket connection uses the IP provided by your WiFi network's DHCP and obtained 
# by the ESP32. Check the IP in the Arduino IDE console (Serial Monitor). 
# The port numer used here is 8066 (SOCKETS_ESP32_QN8066.ino). You can change it if you need it.
# 
# RDS message updates such as PTY, PS, RT, and Time are not executed immediately. 
# This may depend on the receiver's update timing as well as the distribution of 
# each message's timing from the connected controller.
#
# defined for the connection is 8066.
# Author: Ricardo Lima Caratti - Sep. 2024

import tkinter as tk
from tkinter import ttk
import socket
from datetime import datetime 

# Function to send data via socket to the ESP32.
# Change the IP below to the address indicated in the Arduino sketch linked to this application.
def send_to_esp32(field, value):
    try:
        # The IP information can be get usind the Arduino IDE (Serial Monitor) 
        esp32_ip = '10.0.0.34'  # ESP32 IP - Check it in the Arduino IDE Serial Monitor (console)
        esp32_port = 8066  # Defined in the ESP 32 Arduino Sketch 
        message = f"{field}={value}\n"
        
        # Connects to the ESP32 and sends the message.
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((esp32_ip, esp32_port))
            s.sendall(message.encode())
            response = s.recv(1024)
            print(f'Received from ESP32 ({field}):', response.decode())
    except socket.timeout:
        print(f"Connection to ESP32 timed out. The device may be offline.")
    except ConnectionRefusedError:
        print(f"Connection to ESP32 was refused. Is the device online?")
    except socket.error as e:
        print(f"Socket error occurred: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")        

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

def send_freq_dev():
    freq_dev = freq_dev_var.get()
    send_to_esp32("freq_dev", freq_dev)  

def send_soft_clip():
    soft_clip = soft_clip_var.get()
    send_to_esp32("soft_clip", soft_clip)  

def send_datetime():
    datetime_str = datetime_var.get()
    send_to_esp32("datetime", datetime_str)    


# Creating the main window with Tkinter.
root = tk.Tk()
root.title("ESP32 QN8066 FM Transmitter Control")
root.configure(bg='#006400')  # Green

# Fields
frequency_var = tk.StringVar(value = "106.9")
rds_pty_var = tk.StringVar(value = "No program")
rds_ps_var = tk.StringVar(value="PU2CLR")
rds_rt_var = tk.StringVar(value="QN8066 Arduino Library")
stereo_mono_var = tk.StringVar(value = "Stereo")
pre_emphasis_var = tk.StringVar(value = "70us")
buffer_gain_var = tk.StringVar(value = "6dB")
impedance_var = tk.StringVar(value = "20K")
freq_dev_var = tk.StringVar(value = "74.5")
soft_clip_var = tk.StringVar(value = "Disable")
datetime_var = tk.StringVar(value=datetime.now().strftime("%Y/%m/%d %H:%M") )

label_fg = '#FFFF00'  
entry_bg = '#004d00'  
entry_fg = '#FFFF00' 

impedance_map = {
    '10K': 0,
    '20K': 1,
    '40K': 2,
    '80K': 3
}

# Lista com apenas as descrições (segunda coluna) para exibir no combobox
impedance_descriptions = list(impedance_map.keys())

# Forms Layout 
tk.Label(root, text="Transmission Frequency (MHz):", bg='#006400', fg=label_fg).grid(row=0, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=frequency_var).grid(row=0, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_frequency).grid(row=0, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PTY:",bg='#006400', fg=label_fg).grid(row=1, column=0, sticky=tk.E, padx=10, pady=5)

# Combobox
rds_pty_combobox = ttk.Combobox(root, textvariable=rds_pty_var, values=list(({'No program':0,'News':1, 'Information':3, 'Sport':4, 'Education':5, 'Culture':7, 'Science':8, 'Pop Music':10, 'Weather':16, 'Religion':20, 'Documentary':29, 'Alarm':30}).keys()))
rds_pty_combobox.grid(row=1, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_pty).grid(row=1, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PS:", bg='#006400', fg=label_fg).grid(row=2, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_ps_var).grid(row=2, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_ps).grid(row=2, column=2, padx=10, pady=5)

tk.Label(root, text="RDS RT:", bg='#006400', fg=label_fg).grid(row=3, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_rt_var).grid(row=3, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_rt).grid(row=3, column=2, padx=10, pady=5)

tk.Label(root, text="Stereo/Mono:", bg='#006400', fg=label_fg).grid(row=4, column=0, sticky=tk.E, padx=10, pady=5)
stereo_mono_combobox = ttk.Combobox(root, textvariable=stereo_mono_var, values=list(({'Stereo':0,'Mono':1}).keys()))
stereo_mono_combobox.grid(row=4, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_stereo_mono).grid(row=4, column=2, padx=10, pady=5)

tk.Label(root, text="Pre-Emphasis:", bg='#006400', fg=label_fg).grid(row=5, column=0, sticky=tk.E, padx=10, pady=5)
pre_emphasis_combobox = ttk.Combobox(root, textvariable=pre_emphasis_var, values=list(({'50us':0,'75us':1}).keys()))
pre_emphasis_combobox.grid(row=5, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_pre_emphasis).grid(row=5, column=2, padx=10, pady=5)

tk.Label(root, text="Impedance:", bg='#006400', fg=label_fg).grid(row=6, column=0, sticky=tk.E, padx=10, pady=5)
impedance_combobox = ttk.Combobox(root, textvariable=impedance_var, values = impedance_descriptions)

impedance_combobox.grid(row=6, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_impedance).grid(row=6, column=2, padx=10, pady=5)


tk.Label(root, text="Buffer Gain:", bg='#006400', fg=label_fg).grid(row=7, column=0, sticky=tk.E, padx=10, pady=5)
buffer_gain_combobox = ttk.Combobox(root, textvariable=buffer_gain_var, values = list({'3dB':0,'6dB':1,'9dB':2,'12dB':3,'15dB':4,'18dB':5})) 

buffer_gain_combobox.grid(row=7, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_buffer_gain).grid(row=7, column=2, padx=10, pady=5)

tk.Label(root, text="Frequency Deviation (kHz):", bg='#006400', fg=label_fg).grid(row=8, column=0, sticky=tk.E, padx=10, pady=5)
freq_dev_combobox = ttk.Combobox(root, textvariable=freq_dev_var)
freq_dev_combobox['values'] = ['41.5', '60.0', '74.5','92.8','96.6', '110.4']
freq_dev_combobox.grid(row=8, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_freq_dev).grid(row=8, column=2, padx=10, pady=5)

tk.Label(root, text="Soft Clip:", bg='#006400', fg=label_fg).grid(row=9, column=0, sticky=tk.E, padx=10, pady=5)
soft_clip_var_combobox = ttk.Combobox(root, textvariable=soft_clip_var,values=list({'Disable':0,'Enable':1}))
soft_clip_var_combobox.grid(row=9, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_soft_clip).grid(row=9, column=2, padx=10, pady=5)

tk.Label(root, text="Set Date and Time (YYYY/MM/DD HH:MM):", bg='#006400', fg=label_fg).grid(row=10, column=0, padx=10, pady=5)
tk.Entry(root, textvariable=datetime_var).grid(row=10, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_datetime).grid(row=10, column=2, padx=10, pady=5)


# Start interface
root.mainloop()
