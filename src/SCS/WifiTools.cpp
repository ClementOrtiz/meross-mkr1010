/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- WifiTools.h
		- WifiTools.cpp
	Dependencies :
		- Arduino.h
		- WiFi.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This Library is an encapsulation of technical and graphical functions.
	It help for wifi connection and check if connection still alive.
	Also help to find host by name.
	3 functions are dedicated to print information about Wifi and Lan
	
\**************************************************************************/

#include "WiFi.h"
#include "WifiTools.h"




void WifiTools::setDebug( bool isDebug ){
  _isDebug = isDebug;
}


void WifiTools::checkConnection(){
  if( _counterConnect > 1000 ){
    // if not connected, relaunch
    if( WiFi.status() != WL_CONNECTED ){
      if( _isDebug ){
        Serial.println("*** Warning ***\nWiFi Connection lost ... Trying to reconnect");
      }

      connectToWifi( _ssid.c_str(), _pass.c_str() );
    }
    _counterConnect = 0;
  }
  _counterConnect++;
}


void WifiTools::connectToWifi( const char ssid[] , const char pass[] ){
  // store ids
  _ssid = String( ssid );
  _pass = String( pass );

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE){
    if( _isDebug ){
      Serial.println("Communication with WiFi module failed!");
    }
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION){
    if( _isDebug ){
      Serial.println("Please upgrade the firmware");
    }
    // don't continue
    while (true);
  }

  // attempt to connect to Wifi network:
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED){
    if( _isDebug ){
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  if( _isDebug ){
    Serial.print("You're connected to the network");
  }
  printCurrentNet();
  printWifiData();
}


bool WifiTools::hostByName( char hostname[], IPAddress &plugIP ){
  bool founded = false;

  if( _isDebug ){
    Serial.println( "Launching research of plug IP" );
  }

  WiFi.hostByName(hostname, plugIP);

  if( plugIP[0] != 0 ){
    founded = true;
    if( _isDebug ){
      Serial.print("Plug found at ");
      Serial.println(plugIP);
    }
  }else if( _isDebug ){
      Serial.println("Plug not found ");
    }
  return founded;
}

void WifiTools::printWifiData(){
  if( !_isDebug )
    return;
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print( subnet );

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(" IP Address: ");
  Serial.print(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print(" MAC address: ");
  printMacAddress(mac);
}

void WifiTools::printCurrentNet(){
  if( !_isDebug )
    return;

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void WifiTools::printMacAddress( byte mac[] ){
  if( !_isDebug )
    return;

  for (int i = 5; i >= 0; i--){
    if (mac[i] < 16){
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);

    if (i > 0){
      Serial.print(":");
    }
  }
  Serial.println();
}
