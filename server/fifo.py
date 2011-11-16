# Implements a ring buffer with some incrementing key offset. You can push
# elements into the top of the buffer, pull all elements since key X, or get
# some subset of elements from key X to key Y
# ring_buff.data[x] is the element with key ring_buff.offset+x

# index goes from 0 to size-1
# keys go from self.offset to self.offset+size-1

class ring_buff():
    # default value for all elements is zero, start with offset = -size so that
    # key of first value pushed on to the ring is zero
    def __init__(self,size):
        self.size = size
        self.data = [0 for i in xrange(self.size)]
        self.offset = -1*self.size


    def push(self,item):
        self.data.pop(0)
        self.data.append(item)
        self.offset = self.offset+1

    # returns the lastest key value and an array of all elements since lastkey.
    # if we dont have the full history since then, return whatever we have
    # if the lastkey was < 0 - i.e. haven't updated yet, only return one element
    # Thus can only see elements with keys >= 0
    # Returns the latest key in the array currently and some items
    def pull(self,lastkey):
        updatedkey = self.offset+self.size-1
        items = []
        if (lastkey < 0):
            if ((self.size + self.offset) > 0):
                items = [self.data[self.size-1]]
        else:
            if (lastkey < (self.offset + self.size)):
                if (lastkey >= self.offset):
                    items = self.data[lastkey-self.offset+1:self.size]
                else:
                    items = self.data[-1*self.offset:self.size]
        return [updatedkey,items]

    # grab some specific subset of the elements from startkey to endkey,
    # inclusive.
    # If not all of that range still exists, returns whatever subset of that range
    # of keys that is available. If endkey < 0 - i.e. haven't updated yet,
    # return the latest x elements where x is endkey-startkey+1. 
    # Returns the actual keys the list returned starts and ends at inclusively, and some
    # items. If there are no items, returns -1,-1
    def grab(self,startkey,endkey):
        if (((self.offset + self.size) == 0) or (endkey < startkey)):
            return [[-1,-1],[]]
        if (endkey < 0):
            length = endkey-startkey+1
            if (length < 0 or length > self.size):
                length = self.size
            if (length > (self.offset + self.size)):
                length = self.offset + self.size
            items = self.data[self.size-length:self.size]
            startkey = self.size-length+self.offset
            endkey = self.size+self.offset-1
        else:
            if (((endkey-startkey+1) > 0) and (startkey < (self.offset + self.size)) and
                (endkey >= self.offset)):
                if (startkey < 0):
                    startkey = 0
                if (startkey < self.offset):
                    startkey = self.offset
                if (endkey > (self.offset + self.size -1)):
                    endkey = self.offset + self.size - 1
                items = self.data[startkey-self.offset:endkey-self.offset+1]
            else:
                startkey = -1
                endkey = -1
                items = []
        return [[startkey,endkey],items]

