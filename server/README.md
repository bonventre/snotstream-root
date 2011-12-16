snotstream
==========
(SNOplus realTime SimulaTion and data monitoRing intErnet cApable Module?)

snotstream is a monitoring tool for the SNO+ detector, which listens to live data as it is taken and turns it into useful plots, histograms, etc., for presentation to the detector operator.

This package is the 'middleware' that listens to the stream and serves plot data over HTTP in JSON format. Using a simple HTTP API users can query for this data directly, but the real target audience is the SNO+ monitoring web page, which plots the data using JavaScript plotting libraries.

Installation and Setup
----------------------
### Requirements ###
* A web server + proxy (nginx, apache2 + mod_rewrite)
* Python 2.7
* [avalanche](https://github.com/mastbaum/avalanche) dispatcher client module

### Getting the Code ###
Check out the latest version of the code from [github](https://github.com/mastbaum/snotstream):

    $ git clone git@github.com:mastbaum/snotstream.git

### Setting it Up ###
The snotstream package is easily set up with distutils. To install the snotstream binary and associated modules, just run:

    $ python setup.py install

In principle, this is all you need. In order to get the data to the client, however, the following steps are recommended.

Web Server Configuration
------------------------
Steps for configuring nginx are shown; Apache2 with mod_rewrite enabled is very similar.

### Add JSON server proxy ###
Add a URL rewrite to proxy `:8051/data` to `:80/data` in `/etc/nginx/conf.d/default` (or whatever). This pipes the JSON output of the snotstream server to a domain where the front-end can see it.

    # snotstream proxy
    location /data {
        proxy_pass         http://127.0.0.1:8051/data;
        proxy_redirect     off;
    }


### Add a location for the front-end ###
Serve static files for the snotstream "home page"

    # snotstream front-end
    location / {
        root               /path/to/snotstream/client;
    }

    $ service nginx restart

Running snotstream
------------------
`server/snotstream.py` is a runnable WSGI server. Run:

    $ snotstream

and it should be up and running.

Visit the web page (http://localhost/) and you should see... nothing! We need to listen to dispatched data from somewhere. By default, snotstream listens to `localhost:5024`. You can override this with, e.g.,

    $ snotstream tcp://realserver.site.com:1234

Actually getting dispatched data is beyond the scope of snotstream, but there are two ways:

1. Be running the detector + DAQ + event builder + dispatcher
2. Dispatch simulated events from RAT

See [avalanche](https://github.com/mastbaum/avalanche) for more details.

