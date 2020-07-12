There needs to be a passphrase for the key

The CN / FQDN needs to be filled out to the fully qualified domain name that will be used to access this CA Cert. (ie localhost for local or the name of the server otherwise or the ipaddress).

This same FQDN needs to be used in the request for the certificate to work.

To Generate the Certificate:
openssl req -x509 -newkey rsa:2048 -keyout ca_key.pem -out ca_cert.pem -days 365

To Start the https server to host the file:
#openssl s_server -WWW -key ca_key.pem -cert ca_cert.pem -port 8070
use the server.py program in this directory to start the server, you must supply a pass phrase.  Working dir is set to /home/pi/iot-binaries/ where the binaries should be

Test the connection with Curl (on localhost):
curl --cacert /home/dustin/workspace/esp32/iot_lightstrip/iot_eclipse_project/external/certificate_ota/ca_cert.pem https://localhost:8070/ca_cert.pem

