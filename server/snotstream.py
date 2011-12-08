import sys
import collections
import avalanche

import jsonserver

class IndexedDeque(collections.deque):
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

    dispatch_client = avalanche.Client(address)

    httpd = jsonserver.JSONServer('', 8051, dispatch_client, fifos)
    httpd.serve_forever()

