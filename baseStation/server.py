#!/usr/bin/env python 

""" 
A simple echo server 
""" 

import socket 

host = '' 
port = 8080 
backlog = 5 
size = 1024 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
print(socket.AF_INET)
s.bind((host,port)) 
s.listen(backlog) 
while 1: 
    client, address = s.accept() 
    data = client.recv(size) 
    print(data)
    if data: 
        client.send(data) 
    client.close()

"""
#!/usr/bin/env python 


A simple echo client 


import socket 

host = 'localhost' 
port = 50000 
size = 1024 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.connect((host,port)) 
s.send('Hello, world') 
data = s.recv(size) 
s.close() 
print 'Received:', data
"""