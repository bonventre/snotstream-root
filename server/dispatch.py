import zmq
import threading
import avalanche
from rat import ROOT

class DispatchClient(threading.Thread, avalanche.Client):
    '''wrapper for avalanche.Client that writes received event data into some
    fifo buffers. runs as a threading.Thread thread so we can write to the
    buffers while serving.'''
    def __init__(self, address, buffers):
        self.request_exit = False
        self.buffers = buffers
        avalanche.Client.__init__(self, address)
        threading.Thread.__init__(self)
    def run(self):
        '''perform the threaded action, per threading.Thread'''
        print 'DispatchClient: listening to', self.address
        while True:
            while True:
                if self.request_exit:
                    print 'DispatchClient: exiting'
                    return
                else:
                    rec = self.recv_object(ROOT.RAT.DS.PackedRec.Class(), flags=zmq.NOBLOCK)
                    if rec:
                        break

            if rec:
                print 'DispatchClient: received PackedRec of type', rec.RecordType
                if (rec.RecordType == 1):
                    event = rec.Rec
                    event.__class__ = ROOT.RAT.DS.PackedEvent

                    pevent = ParsedEvent(event)

                    for buf in self.buffers:
                        if (buf == 'nhit'):
                            print event.NHits
                            self.buffers[buf].append(int(event.NHits))
                        elif (buf == 'crateevent'):
                            self.buffers[buf].append(pevent.crateevents)
                        elif (buf == 'cardevent'):
                            self.buffers[buf].append(pevent.cardevents)
                        elif (buf == 'craterate'):
                            self.buffers[buf].append([pevent.crateevents, pevent.clockCount10])
            else:
                print 'DispatchClient: error deserializing message data'


def get_bits(arg, loc, n):
    '''pull bits out a number'''
    shifted = arg >> loc
    mask = (1<<n) - 1
    return shifted & mask

class Hit():
    '''represents the crate/card/channel of a pmt hit'''
    def __init__(self, bundle):
        self.crate = get_bits(bundle.Word[0], 21, 5)
        self.card = get_bits(bundle.Word[0], 26, 4)
        self.chan = get_bits(bundle.Word[0], 16, 5)

class ParsedEvent():
    '''basic detector event data, parsed from 'packed' data into arrays'''
    def __init__(self, event):
        self.hits = []
        self.crateevents = [0 for i in xrange(19)]
        self.cardevents = [0 for i in xrange(304)]

        clock10part1 = event.MTCInfo[0]
        clock10part2 = event.MTCInfo[1] & 0x1FFFF
        self.clockCount10 = (clock10part2 << 32) + clock10part1

        for bundle in event.PMTBundles:
            hit = Hit(bundle)
            self.hits.append(hit)
            self.crateevents[hit.crate] += 1
            self.cardevents[16*hit.crate+hit.card] += 1

