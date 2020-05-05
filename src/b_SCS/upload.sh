#!/bin/bash

./arduino_cli/bin/arduino-cli compile --fqbn arduino:samd:mkrwifi1010 SCS.ino

mv SCS.ino.arduino.samd.mkrwifi1010.bin SCS.arduino.samd.mkrwifi1010.bin
mv SCS.ino.arduino.samd.mkrwifi1010.elf SCS.arduino.samd.mkrwifi1010.elf
cd ..
./SCS/arduino_cli/bin/arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:samd:mkrwifi1010 SCS
