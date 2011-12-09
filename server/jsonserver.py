import json
from urlparse import parse_qs
from wsgiref.simple_server import WSGIServer, WSGIRequestHandler

class JSONServer(WSGIServer):
    def __init__(self, host, port, buffers):
        WSGIServer.__init__(self, (host, port), WSGIRequestHandler)
        self.buffers = buffers
        self.set_app(self.main)

    def main(self, env, start_response):
        '''parses the request url to decide what you want
       
        * to normally update an array of charts, data?(chartname)=(current key).
        * to update with a sublist of keys, data?buffer=(chartname)&startkey=\
            (startkey)&endkey=(endkey)
        '''
        path = env['PATH_INFO'].lstrip('/')
        query = parse_qs(env['QUERY_STRING'])

        if (path != 'data'):
            status = '404 NOT FOUND'
            headers = []
            start_response(status,headers)
            return ['404 Not found']

        status = '200 OK'
        headers = [('content-type', 'application/json')]
        start_response(status, headers)

        # handle '?buffer=name&startkey=n1&endkey=n2'-type request 
        buf = query.get('buffer',[''])[0]
        startkey = query.get('startkey',[''])[0]
        endkey = query.get('endkey',[''])[0]
        if ((buf in self.buffers) and startkey and endkey):
            b = self.buffers[buf]
            if endkey < 0:
                start_idx = max(len(b)-startkey, 0)
                start_idx = len(b)
            else:
                start_idx = ((startkey - b.update_seq) % (len(b)+1) if startkey > b.update_seq else 0)
                end_idx = ((endkey - b.update_seq) % (len(b)+1) if endkey > b.update_seq else 0)
            items = list(numpy.array(b)[start_idx:end_idx]) # deque doesn't have slicing
            results = [(start_idx, end_idx), items]
            return [json.dumps(results)]

        # handle '?buffer1=n1&buffer2=n2'-type request
        items = {}
        for key in query:
            if key in self.buffers:
                b = self.buffers[key]
                currentkey = query.get(key, [''])[0]
                start = currentkey if currentkey else 0
                items[key] = list(numpy.array(b[key])[start:])
        results = [b.update_seq, items]
        return [json.dumps(results)]

