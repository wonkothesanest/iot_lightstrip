#!/bin/bash

BASEDIR=$(dirname "$0")
echo "$BASEDIR"

scp $BASEDIR/../../build/*.bin pi@raspberrypi.local:/home/pi/iot-binaries/
echo ""
echo ""
echo "Coppied Binaries... now run run_ota_server.sh"
echo ""
echo ""
echo ""
ssh pi@raspberrypi.local
