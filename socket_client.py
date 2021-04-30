import sys
import socket

if len(sys.argv) != 2:
    exit()

PORT = 4000
ADDR = "127.0.0.1"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((ADDR, PORT))

f = sys.argv[1]
f += '\n'
s.sendall(f)
data = s.recv(1024)
print(data)
