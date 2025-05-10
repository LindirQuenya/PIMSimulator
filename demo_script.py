from test import *            # Load program, setup memory with dummy values.
read_grf(False)               # Read GRF_A
read_grf(True)                # Read GRF_B
read_memgrf(1, 0)             # Read row 1, bank 0 (even)
read_memgrf(1, 1)             # Read row 1, bank 1 (odd)
pimsim.read_register(32)      # Read PC (index into program), which starts out at 0
pimsim.single_step()          # First instruction: GRF_A += EVEN_BANK
pimsim.read_register(32)      # Read PC
read_grf(False)               # Read GRF_A
pimsim.set_breakpoint(2)      # Next instruction: GRF_B += ODD_BANK
pimsim.execute()              # Continue execution
pimsim.read_register(32)      # Read PC
read_grf(True)                # Read GRF_B
pimsim.execute()              # Execute last instruction, GRF_A *= EVEN_BANK
pimsim.read_register(32)      # Read PC
read_grf(False)               # Read GRF_A
pimsim.write_register(32, 1)  # Set PC = 1
pimsim.read_register(32)      # Read PC
pimsim.single_step()          # Next instruction: GRF_B += ODD_BANK
pimsim.read_register(32)      # Read PC
read_grf(True)                # Read GRF_B
