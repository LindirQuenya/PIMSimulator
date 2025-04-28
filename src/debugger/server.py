import socket
import signal #for hanlding an interrupt to end gdb server

class GDB_SERVER(object):
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        port = input("GDB Socket Port Target:")
        port = int(port)
        self.sock.setsockopt(level=socket.SOL_SOCKET, option_name=socket.SO_KEEPALIVE)
        self.sock.settimeout(15)
        self.sock.bind((socket.gethostbyaddr(), port)) #used socket documentation
        self.stage = 0
        #self.data = " "
    def receiver(self):
        data = self.data
        try:
            data = self.sock.recv()
        except self.sock.timeout:
            pass
        else: 
            if data == " " or data == b'':
                raise RuntimeError("No data is being received, Something went wrong --> GDB server not sending constant stream of message")
        print(F"Data received: {data}")  
        self.stage = 1  
    def continuous_send(self):
        data = self.sock.recv()
        if  "<-:" in data:
            buf = "->:+$#00"
            self.sock.send(buf.encode())

    def close_socket(self):
        self.sock.close()
    def parser(self):
        if self.stage != 1: #do not enter parser mode
            pass
        else:
            self.stage = 2
            pass
