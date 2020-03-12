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


void WifiTools::printWifiData(){
  if( !_isDebug )
    return;
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print( subnet );

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
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
