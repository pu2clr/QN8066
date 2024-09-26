import socket


pty_tab = {'0':'No program',
           '1':'News', 
           '3':'Information', 
           '4':'Sport', 
           '5':'Education', 
           '7':'Culture', 
           '8':'Science', 
           '10':'Pop Music', 
           '16':'Weather', 
           '20':'Religion', 
           '29':'Documentary', 
           '30':'Alarm'}


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
        print(f"Tuning at {value}MHz")
        return f"Frequency set to: {value}"
    elif field == "rds_pty":
        print(f"Set RDS PTY to {pty_tab[value]}")
        return f"RDS PTY set to: {value}"
    elif field == "rds_ps":
        print(f"Set RDS Program Station to {value}")
        return f"RDS PS set to: {value}"
    elif field == "rds_rt":
        print(f"Set RDS Radio Text to {value}")
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
    host = '127.0.0.1'  # Aceita conexões de qualquer interface de rede
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
