import sys
import collections
import threading
import avalanche

import dispatch
import jsonserver

class IndexedDeque(collections.deque):
    '''wrapper for collections.deque that includes an update sequence number so
    we can figure out our real displacement.
    '''
    def __init__(self, *args, **kwargs):
        collections.deque.__init__(self, *args, **kwargs)
        self.update_seq = -1 * self.__len__()
    def __append__(self, x):
        collections.deque.append(self, x)
        self.update_seq += 1

if __name__ == '__main__':
    if len(sys.argv) > 1:
        address = sys.argv[1]
    else:
        address = 'tcp://*:5024'

    fifos = {}
    fifos['nhit'] = IndexedDeque([], 100)
    fifos['crateevent'] = IndexedDeque([], 100)
    fifos['cardevent'] = IndexedDeque([], 100)
    fifos['craterate'] = IndexedDeque([], 100)

    try:
        dispatch_client = dispatch.DispatchClient(address, fifos)
        print 'snotstream: starting dispatcher client'
        dispatch_client.start()

        httpd = jsonserver.JSONServer('', 8051, fifos)
        print 'snotstream: starting json server'
        httpd.serve_forever()

    except (KeyboardInterrupt, SystemExit):
        print 'snotstream: caught SIGINT, exiting'
        dispatch_client.request_exit = True
        dispatch_client.join()

