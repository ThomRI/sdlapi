# -*- coding: utf-8 -*-
"""
Created on Tue Feb  5 19:30:36 2019

@author: Thomas
"""

import socket
import struct
import simulate

# Commands
SIMULATE = -1
READY = -2

HOST = '127.0.0.1'
PORT = 65423

def receive_cmd(socket):
    cmd = struct.unpack('i', socket.recv(4))[0]
    len_params = struct.unpack('i', socket.recv(4))[0]
    
    params = []
    for i in range(len_params):
        params.append(struct.unpack('i', socket.recv(4))[0])
        
    return (cmd, params)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as socket:
    socket.connect((HOST, PORT))
    socket.sendall(struct.pack('i', READY))
    
    (cmd, params) = receive_cmd(socket)
    if cmd == SIMULATE:
        print("Starting", params[0], "simulations")
        (average_x, average_y, average_dist, dev_percent_x, dev_percent_y, var_x, var_y) = simulate.simulate(params[0])
        print(average_x, average_y, average_dist, dev_percent_x, dev_percent_y, var_x, var_y)
    
    socket.close()