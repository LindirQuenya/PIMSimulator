


def read_register(index):
    return index


def read_byte(address):
    return address & 0xFF

def write_byte(address,data):
    if address:
        return "Successful"
    else:
        return "Success"
def write_register(register,data):
    return "Success"
def set_breakpoint(pc):
    return "Success"
   
def delete_breakpoint(pc):
    return "Success"