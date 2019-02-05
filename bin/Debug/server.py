# -*- coding: utf-8 -*-
"""
Created on Tue Feb  5 19:30:25 2019

@author: Thomas
"""

import socket
import selectors
import types
import struct

# Commands
SIMULATE = -1
READY = -2

HOST = '127.0.0.1'
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
        recv_data = clientsocket.recv(4) # Max 1024 bytes

        if recv_data: # Data received
            event_read(clientsocket, recv_data, socketdata)
        else: # Socket closed
            print("Client disconnected :", clientsocket.getsockname())
            sel.unregister(clientsocket)
            clientsocket.close()
    
    if mask & selectors.EVENT_WRITE:
        if socketdata.outb:
            bytes_sent = clientsocket.send(socketdata.outb)
            socketdata.outb = socketdata.outb[bytes_sent:] # Next time we'll send the remaining datas

def event_read(socket, recv_data, socketdata):
    udata = struct.unpack('i', recv_data)[0] # Unpacked data to an integer
    print("Received", udata, "from", socket.getsockname())
    if udata == READY:
        trigger_simulation(socket, socketdata)
        
def send(socketdata, cmd, parameters):
    socketdata.outb += struct.pack('i', cmd)
    socketdata.outb += struct.pack('i', len(parameters)) # Number of parameters
    for p in parameters:
        socketdata.outb += struct.pack('i', p)
        
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
        