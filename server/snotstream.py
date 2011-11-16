import sys
import zmq

from fifo import ring_buff
from dispatch import stream_in
from jsonserver import jsonserver
        
if __name__ == '__main__':
    if len(sys.argv) > 1:
        address = sys.argv[1]
    else:
        address = 'tcp://*:5024'

    fifos = {}
    fifos['nhit'] = ring_buff(100)
    fifos['crateevent'] = ring_buff(100)

    # set up zeromq socket
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, '')
    socket.bind(address)

    stream = stream_in(socket,fifos)
    stream.start()

    httpd = jsonserver('', 8051,fifos)
    httpd.serve_forever()

    while True:
        try:
            if stream.isAlive():
                stream.join(1)
        except KeyboardInterrupt:
            print "Ctrl-c received! Sending kill to threads..."
            stream.kill_received = True
            break
