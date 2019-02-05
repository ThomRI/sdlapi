# -*- coding: utf-8 -*-
"""
Created on Sun Feb  3 18:11:59 2019

@author: Thomas
"""

import subprocess as sp
#import matplotlib.pyplot as plt
import numpy as np

world = 480.0
start_x = 240.0
start_y = 240.0

avpos_x = []
avpos_y = []
dist = []

def simulate(N):
    finalpos = []
    for i in range(N):
        output = sp.Popen(["sdlapi.exe", str(np.random.rand() * 100000000), str(start_x), str(start_y)], stdout=sp.PIPE).communicate()[0].decode().replace('\r', '').replace('\n', '').split(" ") #[x, y]
        finalpos.append([float(output[0]), float(output[1])])
        print("\tSimulation", i, "done")
    
    X = [finalpos[i][0] for i in range(N)]
    Y = [finalpos[i][1] for i in range(N)]
    
    
    # Results
    average_x = np.average(X)
    average_y = np.average(Y)
    dev_x = average_x - start_x
    dev_y = average_y - start_y
    dev_percent_x = (dev_x / world) * 100
    dev_percent_y = (dev_y / world) * 100
    
    var_x = np.average([(X[i] - start_x)**2 for i in range(N)])
    var_y = np.average([(Y[i] - start_y)**2 for i in range(N)])
    
    '''
    plt.plot(X, Y, "o")
    plt.plot([start_x], [start_y], "or")
    plt.show()    

    print("Average position is (", average_x, ",", average_y, ")", sep='')
    print("Average deviation from starting point is (", dev_x, ",", dev_y, ")", sep='')
    print("Average deviation in % is (", abs(dev_percent_x), ",", abs(dev_percent_y), ")", sep='')
    print("Variance is (", var_x, ",", var_y, ")", sep='')
    '''
    
    # (average x, average y, average dist, deviation % x, deviation % y, var x, var y)
    return (average_x, average_y, np.sqrt((dev_x)**2 + (dev_y)**2), dev_percent_x, dev_percent_y, var_x, var_y)
