#set architecture riscv:rv32
import socket
import signal #for hanlding an interrupt to end gdb server
import json
import function_api as api
COUNT = 16
NUMBER_OF_BYTES = COUNT
brk_pnt_reply = "+SO5 "
PC_INCREMENTOR = 4

def reverse_bytes(s):
    return "".join([s[x:x+2] for x in range(0,len(s),2)][::-1])
def reverse_bytes_memory(s):
    return "".join([s[x:x+1] for x in range(0,len(s),1)][::-1])
def extract_data(register_collection, register_number):
    temp_string = ""
    for i in range(4):
        temp_string += f"{register_collection[(register_number*4)+i]:02x}" #took inspiration from the register read
    register_value = reverse_bytes(temp_string)
    return int(register_value,16) #convert from hexadecimal back to decimal
def test():
    with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:
        test_server = GDB_SERVER(s)
        while True:
            test_server.parser()
class GDB_SERVER(object):
    def __init__(self, sock):
        self.sock = sock
#        port = input("GDB Socket Port Target:")
#        port = int(port)
        port = 11111
        # Unsure if this is correct, but trying it out!
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.settimeout(150)
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
   
   
   
    def register_read(self,index):
        print(index) 
        #PROVES PROOF OF CONCEPT FOR THE FUNCTION API WORKING
        #bank = "GRF_B or GRF_A"
        #if index <= int(15):
           # bank = "GRF_A"
        read_value = ""
        for i  in range(33):
            #read_value += str((api.read_register() ).to_bytes(4,'little'))#api function 
            #read_value += str((api.read_register()).hex())#api function --> uncomment when floats get integrated
            read_value += reverse_bytes(f"{api.read_register(i):08x}") #api function 
        #print("Register {index} corresponding to {bank} is being read from is {read_value}") #tells you the bank being read from    
        return read_value #api function 
    
    
    
    
    def read_memory(self,data):
        print(f"Memory Read Occurs at address {data['address']} with length {data['length']}")
        builder = ""
        for i in range(data['length']):
            builder += f"{api.read_byte(data['address']+i):02x}"
        builder1 = reverse_bytes_memory(builder)
        return builder1
    
    
    
    def write_memory(self,data):
        status = api.write_byte(data['address'],data['value']) #if we want to write a byte to memory
        print(f"Memory Write occured {status}")
        reply = "OK" #states that things were successful in writing to memory
        return reply
    




    def register_write(self,data):
        reply = ""
        for i in range(33):
            register_number = (extract_data(data['data'],i))
            print(f"Register {i} contains {register_number}")
            reply = api.write_register(i,register_number)
        return reply
    
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
    
    def set_breakpoint(self, program_location):
        print("Preparing to set a breakpoint at {program_location}")
        address_set = api.set_breakpoint(program_location)
        if address_set == "Success":
            print("Breakpoint successfully set at {address_set}")
        return address_set

    

    def delete_breakpoint(self,breakpoint_location):
        print("Deleting Breakpoint at {breakpoint_location}")
        deleted = api.delete_breakpoint(breakpoint_location)
        print("Breakpoint deleted {deleted}")
    
    def intermediate_step(self,command,data):
        print(f"<-:{command}")
        status = "ok"
        if command == "+":
            return
        if command == "?":
            print("Pseudo Breakpoint Setting\n")
            reply = "+" + "SO5 " #specifies that a breakpoint is getting handled
        elif command == "g" or command == "G": #register access
            if command == "g":
                print("Register Reads Initiated \n")
                temp_reply = self.register_read(data)
                reply = str(temp_reply)
            else:
                print("Writing to Registers")
                temp_reply = self.register_write(data)
                reply = temp_reply
        elif command == "c": #continue command
            print("Continue Command Initiated\n")
            reply = self.handle_step(command)
        elif command == "s": #step command
            print("STEP Command initiated\n")
            reply = self.handle_continue(command)
        elif command == "m" or command == "M": #memory access
            print("Memory access initiated") 
            if command == 'm':
                reply = self.read_memory(data)
            else:
                reply = self.write_memory(data)
        elif command == 'Z' or command == "Z":
            breakpoint_location = data['breakpoint']
            if command == "Z":
                reply = self.set_breakpoint(breakpoint)
            else:
                reply = self.delete_breakpoint(breakpoint_location)
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
        #temp_json_data = json.dumps(temp_data) #if it fails, take this line and the next line out
        json_data = json.loads(temp_data)
        print(f"JSON Dumps: {json_data}")
        command =  json_data.get("type")
        data = json_data.get("data")
        print(f"Data being sent to intermediate step: {command}")
        reply = json.dumps(self.intermediate_step(command,data))
        self.conn.send(reply.encode())


#RUNNING THE TESTING CODE:
test()
