


def read_register(index):
    return index


def read_byte(address):
    return address & 0xFF

def write_byte(address):
    if address:
        return "Successful"
    else:
        return "Success"
