import socket
import os
import json

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 65432        # The port used by the server

def GetStatus():
	with open("../jsons/status.json") as f:
		
		data = json.load(f)
	return json.dumps(data)

def AddTask(task):
	dir_name = "./data/id_" + str(task["TaskID"])
	if os.path.exists(dir_name) == False:
		os.mkdir(dir_name)
		with open(os.path.join(dir_name, "task.json"), 'w') as outfile:
		    json.dump(task, outfile)
		print("os.mkdir OK")
	else:
		raise ValueError(f"TaskID {task['TaskID']} already exists!")

def Transmit(task):
	pass

def Discard(task):
	pass

if __name__ == "__main__":
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	    s.connect((HOST, PORT))
	    s.sendall(b'Hello, world')
	    data = s.recv(1024).decode()
	    command = json.loads(data)
	    if command["Type"] == "GetStatus":
	    	print("Got command GetStatus()")
	    	s.sendall(GetStatus().encode())
	    else:
	    	print("Unknown command")
	    data = s.recv(1024).decode()
	    command = json.loads(data)
	    if command["Type"] == "AddTask":
	    	print("Got command AddTask()")
	    	AddTask(command["Task"])
	    	s.sendall(b"OK\n")
	    else:
	    	print("Unknown command")

	#print('Received', data)