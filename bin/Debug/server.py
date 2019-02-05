# -*- coding: utf-8 -*-
"""
Created on Tue Feb  5 19:30:25 2019

@author: Thomas
"""

import socket
import selectors
import types
import struct

# Defines
TRAME_VALUE_SIZE = 4

# Commands
SIMULATE = -1
READY = -2
DONE = -3
EOF = -10

HOST = '' # Accept from any IP
PORT = 65423

def accept_connection(socket):
    clientsocket, addr = socket.accept()
    print("Accepted connection from", addr)
    clientsocket.setblocking(False)
    
    socketdata = types.SimpleNamespace(addr = addr, inb=b'', outb=b'') # Custom namespace for the datas of the client socket
    
    # Registering the client socket for read and write with our custom namespace datas
    sel.register(clientsocket, selectors.EVENT_READ | selectors.EVENT_WRITE, data=socketdata)

def event_wrapper(key, mask):
    clientsocket = key.fileobj
    socketdata = key.data
    
    # Read
    if mask & selectors.EVENT_READ:
        recv_data = clientsocket.recv(TRAME_VALUE_SIZE)

        if recv_data: # Data received
            if len(recv_data) == TRAME_VALUE_SIZE and struct.unpack('i', recv_data)[0] == EOF: # End of a full trame
                event_read(clientsocket, socketdata)
            else: # Else required not to add the EOF
                socketdata.inb += recv_data
        else: # Socket closed
            print("Client disconnected :", clientsocket.getsockname())
            sel.unregister(clientsocket)
            clientsocket.close()
    
    if mask & selectors.EVENT_WRITE:
        if socketdata.outb:
            bytes_sent = clientsocket.send(socketdata.outb)
            socketdata.outb = socketdata.outb[bytes_sent:] # Next time we'll send the remaining datas

def event_read(socket, socketdata):
    cmd = struct.unpack('i', socketdata.inb[:TRAME_VALUE_SIZE])[0]
    len_params = struct.unpack('i', socketdata.inb[TRAME_VALUE_SIZE:2 * TRAME_VALUE_SIZE])[0]
    socketdata.inb = socketdata.inb[2 * TRAME_VALUE_SIZE:]
    params = []
    for i in range(len_params):
        params.append(struct.unpack('f', socketdata.inb[TRAME_VALUE_SIZE*i:TRAME_VALUE_SIZE*(i+1)])[0])
    
    socketdata.inb = socketdata.inb[len_params*TRAME_VALUE_SIZE:] # We read that already into params
        
    if cmd == READY:
        # params[0] is the number of simulation max allowed
        trigger_simulation(socket, socketdata, params[0])
    elif cmd == DONE:
        average_x = params[0]
        average_y = params[1]
        average_dist = params[2]
        dev_percent_x = params[3]
        dev_percent_y = params[4]
        var_x = params[5]
        var_y = params[6]
        
        print("Received simulation results from", socket.getsockname(), ":")
        print("\t Average:", (average_x, average_y))
        print("\t Average dist:", average_dist)
        print("\t Deviation (%):", (dev_percent_x, dev_percent_y))
        print("\t Variance:", (var_x, var_y))
        
def send(socketdata, cmd, parameters):
    socketdata.outb += struct.pack('i', cmd)
    socketdata.outb += struct.pack('i', len(parameters)) # Number of parameters
    for p in parameters:
        socketdata.outb += struct.pack('f', p)
        
def trigger_simulation(socket, socketdata, N):
    send(socketdata, SIMULATE, [N])
    
    
sel = selectors.DefaultSelector() # Event handler
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serversocket:
    serversocket.bind((HOST, PORT))
    serversocket.listen()
    print("Now listening on", (HOST, PORT))
    serversocket.setblocking(False)
    
    sel.register(serversocket, selectors.EVENT_READ, data = None)
    
    while True: # Event loop
        events = sel.select(timeout = None)
        for key, mask in events:
            # mask is the mask for the toggled IO events, key is a tuple that contains the socket in key.fileobj
            if key.data is None: # New connection waiting for accept() from the listening socket (the server socket)
                accept_connection(key.fileobj) # The server socket is passed in key.fileobj
            else: # Data received 
                event_wrapper(key, mask)
    
    serversocket.close()
        