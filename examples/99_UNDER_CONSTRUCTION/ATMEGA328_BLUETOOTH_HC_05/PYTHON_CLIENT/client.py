import asyncio
from bleak import BleakClient

# Endereço MAC do módulo HC-05
ADDRESS = "00:00:00:00:00:00"  # Substitua pelo endereço correto

# UUID do serviço Bluetooth (pode variar, ajustar se necessário)
CHARACTERISTIC_UUID = "00001101-0000-1000-8000-00805F9B34FB"

async def send_data(first_name, last_name, social_number):
    async with BleakClient(ADDRESS) as client:
        if client.is_connected:
            print(f"Conectado ao dispositivo {ADDRESS}")
            data = f'{{"firstName": "{first_name}", "lastName": "{last_name}", "socialNumber": "{social_number}"}}\n'
            await client.write_gatt_char(CHARACTERISTIC_UUID, data.encode())
            print("Dados enviados:", data)

async def get_data():
    async with BleakClient(ADDRESS) as client:
        if client.is_connected:
            await client.write_gatt_char(CHARACTERISTIC_UUID, "GET_DATA\n".encode())
            response = await client.read_gatt_char(CHARACTERISTIC_UUID)
            print("Dados recebidos:", response.decode('utf-8'))

# Exemplo de execução
async def main():
    await send_data("John", "Doe", "123-45-6789")
    await asyncio.sleep(2)
    await get_data()

asyncio.run(main())
