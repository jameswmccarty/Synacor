"""eight registers"""
"""numbers 32768..32775 instead mean registers 0..7"""
regs = {
	32768 : 'reg0',
	32769 : 'reg1',
	32770 : 'reg2',
	32771 : 'reg3',
	32772 : 'reg4',
	32773 : 'reg5',
	32774 : 'reg6',
	32775 : 'reg7'}

# (function, number of arguments)
ops = [	("HALT",0),
		("SET",2),
		("PUSH",1),
		("POP",1),
		("EQ",3),
		("GT",3),
		("JMP",1),
		("JT",2),
		("JF",2),
		("ADD",3),
		("MULT",3),
		("MOD",3),
		("AND",3),
		("OR",3),
		("NOT",2),
		("RMEM",2),
		("WMEM",2),
		("CALL",1),
		("RET",0),
		("OUT",1),
		("IN",1),
		("NOOP",0)]

def dump_mem(mem):
	with open('snapshot.txt', 'w') as outfile:
		read_idx = 0
		while read_idx < len(mem):
			outfile.write("0x"+hex(read_idx)[2:].rjust(4,'0')+' ')
			if mem[read_idx] < len(ops):
				name, size = ops[mem[read_idx]]
				args = []
				while(size > 0):
					read_idx += 1
					value = mem[read_idx]
					if value in regs:
						value = regs[value]
					elif name == "OUT":
						value = chr(value)
						if value == '\n':
							value = "\\n"
					elif name in ["JMP", "JT", "CALL", "JF"]:
						value = hex(value)
					else:
						value = str(value)
					args.append(value)
					size -= 1
				line = [name] + args
				outfile.write(' '.join(line)+'\n')
			else:
				outfile.write(str(mem[read_idx])+'\n')
			read_idx += 1



if __name__ == "__main__":

	"""memory with 15-bit address space storing 16-bit values"""
	"""address 0 is the first 16-bit value, address 1 is the second 16-bit value, etc"""
	mem = [ 0 for x in range(32768) ]

	with open("challenge.bin", 'rb') as infile:
		raw_data = infile.read()

	"""programs are loaded into memory starting at address 0"""
	read_idx = 0
	"""each number is stored as a 16-bit little-endian pair (low byte, high byte)"""
	while(len(raw_data) > 0):
		mem[read_idx] = int.from_bytes(raw_data[0:2], byteorder='little')
		"""address 0 is the first 16-bit value, address 1 is the second 16-bit value, etc"""
		raw_data = raw_data[2:]
		read_idx += 1

	dump_mem(mem)


