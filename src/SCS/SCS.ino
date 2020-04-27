/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020
	Hardware : 
		- Arduino MKR Wifi 1010
		- Push button
		- 47k resistor
		- optionally light capacitor (1µF-10µF)
	All this hardware is compatible with the SCS project. (see below for more details)
	SCS Official electronic schema : https://github.com/AIREL46/SCAO/raw/master/kicad/e-p-wifi-shield/e-p-wifi.png 
	Files : 
		- SCS.ino
		- WifiInterrupt.h
		- WifiInterrupt.cpp
		- WifiTools.h
		- WifiTools.cpp
	Dependencies :
		- Arduino.h
		- SPI.h
		- WiFiNINA.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This application is driven by the SCS (Smart Cooking System) project
	developed by A3C Presqu'île association.
	
	The goal of those files is to make a POC (Proof Of Concept) on how 
	to drive a WiFi Meross smart plug with a MKR Wifi 1010.
	* 	This POC implement an auto-retrieve of plug's IP address based 
		on hostByName Wifi library.
	*	This POC also implement the sending of an HTTP query on plug's
		IP to emulate the Android's application call.
		As this, this application is able to switch ON and OFF the 
		smart plug.
		This ability is launch when user push the push-button.
		
	Prerequisite :
	Having an example of the frame sent by the Android application.
	*	Meross'plug communication is based on a combination of 
		messageID and handshake/token.
		Its algorithm is purely constructor and not replicated in this 
		POC.
	* 	So it's needed to reproduce sniff describe in this wiki :
		https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS
		To ensure that this program has the right messageIP and token
\**************************************************************************/

#include <SPI.h>
#include <WiFiNINA.h>
#include "WifiInterrupt.h"
#include "WifiTools.h"
#include "SoftUnbouncedButton.h"

////////////////
// WIFI
#define SECRET_SSID "XXXX"  // to be replaced by your router SSID
#define SECRET_PASS "XXXX" 
WiFiClient myClient;
WifiTools wTools;


////////////////
// PIN
#define BUTTON_PIN 20                  // Button pin number
boolean onOff = false;                 // Store the plug state (false = OFF by default)
SoftUnbouncedButton button(BUTTON_PIN, onOff, 20); // SoftUnbouncedButton on pin20=>A5, starting in OFF state, waiting 20ms before read



////////////////
// COMMUNICATION
int portMSS210 = 80;                  // comon port for HTTP communication
IPAddress serverMSS210(192, 168, 1, 14);   // container for the plug's IP
// Took from sniffed frames : (follow https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS to retriece them)
#define MEROSS_FROM      "/app/551099-255910cc5b1fa6ae07c6955441e14c66/subscribe"
#define MEROSS_MSG_ID    "015a32224121606ab6c94f31e4bd8e81"
#define MEROSS_SIGN      "5f8f0d7ea7bd27b33e5920e9fec7425e"
#define MEROSS_HOSTNAME  "merosssmartplug"
WifiInterrupt merossPlug (myClient, MEROSS_FROM, MEROSS_MSG_ID, MEROSS_SIGN);


////////////////
// APPLICATION MODES
#define APPMODE_SEARCHING 0           // Application is searching for plug's IP
#define APPMODE_SENDING   1           // Application is waiting to send orders
#define APPMODE_BTN_TEST  2           // Application is button test mode
int appMode = APPMODE_SENDING;      // Stores the Application mode => Here we define the default starting mode


////////////////
// GLOBAL VARIABLES
bool isDebug 	 = true;                 // Enable application to print on Serial
long waitingTime = 5000;               // Waiting time to retry hostByName
bool waitForSerial = true;           // If 'true', application will wait on start-up that user connect to serial port console



void setup()
{
  // Set on debug for lib's verbose on Serial
  merossPlug.setDebug(isDebug);
  wTools.setDebug(isDebug);
  button.setDebug(false); // we don't want to see button debug's informations

  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );
  
  // Defining electrical way 
  pinMode( BUTTON_PIN, INPUT ); 

  // wait for serial port to connect. Needed for native USB port only
  if( waitForSerial ){
    while (!Serial);
  }

  // Launch the connection to the previously defined WIFI
  if( appMode != APPMODE_BTN_TEST ){
    wTools.connectToWifi( SECRET_SSID, SECRET_PASS );
  }else{
    Serial.println("App in Button test mode. Please click button");
  }
}

void loop()
{
  // switching on the different mode of the application (Searching for ip or Sending query)
  switch( appMode ){
    case APPMODE_SEARCHING :{
      if( isDebug ){
        Serial.println( "\n=============================" );
      }
	  
	  // Launch the plug's IP research
    if( wTools.hostByName( MEROSS_HOSTNAME, serverMSS210) ){
        // if IP found, change application mode to "Sending"
        appMode = APPMODE_SENDING;

        if( isDebug ){
          Serial.println( "\n=============================\nApplication's now waiting for sending queries to plug" );
        }
      
	  }else{
        // if IP not found, wait a few second and relaunch research
        if( isDebug ){
          Serial.print( "Waiting " );
          Serial.print( waitingTime/1000 );
          Serial.print( "s and trying again..." );
        }
		delay(waitingTime);
	  }

      break;
    }
    case APPMODE_SENDING :
      // if app on "Sending" mode, launch On/Off manager
      onOffMode();
      break;
    case APPMODE_BTN_TEST :{
      bool state = button.getSwitchState();
      if( state != onOff ){
        onOff = state;
        
        if( onOff ){
          Serial.println("ON command received");
        }else{
          Serial.println("OFF command received");
        }
      }
      break;
    }
    default :
      Serial.println("Unknown Mode, application will stop");
      while (true);
  }

  // Checking if WiFi connection still up
  if( appMode != APPMODE_BTN_TEST ){
    wTools.checkConnection();
  }
}




/**
 * Function managing user action on push-button
 * When user push the button down,
 * Actual state (On or Off) is sent to the plug
 * Then state is reverse
 * And app's waiting for user to release the button
 */
void onOffMode(){
  // Reading Button's pin state
  bool state = button.getSwitchState();
  // if state changed
  if( state != onOff ){
    // send query to plug
    merossPlug.sendSwitchWithMerossJson( serverMSS210, onOff );
    onOff = state;
  }
}
