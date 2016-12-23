#!/bin/bash

# iC880A-SPI Reset
# RPi pin22 == WiringPi pin6 == BCM gpio25

sudo echo "25"  > /sys/class/gpio/export
sudo echo "out" > /sys/class/gpio/gpio25/direction
sudo echo "1"   > /sys/class/gpio/gpio25/value
sudo sleep 5
sudo echo "0"   > /sys/class/gpio/gpio25/value
sudo sleep 1
sudo echo "0"   > /sys/class/gpio/gpio25/value

