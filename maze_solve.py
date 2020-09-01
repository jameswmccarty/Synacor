#!/usr/bin/python

from collections import deque

def mult(weight, factor):
	return weight * factor

def minus(weight, factor):
	return weight - factor

def plus(weight, factor):
	return weight + factor

maze = [[mult, 8, minus, 1],
		[4, mult, 11, mult],
		[plus,4,minus,18  ],
		[None,minus,9,mult]]

def bfs():

	q = deque()
	q.append( (3,0,22,None,'') )

	while len(q) > 0:

		x,y,w,op,path = q.popleft()

		# found a solution
		if (x,y) == (0,3) and w == 30:
			return path

		elif (x,y) != (0,3):
			for nx, ny, step in ((x-1,y, 'n'),(x+1,y,'s'),(x,y-1,'w'),(x,y+1,'e')):
				if nx >= 0 and nx < 4 and ny >= 0 and ny < 4:
					f = maze[nx][ny]
					if f != None:
						n_w = w
						if callable(op):
							n_w = op(w,f)
						if n_w > 0:
							q.append((nx,ny,n_w,f,path+step))



if __name__ == "__main__":
	
	print(bfs())
