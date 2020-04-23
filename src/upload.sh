#!/bin/bash

# $1 => folder & filename

filenameext=$(basename -- "$1")
directory=$(dirname "$1")
filename="${filenameext%.*}"

cd $directory
~/git/arduino-cli/bin/arduino-cli compile --fqbn arduino:samd:mkrwifi1010 $filenameext

mv $filenameext.arduino.samd.mkrwifi1010.bin $filename.arduino.samd.mkrwifi1010.bin
mv $filenameext.arduino.samd.mkrwifi1010.elf $filename.arduino.samd.mkrwifi1010.elf
cd ..
~/git/arduino-cli/bin/arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:samd:mkrwifi1010 $filename
