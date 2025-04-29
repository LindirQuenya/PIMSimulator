import socket
import signal #for hanlding an interrupt to end gdb server
import json
COUNT = 16
NUMBER_OF_BYTES = COUNT/8
brk_pnt_reply = "+SO5 "
PC_INCREMENTOR = 4
def test():
    with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:
        test_server = GDB_SERVER(s)
        while True:
            test_server.parser()
class GDB_SERVER(object):
    def __init__(self, sock):
        self.sock = sock
        port = input("GDB Socket Port Target:")
        port = int(port)
        # Unsure if this is correct, but trying it out!
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.settimeout(15)
        self.sock.bind(("127.0.0.1", port)) #used socket documentation
        self.stage = 0
        #self.data = " "
        self.sock.listen(2)
        self.conn, self.addr = self.sock.accept()
    def receiver(self):
        data = self.data
        try:
            data = self.sock.recv()
        except self.sock.timeout:
            pass
        else: 
            if data == " " or data == b'':
                raise RuntimeError("No data is being received, Something went wrong --> GDB server not sending constant stream of message")
        print("Data received:")
        print(data)  
        self.stage = 1     
    def continuous_send(self):
        data = self.sock.recv()
        if  "<-:" in data:
            buf = "->:+$#00"
            self.conn.send(buf.encode())

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
        self.temp.insert([self.hi,self.lo,self.bad_address,self.cause_of_bad_address,self.program_counter])
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
    def intermediate_step(self,command,data):
        print(f"<-:{command}")
        status = "ok"
        if command == "+":
            return
        if command == "?":
            print("Pseudo Breakpoint Setting\n")
            reply = "+" + "SO5 " #specifies that a breakpoint is getting handled
        elif command == "g" or command == "G": #register access
            print("Register Reads Initiated \n")
            temp_reply = self.register_read()
            reply = "+" + str(temp_reply) + ''            
        elif command == "c": #continue command
            print("Continue Command Initiated\n")
            reply = self.handle_step(command)
        elif command == "s": #step command
            print("STEP Command initiated\n")
            reply = self.handle_continue(command)
        elif command == "m" or command[1] == "M": #memory access
            print("Memory access initiated") 
            if command[1] == 'm':
                reply = self.read_memory()
            else:
                reply = self.write_memory()
        else:
            status = "NOT OKAY"
            print("Command is not supported... Please check for the next update")
            reply = "+" + "E.errtext" #returns error message
        print(f"->:{reply} & {status}")
        transmit_information = {"status":status , "data":reply}
        return transmit_information
    def parser(self):
        temp_data = self.conn.recv(4096).decode()
        print(f"Data received over socket: {temp_data}")
        temp_json_data = json.dumps(temp_data)
        json_data = json.loads(temp_json_data)
        print(f"JSON Dumps: {json_data}")
        command =  json_data.get("type")
        data = json_data.get("data")
        print(f"Data being sent to intermediate step: {command}")
        reply = self.intermediate_step(command,data)
        self.conn.send(reply.encode())


#RUNNING THE TESTING CODE:
test()
