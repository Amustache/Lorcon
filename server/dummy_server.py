#!/usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler
from http import HTTPStatus
from urllib.parse import urlparse
import json
import random

# List is empty at first
procotones = {}

# The following is the complete structure in use
structure = {
  "uniqueid": {
    "state": -1,
    "last_update": {
      "year": 0,
      "month": 0,
      "day": 0,
      "hours": 0,
      "minutes": 0,
      "seconds": 0
    },
    "location": {
      "lat": 0,
      "lon": 0
    },
    "power": 0,
    "shields": 0,
    "capture_code": "abcdefgh",
    "name": "bla",
    "team": "bla",
    "debug": "bla"
  }
}

# Different values are computed on different sides
client_side = ["state", "last_update", "location", "debug"]
server_side = ["power", "shields", "capture_code", "name", "team"]

# These codes are hexwords
capture_codes = ['acce55ed', 'acce55e5', 'acc01ade', 'acc057ed', 'affec7ed', 'a11077ed', 'a11077ee', 'a5be5705', 'a55e55ed', 'a55e55e5', 'a77e57ed', 'ba11a575', 'ba5eba11', 'ba5e1e55', 'bea7ab1e', 'bedabb1e', 'bed57ead', 'be5077ed', 'b0a710ad', 'b01dface', 'b007ab1e', 'b0071ace', 'b0071e55', 'caba55e7', 'cab00d1e', 'ca11ab1e', 'ca5cabe1', 'ca5caded', 'ca5cade5', 'ca55e77e', 'ca7a1a5e', 'c105e7ed', 'c0a1e5ce', 'c011ec75', 'c01055a1', 'da7aba5e', 'deadbea7', 'deadfa11', 'dea110c8', 'decea5ed', 'decea5e5', 'defec7ed', 'de7ec7ed', 'de7e57ed', 'ea7ab1e5', 'effec7ed', 'e5ca1ade', 'face1e55', 'fade1e55', 'fa15e770', 'fa17b0a7', 'feeb1e57', 'f1abe118', 'f1a7b0a7', 'f1a7f007', 'f1a77e57', 'f1ee7e57', 'f10cc05e', 'f01db0a7', 'f005ba11', 'f007ba11', 'f007fa11', 'f0071e55', 'f055e77e', '1abe11ed', '1eaf1e55', '1eaf1e75', '0b501e7e', '0b57ac1e', '5a1eab1e', '5caff01d', '5ca1ab1e', '5eac0a57', '5eedca5e', '5e1ec7ed', '5e1ec7ee', '5e1f1e55', '5e77ab1e', '50f7ba11', '57acca70', '57edfa57', '7ac71e55', '7a7700ed', '7ee707a1', '7e1eca57', '7e117a1e', '7e55e118', '7e57ab1e', '707a11ed']

base_names = ['Albas', 'Bayaz', 'Colin', 'Drami', 'Ellen', 'Fredy', 'Ganon', 'Hanse', 'Ianne', 'Juann', 'Kvoth', 'Luiis', 'Morgo', 'Nystu', 'Olenk', 'Peter', 'Quiny', 'Randa', 'Steve', 'Tabea', 'Uriah', 'Vaira', 'Wacia', 'Xenia', 'Yancy', 'Zemen']

class _RequestHandler(BaseHTTPRequestHandler):
    # Borrowing from https://gist.github.com/nitaku/10d0662536f37a087e1b
    def _set_headers(self):
        self.send_response(HTTPStatus.OK.value)
        self.send_header('Content-type', 'application/json')
        # Allow requests from any origin, so CORS policies don't
        # prevent local development.
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()

    def do_GET(self):
        key = urlparse(self.path).path.split('/')[1]
        query = urlparse(self.path).query

        if key == "all":
            data = procotones
        else:
            if key not in procotones:
                return

            if query == "full":
                data = procotones[key]
            else:
                data = {element: procotones[key][element] for element in server_side}  # Unsound security

        self._set_headers()
        self.wfile.write(json.dumps({key: data}, separators=(',', ':')).encode('utf-8'))  # Remove space for lighter payload

    def do_POST(self):
        key = urlparse(self.path).path.split('/')[1]
        length = int(self.headers.get('content-length'))
        message = json.loads(self.rfile.read(length))
        for k, v in message.items():
            if key != k:  # Unsound security
                return
            if k not in procotones:
                procotones.update({k: v})
                procotones[k]["power"] = 0
                procotones[k]["shields"] = 0
                procotones[k]["name"] = random.choice(base_names)
                procotones[k]["team"] = ""
                procotones[k]["capture_code"] = random.choice(capture_codes)
                print("New protocone to be added: {} ({})!".format(procotones[k]["name"], k))
            else:
                for k2, v2 in v.items():
                    if k2 in client_side:  # Unsound security
                        procotones[k][k2] = v2
        self._set_headers()
        self.wfile.write(json.dumps({'success': True}).encode('utf-8'))

    def do_OPTIONS(self):
        # Send allow-origin header for preflight POST XHRs.
        self.send_response(HTTPStatus.NO_CONTENT.value)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST')
        self.send_header('Access-Control-Allow-Headers', 'content-type')
        self.end_headers()


def run_server():
    server_address = ('0.0.0.0', 8001)
    httpd = HTTPServer(server_address, _RequestHandler)
    print('serving at %s:%d' % server_address)
    httpd.serve_forever()


if __name__ == '__main__':
    run_server()
