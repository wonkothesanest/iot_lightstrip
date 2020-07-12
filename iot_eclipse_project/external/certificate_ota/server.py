import BaseHTTPServer, SimpleHTTPServer
import ssl
import argparse
import os
import time
import threading

keyfile = "/home/pi/iot_lightstrip/iot_eclipse_project/external/certificate_ota/ca_key.pem"
certfile = "/home/pi/iot_lightstrip/iot_eclipse_project/external/certificate_ota/ca_cert.pem"
binDir = "/home/pi/iot-binaries/"
timeout = time.time() + 10*60


class StoppableHTTPServer(BaseHTTPServer.HTTPServer):
    def run(self):
        try:
            self.serve_forever()
        except KeyboardInterrupt:
            self.shutdown()
            self.server_close()
            pass
        finally:
            # Clean-up server (close socket, etc.)
            self.server_close()


###### MAIN CODE #######
parser = argparse.ArgumentParser()
parser.add_argument('-p', '--port', dest='port', type= int,
    help= "Server Port", default= 8000)
args = parser.parse_args()

os.chdir(binDir)

httpd = StoppableHTTPServer(('', args.port),
        SimpleHTTPServer.SimpleHTTPRequestHandler)

httpd.socket = ssl.wrap_socket (httpd.socket,
        keyfile= keyfile,
        certfile= certfile, server_side=True)
thread = threading.Thread(None, httpd.serve_forever)

print("Serving on raspberrypi.local port ", args.port)
thread.start()

while(time.time() < timeout):
   try:
      time.sleep(1)
   except KeyboardInterrupt:
      break

httpd.shutdown()
thread.join()


print("Shutdown and Exiting")
#https://github.com/espressif/esp-idf/issues/2625
