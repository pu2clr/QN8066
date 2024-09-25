import socket

# Função para processar o comando recebido
def process_command(command):
    command = command.strip()  # Remove espaços em branco no final/início
    separator_index = command.find('=')
    
    if separator_index == -1:
        return "Invalid command"
    
    field = command[:separator_index]  # Campo
    value = command[separator_index + 1:]  # Valor
    
    # Processa o comando e retorna uma resposta adequada
    if field == "frequency":
        return f"Frequency set to: {value}"
    elif field == "rds_pty":
        return f"RDS PTY set to: {value}"
    elif field == "rds_ps":
        return f"RDS PS set to: {value}"
    elif field == "rds_rt":
        return f"RDS RT set to: {value}"
    elif field == "stereo_mono":
        return f"Stereo/Mono set to: {value}"
    elif field == "pre_emphasis":
        return f"Pre-Emphasis set to: {value}"
    elif field == "impedance":
        return f"Impedance set to: {value}"
    elif field == "buffer_gain":
        return f"Buffer Gain set to: {value}"
    elif field == "freq_dev":
        return f"Frequency Deviation set to: {value}"
    elif field == "soft_clip":
        return f"Soft Clip set to: {value}"
    elif field == "datetime":
        return f"Date and Time set to: {value}"
    else:
        return "Unknown command"

# Inicia o servidor socket
def start_server():
    host = '0.0.0.0'  # Aceita conexões de qualquer interface de rede
    port = 8066  # Porta do servidor, deve corresponder à do cliente

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)

    print(f"Server listening on {host}:{port}...")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address}")

        while True:
            data = client_socket.recv(1024)
            if not data:
                break

            command = data.decode()
            print(f"Received command: {command}")

            # Processa o comando e obtém a resposta
            response = process_command(command)
            client_socket.send(response.encode())

        client_socket.close()
        print(f"Connection closed with {client_address}")

# Inicia o servidor
if __name__ == "__main__":
    start_server()
