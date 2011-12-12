import json
from urlparse import parse_qs
from wsgiref.simple_server import WSGIServer, WSGIRequestHandler

import numpy

class JSONServer(WSGIServer):
    def __init__(self, host, port, buffers):
        WSGIServer.__init__(self, (host, port), WSGIRequestHandler)
        self.buffers = buffers
        self.set_app(self.main)

    def main(self, env, start_response):
        '''parses the request url to decide what you want
       
        * to normally update an array of charts, data?(chartname)=(current key).
        * to update with a sublist of keys, data?history=(chartname)&startkey=\
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

        # handle '?history=name&startkey=n1&endkey=n2'-type request 
        buf = query.get('history',[''])[0]
        startkey = int(query.get('startkey',[''])[0] or 0)
        endkey = int(query.get('endkey',[''])[0] or 0)
        if ((buf in self.buffers) and startkey and endkey):
            b = self.buffers[buf]
            keys, items = b[startkey:endkey]
            return [json.dumps([keys, items])]

        # handle '?buffer1=n1&buffer2=n2'-type request
        items = {}
        for key in query:
            if key in self.buffers:
                b = self.buffers[key]
                currentkey = int(query.get(key, [''])[0])
                if currentkey == -1:
                    start = len(b)-1
                else:
                    start = currentkey
                k, l = b[start:]
                items[key] = (b.offset+len(b), l)
        return [json.dumps(items)]

