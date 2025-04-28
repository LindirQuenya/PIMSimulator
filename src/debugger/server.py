import socket
import signal #for hanlding an interrupt to end gdb server


class GDB(object):
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        port = input("GDB Port:")
        port = int(port)
        self.sock.settimeout(15)
        self.sock.bind((socket.gethostbyaddr(), port)) #used socket documentation


    def close_socket(self):
        self.sock.close()
    def parser():
