import datetime as dt
import socket
import os
import json
import struct
from shutil import rmtree

HOST = '81.5.119.111'   # The server's hostname or IP address
PORT = 5006        # The port used by the server

def CreateRecording(task_dir, n_frames, one_shot=False):
	if one_shot:
		os.spawnl(mode=os.P_NOWAIT, path="xiSample_one_shot", "xiSample_one_shot", task_dir)
	else:
		os.spawnl(mode=os.P_NOWAIT, path="xiSample", "xiSample", task_dir)
	return dt.datetime.now()


def GetStatus():
	with open("status.json") as f:
		print("opening status.json..")
		data = json.load(f)
	data = json.dumps(data)
	L = len(data)
	L_bin = struct.pack('<l', L)
	print(L, L_bin+data.encode())
	return L_bin + data.encode()

def AddTask(task, max_fps=60):
	dir_name = "./data/id_" + str(task["TaskID"])
	if os.path.exists(dir_name) == False:
		os.mkdir(dir_name)
		with open(os.path.join(dir_name, "task.json"), 'w') as outfile:
		    json.dump(task, outfile)
		print("os.mkdir OK")
	else:
		raise ValueError(f"TaskID {task['TaskID']} already exists!")
	n_frames = task["Duration"] * task["Params"]["FPS"]
	n_frames = max(60, max_fps)
	return CreateRecording(dir_name, n_frames), task["Duration"]

def Transmit(task):
	pass

def Discard(task):
	dir_name = "./data/id_" + str(task["TaskID"])
	if os.path.exists(dir_name):
		rmtree(dir_name)


def FrameRequest(cam_id):
	pass

if __name__ == "__main__":
	start_time = None
	duration = None
	if not os.path.exists("./data"):
		os.mkdir("./data")
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
		    if start_time is not None:
		    	if (dt.datetime.now()-start_time).total_seconds() > duration + 20:
		    		start_time = None
		    		duration = None
		    if command["Type"] == "GetStatus":
		    	print("Got command GetStatus()")
		    	s.sendall(GetStatus())
		    elif command["Type"] == "AddTask":
		    	print("Got command AddTask()")
		    	if start_time is None: #check if camera is busy
		    		start_time, duration = AddTask(command["Task"])
		    elif command["Type"] == "TransmitVideo":
		    	print("Got command TransmitVideo()")
		    	TransmitVideo(command["ID"])
		    elif command["Type"] == "DiscardVideo":
		    	print("Got command DiscardVideo()")
		    	Discard(command["ID"])
		    elif command["Type"] == "FrameRequest":
		    	print("Got command FrameRequest()")
		    	FrameRequest(command["CameraID"])
		    else:
		    	print("Unknown command")
