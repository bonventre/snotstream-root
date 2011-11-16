from urlparse import parse_qs
from wsgiref.simple_server import WSGIServer, WSGIRequestHandler

class jsonserver(WSGIServer):
    def __init__(self,host,port,fifos):
        self.fifos = fifos
        WSGIServer.__init__(self,(host,port),WSGIRequestHandler)
        self.set_app(self.main)

    # parses the request url to decide what you want
    # to normally update an array of charts, data?(chartname)=(current key).
    # To update with a sublist of keys, data?history=(chartname)&startkey=(startkey)&endkey=(endkey)
    def main(self, env, start_response):
        path = env['PATH_INFO'].lstrip('/')
        query = parse_qs(env['QUERY_STRING'])

        if (path != "data"):
            status = '403 NOT AUTHORIZED'
            headers = []
            start_response(status,headers)
            return ['Go away']

        status = '200 OK'
        headers = [('content-type','application/json')]
        start_response(status, headers)

        history = query.get('history',[''])[0]
        startkey = query.get('startkey',[''])[0]
        endkey = query.get('endkey',[''])[0]
        if ((history in self.fifos) and startkey and endkey):
            results = self.fifos[history].grab(int(startkey),int(endkey))
            return [str(results)]

        results = {}
        for i in self.fifos:
            currentkey = query.get(i,[''])[0]
            if (currentkey):
                results[i] = self.fifos[i].pull(int(currentkey))
        return [str(results)]

