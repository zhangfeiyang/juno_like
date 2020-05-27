#!/usr/bin/python
import math


file = open('data','r')
lines = file.readlines()
x = []
y = []
z = []
rx = []
ry = []
rz = []

R = 19500
pi = 3.141592654
for line in lines:
	datas = line.split()
	theta = float(datas[1])
	phi = float(datas[2])
	X = R*math.sin(theta/180*pi)*math.cos(phi/180*pi)
	Y = R*math.sin(theta/180*pi)*math.sin(phi/180*pi)
	Z = R*math.cos(theta/180*pi)
	if abs(X) < 0.00001:
		X = 0
	if abs(Y) < 0.00001:
		Y = 0
	if abs(Z) < 0.00001:
		Z = 0
	
	x.append(str(X))
	y.append(str(Y))
	z.append(str(Z))
	ry.append(str(180 + theta))
	rz.append(str(phi))
file2 = open('PMT.h','w')

file2.write('double x[17739] = {\n')
for i in range(17739):
	if i<17738:
		file2.write(x[i]+',\n')
	else:
		file2.write(x[i]+'};\n')
	
file2.write('double y[17739] = {\n')
for i in range(17739):
	if i<17738:
		file2.write(y[i]+',\n')
	else:
		file2.write(y[i]+'};\n')
	
file2.write('double z[17739] = {\n')
for i in range(17739):
	if i<17738:
		file2.write(z[i]+',\n')
	else:
		file2.write(z[i]+'};\n')
	
#file2.write('double rx[17739] = {\n')
#for i in range(17739):
#	if i<17738:
#		file2.write(rx[i]+',\n')
#	else:
#		file2.write(rx[i]+'};\n')
	
file2.write('double ry[17739] = {\n')
for i in range(17739):
	if i<17738:
		file2.write(ry[i]+',\n')
	else:
		file2.write(ry[i]+'};\n')
	
file2.write('double rz[17739] = {\n')
for i in range(17739):
	if i<17738:
		file2.write(rz[i]+',\n')
	else:
		file2.write(rz[i]+'};\n')
