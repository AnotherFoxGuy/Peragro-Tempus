#Copyright Jon Berg , turtlemeat.com

import string,cgi,time
from os import curdir, sep, path
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
#import pri
import os, sys

class MyHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        try:
            file = path.dirname(sys.argv[0]) + sep + "index.html"
            if self.path == "/distro-pack.log":
                file = curdir + sep + "distro-pack.log"
            f = open(file)
            self.send_response(200)
            self.send_header('Content-type',	'text/html')
            self.end_headers()
            text = f.read()
            self.wfile.write(text)
            f.close()
            return
                
        except IOError:
            self.send_error(404,'File Not Found: %s' % self.path)
     

    def do_POST(self):
        global rootnode
        try:
            return
            ctype, pdict = cgi.parse_header(self.headers.getheader('content-type'))
            if ctype == 'multipart/form-data':
                query=cgi.parse_multipart(self.rfile, pdict)
            self.send_response(301)
            
            self.end_headers()
            upfilecontent = query.get('upfile')
            print "filecontent", upfilecontent[0]
            self.wfile.write("<HTML>POST OK.<BR><BR>");
            self.wfile.write(upfilecontent[0]);
            
        except :
            pass

def main():
    global curdir
    if len(sys.argv) > 1:
        print "curdir is " + sys.argv[1]
        curdir = path.normpath(sys.argv[1])
    try:
        server = HTTPServer(('', 80), MyHandler)
        print 'started httpserver...'
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()

