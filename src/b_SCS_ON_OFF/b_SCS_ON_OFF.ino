

#include "b_data.h"
#include "b_SendToMeross.h"



////////////////
// WIFI
WifiTools wTools;


////////////////
// MEROSS PLUG
IPAddress plugIP(192, 168, 0, 12);   // container for the plug's IP
WifiInterrupt merossPlug (plugIP, MEROSS_FROM, MEROSS_MSG_ID, MEROSS_SIGN);


void setup() {
  // Allowing libs to print to serial
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
  merossPlug.sendSwitchWithMerossJson( true );
  delay(5000);
  merossPlug.sendSwitchWithMerossJson( false );
  delay(5000);
}
