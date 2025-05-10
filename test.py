import numpy as np
import pimsim

data1 = np.arange(16, dtype=np.half)

grfa = data1.tobytes()
even = (data1 + 16).tobytes()
grfb = (data1 + 12).tobytes()
odd = (data1 + 1).tobytes()

row1_addr = 0x40000
bank1_offset = 0x200

for i in range(16):
    pimsim.write_register(i, int.from_bytes(grfa[2*i:2*(i+1)], 'little'))
    pimsim.write_register(i+16, int.from_bytes(grfb[2*i:2*(i+1)], 'little'))

for i in range(32):
    pimsim.write_byte(row1_addr + i, even[i])
    pimsim.write_byte(row1_addr + bank1_offset + i, odd[i])

#pimsim.single_step()

def floatreg(i):
    return np.frombuffer(pimsim.read_register(i).to_bytes(2, 'little'), dtype=np.half)[0]

def read_grf(select_b):
    offset = 0
    if select_b:
        offset = 16
    arr = []
    for i in range(16):
        arr.append(float(floatreg(i+offset)))
    return arr

def read_memgrf(row, bank):
    offset = row1_addr * row + bank1_offset * bank
    arr = []
    for i in range(16):
        b1 = pimsim.read_byte(offset+2*i)
        b2 = pimsim.read_byte(offset+2*i+1)
        half = np.frombuffer(bytes([b1, b2]), dtype=np.half)[0]
        arr.append(float(half))
    return arr
