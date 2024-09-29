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
import json
import os
from datetime import datetime 


current_directory = os.path.dirname(os.path.abspath(__file__))
json_filename = os.path.join(current_directory, "config.json")


def save_to_json():
    data = {
        "FREQUENCY": frequency_var.get(),
        "RDS_PTY": rds_pty_var.get(),
        "RDS_PS": rds_ps_var.get(),
        "RDS_RT": rds_rt_var.get(),
        "STEREO_MONO": stereo_mono_var.get(),
        "PRE_EMPHASIS": pre_emphasis_var.get(),
        "IMPEDANCE": impedance_var.get(),
        "BUFFER_GAIN": buffer_gain_var.get(),
        "FREQUENCY_DEVIATION": freq_dev_var.get(),
        "SOFT_CLIP": soft_clip_var.get(),
        "DATETIME": datetime_var.get()
    }
    with open(json_filename, 'w') as f:
        json.dump(data, f, indent=4)
    print("parameters saved in config.json")

def load_from_json():
    if os.path.exists(json_filename):
        with open(json_filename, 'r') as f:
            data = json.load(f)
            # Preenche as variáveis Tkinter com os valores do JSON
            frequency_var.set(data.get("FREQUENCY", "106.9"))
            rds_pty_var.set(data.get("RDS_PTY", "No program"))
            rds_ps_var.set(data.get("RDS_PS", "PU2CLR"))
            rds_rt_var.set(data.get("RDS_RT", "QN8066 ESP32 Arduino Library"))
            stereo_mono_var.set(data.get("STEREO_MONO", "Stereo"))
            pre_emphasis_var.set(data.get("PRE_EMPHASIS", "70us"))
            impedance_var.set(data.get("IMPEDANCE", "20K"))
            buffer_gain_var.set(data.get("BUFFER_GAIN", "6dB"))
            freq_dev_var.set(data.get("FREQUENCY_DEVIATION", "74.5"))
            soft_clip_var.set(data.get("SOFT_CLIP", "Disable"))
            datetime_var.set(data.get("DATETIME", datetime.now().strftime("%Y/%m/%d %H:%M")))
            print("Configuração carregada do arquivo JSON.")          
    else:
        print("Using default values")

def on_change(*args):
    save_to_json()


# Function to send data via socket to the ESP32.
# Change the IP below to the address indicated in the Arduino sketch linked to this application.
def send_to_esp32(field, value):
    try:
        # The IP information can be get usind the Arduino IDE (Serial Monitor) 
        esp32_ip = '10.0.0.143'  # ESP32 IP - Check it in the Arduino IDE Serial Monitor (console)
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




def send_frequency():
    frequency = frequency_var.get()
    send_to_esp32("frequency", frequency)

def send_rds_pty():
    selected_description = rds_pty_combobox.get()
    selected_value = pty_map[selected_description]
    print(f"Program Type (PTY): {selected_value} ({selected_description})")
    send_to_esp32("rds_pty", selected_value)

def send_rds_ps():
    rds_ps = rds_ps_var.get()
    send_to_esp32("rds_ps", rds_ps)

def send_rds_rt():
    rds_rt = rds_rt_var.get()
    send_to_esp32("rds_rt", rds_rt)

def send_stereo_mono():
    selected_description = stereo_mono_combobox.get()
    selected_value = stereo_mono_map[selected_description]
    print(f"Selected Stereo/Mono: {selected_value} ({selected_description})")
    send_to_esp32("stereo_mono", selected_value)

def send_pre_emphasis():
    selected_description = pre_emphasis_combobox.get()  
    selected_value = pre_emphasis_map[selected_description]  # Obtém o valor numérico correspondente
    print(f"Pre-Emphasis: {selected_value} ({selected_description})") 
    send_to_esp32("pre_emphasis", selected_value)

def send_impedance():
    selected_description = impedance_combobox.get()  # Obtém a descrição selecionada
    selected_value = impedance_map[selected_description]  # Obtém o valor numérico correspondente
    print(f"Selected Impedance: {selected_value} ({selected_description})")
    send_to_esp32("impedance", selected_value)    

def send_buffer_gain():
    selected_description = buffer_gain_combobox.get()  
    selected_value = buffer_gain_map[selected_description]  # Obtém o valor numérico correspondente
    print(f"Selected Buffer Gain: {selected_value} ({selected_description})")    
    send_to_esp32("buffer_gain", selected_value)  

def send_freq_dev():
    freq_dev = freq_dev_var.get()
    send_to_esp32("freq_dev", freq_dev)  

def send_soft_clip():
    selected_description = soft_clip_combobox.get()  
    selected_value = soft_clip_map[selected_description]  
    print(f"Selected Soft CLip: {selected_value} ({selected_description})")  
    send_to_esp32("soft_clip", selected_value)  

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


load_from_json()

frequency_var.trace("w", on_change)
rds_pty_var.trace("w", on_change)
rds_ps_var.trace("w", on_change)
rds_rt_var.trace("w", on_change)
stereo_mono_var.trace("w", on_change)
pre_emphasis_var.trace("w", on_change)
buffer_gain_var.trace("w", on_change)
impedance_var.trace("w", on_change)
freq_dev_var.trace("w", on_change)
soft_clip_var.trace("w", on_change)
datetime_var.trace("w", on_change)


