import socket

HOST = '0.0.0.0'
PORT = 44444
BUFFER_SIZE = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((HOST, PORT))

while True:
    
    data = s.recvfrom(BUFFER_SIZE)
    if data:
        print(data[0].decode("utf-8"))
        
        state = int(input("Gir: "))
        if state == 1:
            s.sendto('0000'.encode(), data[1])
        elif state == 2:
            s.sendto('1000'.encode(), data[1])
        
s.close()


# https://realpython.com/python-sockets/