# -*- coding: utf-8 -*-
"""
Created on Sun Feb  3 18:11:59 2019

@author: Thomas
"""

import subprocess as sp
import matplotlib.pyplot as plt

N = 10 # Number of simulations

finalpos = []
for i in range(N):
    output = sp.Popen(["sdlapi.exe", "240", "240"], stdout=sp.PIPE).communicate()[0].decode().replace('\r', '').replace('\n', '').split(" ") #[x, y]
    finalpos.append([float(output[0]), float(output[1])])
    print("Simulation", i, "done")

X = [finalpos[i][0] for i in range(N)]
Y = [finalpos[i][1] for i in range(N)]

plt.plot(X, Y, "o")
plt.plot([240], [240], "or")
plt.show()
