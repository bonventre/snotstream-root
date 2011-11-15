import threading
import Queue
import sys
import zmq
import array
from rat import ROOT
import re
import os
import random
from cgi import parse_qs,escape
from wsgiref.simple_server import make_server, demo_app


class fifo():
    def __init__(self,size):
        self.size = size
        self.data = [None for i in xrange(self.size)]
        self.offset = -1*self.size

    def push(self,item):
        self.data.pop(0)
        self.data.append(item)
        self.offset = self.offset+1

    def pull(self,lastkey):
        if (lastkey >= (self.offset + self.size) or lastkey < 0):
            print "already up to date"
            return [self.offset+self.size,[]]
        elif (lastkey == 0):
            return [self.offset+self.size,[self.data[self.size-1]]]
        elif ((lastkey-self.offset+1) < 0):
            print "behind the end"
            return [self.offset+self.size,self.data[0:self.size+1]]
        else:
            print "good sublist ",lastkey,self.offset,self.offset+self.size,(lastkey-self.offset)+1
#            print self.data[(lastkey-self.offset)+1:self.size+1]
            return [self.offset+self.size,self.data[(lastkey-self.offset):self.size+1]]

class stream_in(threading.Thread):
    def __init__ (self):
        self.kill_received = False
        threading.Thread.__init__ (self)
    
    def run(self):
        while not self.kill_received:
            msg = socket.recv(copy=False)
            # buffer contains null characters, so wrap in array to pass to c
            b = array.array('c', msg.bytes)
            buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)
            rec = buf.ReadObject(ROOT.RAT.DS.PackedRec.Class())

            if rec:
                print 'Received PackedRec of type', rec.RecordType
                if (rec.RecordType == 1):
                    event = rec.Rec
                    event.__class__ = ROOT.RAT.DS.PackedEvent
                    print "nhit = ",event.NHits
                    nhit.push(int(event.NHits))
            else:
                print 'Error deserializing message data'
        print 'done!'

def main(env, start_response):
    
    path = env['PATH_INFO'].lstrip('/')
    query = parse_qs(env['QUERY_STRING'])

    if (path != "data"):
        status = '403 NOT AUTHORIZED'
        headers = []
        print "hi"
        start_response(status,headers)
        return ['Go away']

    status = '200 OK'
    headers = [('content-type','application/json')]
    start_response(status, headers)

    key = query.get('key',[''])[0]
    key = escape(key)
    if (key):
        if (int(key) or (int(key) == 0)):
            print "key is ",int(key)
            return [str(nhit.pull(int(key)))]
        else:
            return [str(nhit.pull(-1))]
    else:
        return [str(nhit.pull(-1))]



        
if __name__ == '__main__':
    if len(sys.argv) > 1:
        address = sys.argv[1]
    else:
        address = 'tcp://*:5024'

    nhit = fifo(100)

    # set up zeromq socket
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, '')
    socket.bind(address)


    stream = stream_in()
    stream.start()


    httpd = make_server('', 8051, main)
    httpd.serve_forever()

    while True:
        try:
            if stream.isAlive():
                stream.join(1)
        except KeyboardInterrupt:
            print "Ctrl-c received! Sending kill to threads..."
            stream.kill_received = True
            break