label_fg = '#FFFF00'  
entry_bg = '#004d00'  
entry_fg = '#FFFF00' 

impedance_map = {
    '10K': 0,
    '20K': 1,
    '40K': 2,
    '80K': 3
}

pty_map = {'No program':0,
           'News':1, 
           'Information':3, 
           'Sport':4, 
           'Education':5, 
           'Culture':7, 
           'Science':8, 
           'Pop Music':10, 
           'Weather':16, 
           'Religion':20, 
           'Documentary':29, 
           'Alarm':30}

stereo_mono_map = {'Stereo':0,'Mono':1}
pre_emphasis_map = {'50us':0,'75us':1}
buffer_gain_map = {'3d{B':0,'6dB':1,'9dB':2,'12dB':3,'15dB':4,'18dB':5}
soft_clip_map = {'Disable':0,'Enable':1}

impedance_descriptions = list(impedance_map.keys())
pty_descriptions = list(pty_map.keys())
stereo_mono_descriptions = list(stereo_mono_map.keys())
pre_emphasis_descriptions = list(pre_emphasis_map.keys())
buffer_gain_descriptions = list(buffer_gain_map.keys())
soft_clip_descriptions = list(soft_clip_map.keys())

# Forms Layout 
tk.Label(root, text="Transmission Frequency (MHz):", bg='#006400', fg=label_fg).grid(row=0, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=frequency_var).grid(row=0, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_frequency).grid(row=0, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PTY:",bg='#006400', fg=label_fg).grid(row=1, column=0, sticky=tk.E, padx=10, pady=5)

# Combobox
rds_pty_combobox = ttk.Combobox(root, textvariable=rds_pty_var, values=pty_descriptions)
rds_pty_combobox.grid(row=1, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_pty).grid(row=1, column=2, padx=10, pady=5)

tk.Label(root, text="RDS PS:", bg='#006400', fg=label_fg).grid(row=2, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_ps_var).grid(row=2, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_ps).grid(row=2, column=2, padx=10, pady=5)

tk.Label(root, text="RDS RT:", bg='#006400', fg=label_fg).grid(row=3, column=0, sticky=tk.E, padx=10, pady=5)
tk.Entry(root, textvariable=rds_rt_var).grid(row=3, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_rds_rt).grid(row=3, column=2, padx=10, pady=5)

tk.Label(root, text="Stereo/Mono:", bg='#006400', fg=label_fg).grid(row=4, column=0, sticky=tk.E, padx=10, pady=5)
stereo_mono_combobox = ttk.Combobox(root, textvariable=stereo_mono_var, values= stereo_mono_descriptions)
stereo_mono_combobox.grid(row=4, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_stereo_mono).grid(row=4, column=2, padx=10, pady=5)

tk.Label(root, text="Pre-Emphasis:", bg='#006400', fg=label_fg).grid(row=5, column=0, sticky=tk.E, padx=10, pady=5)
pre_emphasis_combobox = ttk.Combobox(root, textvariable=pre_emphasis_var, values=pre_emphasis_descriptions)
pre_emphasis_combobox.grid(row=5, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_pre_emphasis).grid(row=5, column=2, padx=10, pady=5)

tk.Label(root, text="Impedance:", bg='#006400', fg=label_fg).grid(row=6, column=0, sticky=tk.E, padx=10, pady=5)
impedance_combobox = ttk.Combobox(root, textvariable=impedance_var, values = impedance_descriptions)

impedance_combobox.grid(row=6, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_impedance).grid(row=6, column=2, padx=10, pady=5)


tk.Label(root, text="Buffer Gain:", bg='#006400', fg=label_fg).grid(row=7, column=0, sticky=tk.E, padx=10, pady=5)
buffer_gain_combobox = ttk.Combobox(root, textvariable=buffer_gain_var, values = buffer_gain_descriptions) 

buffer_gain_combobox.grid(row=7, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_buffer_gain).grid(row=7, column=2, padx=10, pady=5)

tk.Label(root, text="Frequency Deviation (kHz):", bg='#006400', fg=label_fg).grid(row=8, column=0, sticky=tk.E, padx=10, pady=5)
freq_dev_combobox = ttk.Combobox(root, textvariable=freq_dev_var)
freq_dev_combobox['values'] = ['41.5', '60.0', '74.5','92.8','96.6', '110.4']
freq_dev_combobox.grid(row=8, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_freq_dev).grid(row=8, column=2, padx=10, pady=5)

tk.Label(root, text="Soft Clip:", bg='#006400', fg=label_fg).grid(row=9, column=0, sticky=tk.E, padx=10, pady=5)
soft_clip_combobox = ttk.Combobox(root, textvariable=soft_clip_var,values=soft_clip_descriptions)
soft_clip_combobox.grid(row=9, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_soft_clip).grid(row=9, column=2, padx=10, pady=5)

tk.Label(root, text="Set Date and Time (YYYY/MM/DD HH:MM):", bg='#006400', fg=label_fg).grid(row=10, column=0, padx=10, pady=5)
tk.Entry(root, textvariable=datetime_var).grid(row=10, column=1, padx=10, pady=5)
tk.Button(root, text="Set", command=send_datetime).grid(row=10, column=2, padx=10, pady=5)


# Start interface
root.mainloop()
