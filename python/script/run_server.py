#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from venture.shortcuts import *

port = 8082
lite = False

for arg in sys.argv[1:]:
    if arg == "--lite":
        lite = True
    else:
        try:
            port = int(arg)
        except ValueError:
            print("Unknown argument " + arg)

server = make_ripl_rest_server(lite=lite)
server.run(host='0.0.0.0', port=port, debug=True, use_reloader=False)
