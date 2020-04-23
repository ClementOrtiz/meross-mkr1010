#!/bin/bash


# $1 => folder & filename

filenameext=$(basename -- "$1")
directory=$(dirname "$1")
filename="${filenameext%.*}"

cd $directory
~/git/arduino-cli/bin/arduino-cli compile --fqbn arduino:samd:mkrwifi1010 $filenameext
