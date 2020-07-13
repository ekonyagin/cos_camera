import socket
import os
import json
import struct

HOST = '172.16.48.141'   # The server's hostname or IP address
PORT = 5006        # The port used by the server

def GetStatus():
	with open("status.json") as f:
		print("opening status.json..")
		data = json.load(f)
	data = json.dumps(data)
	L = len(data)
	L_bin = struct.pack('<l', L)
	print(L, L_bin+data.encode())
	return L_bin + data.encode()

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
	    while(True):
		    data = s.recv(1024)
		    print(data)
		    l = data[:4]
		    print(struct.unpack('<l', l), l)
		    data = data[4:]
		    print("raw data is: ", data)
		    command = json.loads(data)
		    if command["Type"] == "GetStatus":
		    	print("Got command GetStatus()")
		    	s.sendall(GetStatus())
		    else:
		    	print("Unknown command")

	#print('Received', data)
