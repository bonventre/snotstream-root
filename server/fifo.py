class RingBuffer(list):
    '''a ring buffer with some incrementing key offset, with an api similar to
    lists. the notable departure is slice, which returns only the available
    subset of what was requested, along with the actual key range returned, e.g.
    (15, 17), [3, 4]

    internal storage is a list, such that data[x] is the element with
    key = offset + x. the index goes from 0 to size-1, and  keys go from
    offset to offset+size-1

    default values are zero for all elements and offset=-size, so that key of
    the first value pushed on to the ring is zero.
    '''
    def __init__(self, length):
        self.data = [0 for i in range(length)]
        self.offset = -1 * len(self.data)

    def __len__(self):
        return len(self.data)

    def append(self, item):
        self.data.pop(0)
        self.data.append(item)
        self.offset += 1

    def __getitem__(self, i):
        return self.data[i]

    # grab some specific subset of the elements from startkey to endkey,
    # inclusive.
    # If not all of that range still exists, returns whatever subset of that range
    # of keys that is available. If endkey < 0 - i.e. haven't updated yet,
    # return the latest x elements where x is endkey-startkey+1. 
    # Returns the actual keys the list returned starts and ends at inclusively, and some
    # items. If there are no items, returns -1,-1
    def __getslice__(self, startkey, endkey):
        size = len(self.data)
        if (self.offset + size) == 0 or endkey < startkey:
            return (-1,-1), []
        if endkey < 0:
            length = endkey - startkey + 1
            if length < 0 or length > size:
                length = self.size
            if length > self.offset + size:
                length = self.offset + size
            items = self.data[size-length:size]
            startkey = size - length + self.offset
            endkey = size + self.offset - 1
        else:
            if (endkey - startkey + 1) > 0 and startkey < (self.offset + size) and endkey >= self.offset:
                startkey = max(startkey, self.offset, 0)
                endkey = min(endkey, self.offset+size-1)
                items = self.data[startkey-self.offset:endkey-self.offset+1]
            else:
                startkey = -1
                endkey = -1
                items = []
        return (startkey, endkey), items



