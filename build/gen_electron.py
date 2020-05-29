#!/usr/bin/python
import random
import math

file = open('ele.txt','w')

pdgid = 11 
mass = 0.0005109989
energy = 0.005

for i in range(10000):

  momentum = ((energy + mass)**2 - mass**2)**0.5

  costheta = random.uniform(-1,1)
  theta = math.acos(costheta)
  phi = random.uniform(0,2*math.pi)

  pz = momentum*math.cos(theta)
  px = momentum*math.sin(theta)*math.cos(phi)
  py = momentum*math.sin(theta)*math.sin(phi)

  file.write('1\n')
  file.write('1\t'+str(pdgid)+' 0 0 '+str(px)+'\t'+str(py)+'\t'+str(pz)+'\t'+str(mass)+'\n')

file.close()

