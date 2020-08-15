#!/usr/bin/python

from itertools import permutations

"""

red coin   = 2
corroded coin = 3
shiny coin = 5
concave    = 7
blue coin  = 9

_ + _ * _^2 + _^3 - _ = 399

(9, 2, 5, 7, 3)

"""

for p in permutations([2,3,5,7,9],5):
	if p[0] + p[1] * p[2]**2 + p[3]**3 - p[4] == 399:
		print(p)
		exit()
