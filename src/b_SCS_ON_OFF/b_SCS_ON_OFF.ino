

#include "b_data.h"

#include "b_SendToMeross.h"

#include <SPI.h>
#include <WiFiNINA.h>


////////////////
// WIFI
WiFiClient myClient;
WifiTools wTools;


////////////////
// MEROSS PLUG
IPAddress serverMSS210(192, 168, 0, 12);   // container for the plug's IP
WifiInterrupt merossPlug (myClient, MEROSS_FROM, MEROSS_MSG_ID, MEROSS_SIGN);


void setup() {
  merossPlug.setDebug(true);
  wTools.setDebug(true);

  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );

  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);


  // Launch the connection to the previously defined WIFI
  wTools.connectToWifi( SECRET_SSID, SECRET_PASS );

}

void loop() {
  merossPlug.sendSwitchWithMerossJson( serverMSS210, true );
  delay(5000);
  merossPlug.sendSwitchWithMerossJson( serverMSS210, false );
  delay(5000);
}
