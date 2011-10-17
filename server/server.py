import socket
import json

host = ''
port = 5988

c = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
c.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
c.bind((host,port))
c.listen(1)
print "server is now listening"

while 1:
  csock, caddr = c.accept()
  print "accepted connection"
  cfile = csock.makefile('rw',0)
  line = cfile.readline().strip()
  cfile.write('http/1.1 200 OK\n')
  cfile.write('content-type: application/json\n\n')
  cfile.write('{"hello":"hi","how_r_u":{"noob":[1,2,"yes"]}}')
  cfile.close()
  csock.close()
