import threading
import zmq
import array
from rat import ROOT

class stream_in(threading.Thread):
    def __init__ (self,socket,fifos):
        self.kill_received = False
        self.socket = socket
        self.fifos = fifos
        threading.Thread.__init__ (self)
    
    def run(self):
        while not self.kill_received:
            msg = self.socket.recv(copy=False)
            # buffer contains null characters, so wrap in array to pass to c
            b = array.array('c', msg.bytes)
            buf = ROOT.TBufferFile(ROOT.TBuffer.kRead, len(b), b, False, 0)
            rec = buf.ReadObject(ROOT.RAT.DS.PackedRec.Class())

            if rec:
                print 'Received PackedRec of type', rec.RecordType
                if (rec.RecordType == 1):
                    event = rec.Rec
                    event.__class__ = ROOT.RAT.DS.PackedEvent

                    clock10part1 = event.MTCInfo[0]
                    clock10part2 = event.MTCInfo[1] & 0x1FFFF
                    clockCount10 = (clock10part2 << 32) + clock10part1

                    pevent = parsed_event(event)

                    for i in self.fifos:
                        if (i == 'nhit'):
                            self.fifos[i].push(int(event.NHits))
                        if (i == 'crateevent'):
                            self.fifos[i].push(pevent.crateevents)
                        if (i == 'cardevent'):
                            self.fifos[i].push(pevent.cardevents)
                        if (i == 'craterate'):
                            self.fifos[i].push([pevent.crateevents,clockCount10])
            else:
                print 'Error deserializing message data'
        print 'done!'

def getbits(arg,loc,n):
    shifted = arg >> loc
    mask = (1<<n)-1
    return shifted & mask

class hit():
    def __init__(self,bundle):
        self.crate = getbits(bundle.Word[0],21,5)
        self.card = getbits(bundle.Word[0],26,4)
        self.chan = getbits(bundle.Word[0],16,5)

class parsed_event():
    def __init__(self,event):
        self.hits = []
        self.crateevents = [0 for i in xrange(19)]
        self.cardevents = [0 for i in xrange(304)]
        for bundle in event.PMTBundles:
            self.hits.append(hit(bundle))
        for i in self.hits:
            self.crateevents[i.crate] += 1
            self.cardevents[i.crate*16+i.card] += 1

