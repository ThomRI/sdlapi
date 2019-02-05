# -*- coding: utf-8 -*-
"""
Created on Tue Feb  5 19:30:36 2019

@author: Thomas
"""

import socket
import struct
import simulate
import sys

# Defines
TRAME_VALUE_SIZE = 4

# Commands
SIMULATE = -1
READY = -2
DONE = -3

EOF = -10

HOST = '127.0.0.1'
PORT = 65423
MAX_SIMULATIONS_ALLOWED = 5

def receive_cmd(socket):
    cmd = struct.unpack('i', socket.recv(TRAME_VALUE_SIZE))[0]
    len_params = struct.unpack('i', socket.recv(TRAME_VALUE_SIZE))[0]
    
    params = []
    for i in range(len_params):
        params.append(struct.unpack('f', socket.recv(TRAME_VALUE_SIZE))[0])
    
    return (cmd, params)

def send(socket, cmd, parameters = []):
    socket.send(struct.pack('i', cmd))
    socket.send(struct.pack('i', len(parameters)))
    
    for p in parameters:
        socket.send(struct.pack('f', float(p)))
    
    socket.send(struct.pack('i', EOF))
    

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as socket:
    socket.connect((HOST, PORT))
    send(socket, READY, [MAX_SIMULATIONS_ALLOWED])
    
    (cmd, params) = receive_cmd(socket)
    if cmd == SIMULATE:
        N = int(params[0])
        print("Starting", N, "simulations")
        (average_x, average_y, average_dist, dev_percent_x, dev_percent_y, var_x, var_y) = simulate.simulate(N)
        print("Results (sent to the server) :", average_x, average_y, average_dist, dev_percent_x, dev_percent_y, var_x, var_y)
        send(socket, DONE, [average_x, average_y, average_dist, dev_percent_x, dev_percent_y, var_x, var_y])
    
    socket.close()