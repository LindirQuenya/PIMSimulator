import socket
import signal #for hanlding an interrupt to end gdb server
COUNT = 16
NUMBER_OF_BYTES = COUNT/8
brk_pnt_reply = "+SO5 "
PC_INCREMENTOR = 4
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
    def register_read(self):
        temp = []
        for i in range(COUNT):
            temp.insert(i)
    #code from Matsuo
        self.hi = 0
        self.lo = 0
        self.bad_address = 0
        self.cause_of_bad_address = 0
        self.program_counter = 0x10000
        self.temp.insert([hi,lo,bad_address,cause_of_bad_address,program_counter])
        temp.to_bytes(NUMBER_OF_BYTES, byteorder='little')
        print(F" registers printed out from 0 to {COUNT}: {temp}")
        return temp
    def read_memory(self):
        pass
    def write_memory(self):
        pass
    def update_pc(self,address):
        self.program_counter = address + PC_INCREMENTOR
    def handle_continue(self,temp_addr):
        address = int(temp_addr)
        self.update_pc(address)
        print(F"continue onwards --> increment the address value to now be: {self.program_counter}")
        return brk_pnt_reply
    def handle_step(self,temp_addr):
        address = int(temp_addr)
        self.update_pc(address)
        print(F"step over --> increment the address value to now be: {self.program_counter}")
        return brk_pnt_reply
    def parser(self):
        data = self.sock.recv(4096)
        if data[1] == "?":
            print("Pseudo Breakpoint Setting\n")
            reply = "+" + "SO5 " #specifies that a breakpoint is getting handled
            self.sock.send(reply)
        elif data[1] == "g" or data[1] == "G": #register access
            print("Register Reads Initiated \n")
            temp_reply = self.register_read()
            reply = "+" + str(temp_reply) + ''            
        elif data[1] == "c" or "vCont" in data: #continue command
            print("Continue Command Initiated\n")
            reply = self.handle_step(data)
            self.sock.send(reply)
        elif data[1] == "s": #step command
            print("STEP Command initiated\n")
            reply = self.handle_continue(data)
            self.sock.send(reply)
        elif data[1] == "m" or data[1] == "M": #memory access
            print("Memory access initiated") 
            if data[1] == 'm':
                reply = self.read_memory()
            else:
                reply = self.write_memory()
            self.sock.send(reply) 
        else:
            print("Command is not supported... Please check for the next update")
            reply = "+" + "E.errtext" #returns error message
            self.sock.send(reply)
            return 