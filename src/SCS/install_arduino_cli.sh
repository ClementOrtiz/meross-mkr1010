#!/bin/bash

sudo -v

echo "## Installing Arduino CLI ##"
## Create folder
mkdir arduino_cli
cd arduino_cli
## Download files and execute install
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

echo ""
echo "## Installing SAMD's board (containing MKR 1010) ##"
# Install SAMD's board
cd bin/
chmod 755 arduino-cli
./arduino-cli core install arduino:samd

echo ""
echo "## Installing needed library's ##"
# Install lib WifiNINA
./arduino-cli lib install WiFiNINA
## Add here more library if needed

echo ""
echo "## Defining access to serial port ##"
# Access to serial port
sudo usermod -a -G dialout $USER
sudo snap connect arduino:raw-usb

echo ""
read -p "## System need to be restarted. Reboot now? (y/n) : " input
if [ "$input" = "y" ]
then
    sudo shutdown -r now
fi
