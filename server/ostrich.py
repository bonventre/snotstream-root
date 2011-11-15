#!/usr/bin/env python

import re
import os
import sys
import random
from cgi import parse_qs,escape
from wsgiref.simple_server import make_server, demo_app

def main(env, start_response):
    
    path = env['PATH_INFO'].lstrip('/')
    query = parse_qs(env['QUERY_STRING'])

    if (path != "data"):
        status = '403 NOT AUTHORIZED'
        headers = []
        print "hi"
        start_response(status,headers)
        return ['Go away']

    prev = query.get('prev',[''])[0]
    prev = escape(prev)


    status = '200 OK'
    headers = [('content-type','application/json')]
    start_response(status, headers)
    numberarray = []
    for i in range(1,513):
        if (i<200):
            numberarray.append([i,random.randint(0,int(i/10))])
        else:
            numberarray.append([i,random.randint(0,int((513-i)/10))])
    if (prev):
      for j in range(1,int(prev)):
          for i in range(0,512):
              if (i<200):
                  numberarray[i][1] += random.randint(0,int(i/10))
              else:
                  numberarray[i][1] += random.randint(0,int((513-i)/10))



    return [str(numberarray)]
#return ['{"foo":"' + prev + '"}'] 

# serve forever on localhost:8051
httpd = make_server('', 8051, main)
httpd.serve_forever()

