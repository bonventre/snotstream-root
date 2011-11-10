#!/usr/bin/env python

import re
import os
import sys
from cgi import parse_qs
from wsgiref.simple_server import make_server

import render
import resource
import handlers.server
import handlers.database
import handlers.document
import handlers.bulk_document
import handlers.view
import handlers.list_function
import handlers.show_function

couch = resource.Resource('http://adminuser:adminpass@localhost:5984')

def cookie_me(couch, env, username):
    return 200, {'Set-cookie': 'optoken=asdf1234'}, 'Cooookies!'

# map tokens (stored in cookies) to users
# this should be in a real database
#users = {'asdf1234': 'bob'}
users = {'asdf1234': 'mastbaum'}

def main(env, start_response):
    '''get json as if from a query directly to a couchdb server, but filtered
    based on in-document security settings.'''

    renderer = render.Renderer(start_response)

    # determine user from browser cookie matched with server-side token
    try:
        cookies = parse_qs(env['HTTP_COOKIE'])
        auth_token = cookies['optoken'][0]
        username = users[auth_token]
    except KeyError:
        username = ''

    path = env['PATH_INFO'].lstrip(os.sep)

    # url expression to request handler function map
    handler = {
        # testing: get a valid user cookie
        r'^_cookie\/?$': cookie_me,

        # server
        r'^\/?$': handlers.server.root,
        r'^_stats\/?$': handlers.server.stats,
        r'^_config\/(?P<settings>.+)\/?$': handlers.server.config,
        r'^_active_tasks\/?$': handlers.server.active_tasks,
        r'^_all_dbs\/?$': handlers.server.all_dbs,
        r'^_replicate\/?$': handlers.server.replicate,
        r'^_uuids\/?$': handlers.server.uuids,
        r'^_session\/?$': handlers.server.session,

        # database
        r'^(?P<dbname>\w+)\/?$': handlers.database.root,
        r'^(?P<dbname>\w+)\/_revs_limit\/?$': handlers.database.revs_limit,
        r'^(?P<dbname>\w+)\/_compact\/?$': handlers.database.compact,
        r'^(?P<dbname>\w+)\/_changes\/?$': handlers.database.changes,

        # document
        r'^(?P<dbname>\w+)\/(?P<docid>\w+)\/?$': handlers.document.root,
        r'^(?P<dbname>\w+)\/(?P<docid>\w+)\/(?P<attach>.+)$': handlers.document.attachment,
        r'^(?P<dbname>\w+)\/_design\/(?P<design>\w+)\/?$': handlers.document.design,
        r'^(?P<dbname>\w+)\/_design\/(?P<design>\w+)\/(?P<attach>.+)$': handlers.document.design_attachment,

        # bulk document
        r'^(?P<dbname>\w+)\/_all_docs\/?$': handlers.bulk_document.all_docs,
        r'^(?P<dbname>\w+)\/_bulk_docs\/?$': handlers.bulk_document.bulk_docs,

        # view
        r'^(?P<dbname>\w+)\/_design\/(?P<design>\w+)\/_view/(?P<view>\w+)\/?$': handlers.view.view,
        r'^(?P<dbname>\w+)\/_temp_view\/?$': handlers.view.temp_view,
        r'^(?P<dbname>\w+)\/_view_cleanup\/?$': handlers.view.view_cleanup,
        r'^(?P<dbname>\w+)\/_compact\/(?P<design>\w+)\/?$': handlers.view.compact,

        # show
        r'^(?P<dbname>\w+)\/_design\/(?P<design>\w+)\/_show/(?P<show>\w+)\/(?P<docid>\w+)\/?$': handlers.show_function.show_function,

        # list
        r'^(?P<dbname>\w+)\/_design\/(?P<design>\w+)\/_list/(?P<list>\w+)\/(?P<view>\w+)\/?$': handlers.list_function.list_function
    }

    try:
        h = handler[filter(lambda x: re.match(x, path), handler)[0]]
        return renderer.render_response(*h(couch, env, username))
    except (IndexError, TypeError):
        print 'unmatched path:', path
        return renderer.render_response(404, {}, '404 Not found :(')

# serve forever on localhost:8051
httpd = make_server('', 8051, main)
httpd.serve_forever()

