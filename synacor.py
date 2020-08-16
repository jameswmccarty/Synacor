#!/usr/bin/python

import pickle
import dump_code

stdin_buff = ''
ip = 0

"""eight registers"""
"""numbers 32768..32775 instead mean registers 0..7"""
regs = {
	32768 : 0,
	32769 : 0,
	32770 : 0,
	32771 : 0,
	32772 : 0,
	32773 : 0,
	32774 : 0,
	32775 : 0}

def is_reg(a):
	if a >= 32768 and a <= 32775:
		return True
	return False

"""an unbounded stack which holds individual 16-bit values"""
"""empty stack = error"""
stack = []

def push(value):
	global stack
	stack.append(value & 0xFFFF)

def pop():
	global stack
	if len(stack) == 0:
		return None
	else:
		return stack.pop()

"""memory with 15-bit address space storing 16-bit values"""
"""address 0 is the first 16-bit value, address 1 is the second 16-bit value, etc"""
mem = [ 0 for x in range(32768) ]

"""
halt: 0
  stop execution and terminate the program
"""
def halt():
	exit()

"""
set: 1 a b
  set register <a> to the value of <b>
"""
def opset(a, b):
	if not is_reg(a):
		print("Invalid register value in Set.")
		exit()
	val = regs[b] if is_reg(b) else b
	regs[a] = val

"""
push: 2 a
  push <a> onto the stack
"""
def oppush(a):
	if is_reg(a):
		push(regs[a])
	else:
		push(a)

"""
pop: 3 a
  remove the top element from the stack and write it into <a>; empty stack = error
"""
def oppop(a):
	val = pop()
	if val == None:
		print("Stack pop failure.  Empty stack.")
		exit()
	if is_reg(a): 
		regs[a] = val
	else:
		mem[a] = val

"""
eq: 4 a b c
  set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise
"""
def eq(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = 1 if val_c == val_b else 0
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
gt: 5 a b c
  set <a> to 1 if <b> is greater than <c>; set it to 0 otherwise
"""
def gt(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = 1 if val_c < val_b else 0
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
jmp: 6 a
  jump to <a>
"""
def jmp(a):
	global ip
	if is_reg(a): 
		ip = regs[a] - 2
	else:
		ip = a - 2

"""
jt: 7 a b
  if <a> is nonzero, jump to <b>
"""
def jt(a, b):
	global ip
	jumping = False
	if is_reg(a) and regs[a] != 0:
		jumping = True
	elif not is_reg(a) and a != 0:
		jumping = True
	if jumping:
		if is_reg(b):
			ip = regs[b] - 3
		else:
			ip = b - 3

"""
jf: 8 a b
  if <a> is zero, jump to <b>
"""
def jf(a, b):
	global ip
	jumping = False
	if is_reg(a) and regs[a] == 0:
		jumping = True
	elif not is_reg(a) and a == 0:
		jumping = True
	if jumping:
		if is_reg(b):
			ip = regs[b] - 3
		else:
			ip = b - 3

"""
add: 9 a b c
  assign into <a> the sum of <b> and <c> (modulo 32768)
"""
def add(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = (val_b + val_c) % 32768
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
mult: 10 a b c
  store into <a> the product of <b> and <c> (modulo 32768)
"""
def mult(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = (val_c * val_b) % 32768
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
mod: 11 a b c
  store into <a> the remainder of <b> divided by <c>
"""
def mod(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = (val_b % val_c) % 32768
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
and: 12 a b c
  stores into <a> the bitwise and of <b> and <c>
"""
def opand(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = val_c & val_b & 0xFFFF
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
or: 13 a b c
  stores into <a> the bitwise or of <b> and <c>
"""
def opor(a, b, c):
	val_b = regs[b] if is_reg(b) else b
	val_c = regs[c] if is_reg(c) else c
	result = val_c | val_b
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
not: 14 a b
  stores 15-bit bitwise inverse of <b> in <a>
"""
def opnot(a, b):
	val_b = regs[b] if is_reg(b) else b
	result = ~val_b
	result &= 0x7FFF
	if is_reg(a):
		regs[a] = result
	else:
		mem[a] = result
	return

"""
rmem: 15 a b
  read memory at address <b> and write it to <a>
"""
def rmem(a, b):
	val = mem[regs[b]] if is_reg(b) else mem[b]
	if not is_reg(a):
		print("Not a valid register")
	regs[a] = val
	return

"""
wmem: 16 a b
  write the value from <b> into memory at address <a>
"""
def wmem(a, b):
	val = regs[b] if is_reg(b) else b
	addr = regs[a] if is_reg(a) else a
	mem[addr] = val
	return

"""
call: 17 a
  write the address of the next instruction to the stack and jump to <a>
"""
def call(a):
	global ip
	push(ip+2)
	if is_reg(a): 
		ip = regs[a] - 2
	else:
		ip = a - 2

"""
ret: 18
  remove the top element from the stack and jump to it; empty stack = halt
"""
def ret():
	global ip
	retval = pop()
	if retval == None:
		halt()
	if is_reg(retval): 
		ip = regs[retval] - 1
	else:
		ip = retval - 1

"""defined to help make the game easier"""
def save():
	global ip
	global mem
	global regs
	global stdin_buff
	state = (ip, mem, regs, stdin_buff)
	with open('game.save', 'wb') as f:
		pickle.dump(state, f)

def load():
	global ip
	global mem
	global regs
	global stdin_buff
	with open('game.save', 'rb') as f:
		state = pickle.load(f)
	ip, mem, regs, stdin_buff = state

"""
out: 19 a
  write the character represented by ascii code <a> to the terminal
"""
def out(a):
	val = regs[a] if is_reg(a) else a
	print(chr(val), end='')

"""
in: 20 a
  read a character from the terminal and write its ascii code to <a>; it can be assumed that once input starts, it will continue until a newline is encountered; this means that you can safely read whole lines from the keyboard and trust that they will be fully read
"""
def opin(a):
	global stdin_buff
	while len(stdin_buff) == 0:
		stdin_buff = input(">")
		if stdin_buff == "save":
			save()
		elif stdin_buff == "load":
			load()
		elif stdin_buff == "regs":
			print(regs)
		elif stdin_buff == "dump":
			dump_code.dump_mem(mem)
		else:
			stdin_buff += '\n'
	if is_reg(a):
		regs[a] = ord(stdin_buff[0])
	else:
		print("Not a valid register")
	stdin_buff = stdin_buff[1:]


"""
noop: 21
  no operation
"""
def noop():
	return


if __name__ == "__main__":

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

	# (function, number of arguments)
	ops = [	(halt,0),
			(opset,2),
			(oppush,1),
			(oppop,1),
			(eq,3),
			(gt,3),
			(jmp,1),
			(jt,2),
			(jf,2),
			(add,3),
			(mult,3),
			(mod,3),
			(opand,3),
			(opor,3),
			(opnot,2),
			(rmem,2),
			(wmem,2),
			(call,1),
			(ret,0),
			(out,1),
			(opin,1),
			(noop,0)]

	while True:
		#print(ip, ' ', end='')
		#print(mem[ip])
		func, numv = ops[mem[ip]]
		if numv == 3:
			func(mem[ip+1],mem[ip+2],mem[ip+3])
		elif numv == 2:
			func(mem[ip+1],mem[ip+2])
		elif numv == 1:
			func(mem[ip+1])
		else:
			func()
		ip += (numv + 1)
		ip %= 32768
