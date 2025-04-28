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
        data = self.sock.recv(4096)
        if data[1] == "?":
            reply = "+" + "SO5 " #specifies that a breakpoint is getting handled
            self.sock.send(reply)
        elif data[1] == "g" or data[1] == "G": #register access
            
        elif data[1] == "c" or "vCont" in data: #continue command
        elif data[1] == "s": #step command
        elif data[1] == "m" or data[1] == "M": #memory access
            reply = "+" + 
        else:
            print("Command is not supported... Please check for the next update")
            reply = "+" + "E.errtext" #returns error message
            self.sock.send(reply)
            return 