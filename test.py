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
    pimsim.write_register(i, int.from_bytes(grfa[2*i:2*(i+1)]))
    pimsim.write_register(i+16, int.from_bytes(grfb[2*i:2*(i+1)]))

for i in range(32):
    pimsim.write_byte(row1_addr + i, even[i])
    pimsim.write_byte(row1_addr + bank1_offset + i, odd[i])

pimsim.single_step()

#def to_float()
