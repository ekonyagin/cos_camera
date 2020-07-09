import socket
import json

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        
        a = conn.recv(2048)
        data = None

        data = {
                    "Type" : "GetStatus"
        }
        conn.sendall(json.dumps(data).encode())
        print("OK")
        a = conn.recv(2048)
        print(json.loads(a.decode()))
        with open("../jsons/add_task_template.json") as f:
            data = json.load(f)
        conn.sendall(json.dumps(data).encode())
        a = conn.recv(2048)
        print(a.decode())