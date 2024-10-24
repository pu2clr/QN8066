import tkinter as tk
import asyncio
from bleak import BleakClient

# 08:3a:f2:52:92:b0
DEVICE_ADDRESS = "08:3a:f2:52:92:b0"  # Substituir com o endereço MAC do ESP32
CHARACTERISTIC_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb"  # UUID do serviço Bluetooth

# Função para enviar dados via Bluetooth usando Bleak
async def send_data():
    async with BleakClient(DEVICE_ADDRESS) as client:
        if client.is_connected:
            data = {
                "first_name": entry_first_name.get(),
                "last_name": entry_last_name.get(),
                "social_number": entry_social_number.get()
            }
            json_data = (json.dumps(data) + '\n').encode('utf-8')
            await client.write_gatt_char(CHARACTERISTIC_UUID, json_data)
            print("Dados enviados:", json_data)

# Função para receber dados do ESP32
async def receive_data():
    async with BleakClient(DEVICE_ADDRESS) as client:
        if client.is_connected:
            response = await client.read_gatt_char(CHARACTERISTIC_UUID)
            data = json.loads(response.decode('utf-8'))
            entry_first_name.delete(0, tk.END)
            entry_first_name.insert(0, data["first_name"])
            entry_last_name.delete(0, tk.END)
            entry_last_name.insert(0, data["last_name"])
            entry_social_number.delete(0, tk.END)
            entry_social_number.insert(0, data["social_number"])
            print("Dados recebidos:", data)

# Interface Gráfica com tkinter
root = tk.Tk()
root.title("Formulário Bluetooth")

tk.Label(root, text="First Name").grid(row=0, column=0)
entry_first_name = tk.Entry(root)
entry_first_name.grid(row=0, column=1)

tk.Label(root, text="Last Name").grid(row=1, column=0)
entry_last_name = tk.Entry(root)
entry_last_name.grid(row=1, column=1)

tk.Label(root, text="Social Number").grid(row=2, column=0)
entry_social_number = tk.Entry(root)
entry_social_number.grid(row=2, column=1)

tk.Button(root, text="Enviar", command=lambda: asyncio.run(send_data())).grid(row=3, column=0)
tk.Button(root, text="Receber", command=lambda: asyncio.run(receive_data())).grid(row=3, column=1)

root.mainloop()
