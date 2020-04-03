/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020
	Hardware : 
		- Arduino MKR Wifi 1010
		- Push button
		- 10k resistor
		- optionally light capacitor
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
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This application is driven by the SCS (Smart Cooking System) projet
	developped by A3C Presqu'île association.
	
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
		Its algorithm is purelly constructor and not replicated in this 
		POC.
	* 	So it's needed to reproduce sniff describe in this wiki :
		https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS
		To ensure that this programm has the right messageIP and token
\**************************************************************************/

#include <SPI.h>
#include <WiFiNINA.h>
#include "WifiInterrupt.h"
#include "WifiTools.h"

////////////////
// WIFI
#define SECRET_SSID "Bbox-FF996433"   // to be replaced by your router SSID
#define SECRET_PASS "34D476DC271ED15A1E165FFAF241AD"
WiFiClient myClient;
WifiTools wTools;


////////////////
// PIN
#define BUTTON_PIN 0                  // Button pin number


////////////////
// COMMUNICATION
int portMSS210 = 80;                  // comon port for HTTP communication
IPAddress serverMSS210(0, 0, 0, 0);   // container for the plug's IP
// Took from sniffed frames : (follow https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS to retriece them)
#define MEROSS_APP_TOKEN "551099-ef1d9fe37442284be4a06684de36c43d"
#define MEROSS_MSG_ID    "bdf5a8a37e18f8261ce7623687efcc21"
#define MEROSS_SIGN      "09ee94a666bf3f322ab3240bec0a6bc0"
#define MEROSS_HOSTNAME  "Meross_Smart_Plug"
WifiInterrupt merossPlug (myClient, MEROSS_APP_TOKEN, MEROSS_MSG_ID, MEROSS_SIGN);


////////////////
// APPLICATION MODES
#define APPMODE_SEARCHING 0           // Application is searching for plug's IP
#define APPMODE_SENDING   1           // Application is waiting to send orders
int appMode = APPMODE_SEARCHING;      // Stock the Application mode // Here we define the default starting mode


////////////////
// GLOBAL VARIABLES
int buttonState  = LOW;                // state of the push button
bool onOff 		 = false;                // current state of the plug
bool isDebug 	 = true;                 // Enable application to print on Serial
long waitingTime = 5000;               // Waiting time to retry hostByName



void setup()
{
  // Set on debug for lib's verbose on Serial
  merossPlug.setDebug(isDebug);
  wTools.setDebug(isDebug);

  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );
  
  // Defining electrical way 
  pinMode( BUTTON_PIN, INPUT ); 

  // wait for serial port to connect. Needed for native USB port only
  while (!Serial); 

  // Launch the connection to the previously defined WIFI
  wTools.connectToWifi( SECRET_SSID, SECRET_PASS );

  // Once every thing is done, power up the button
  //digitalWrite( BUTTON_POWER_PIN, HIGH );
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
          Serial.println( "\n=============================\nApplication's now wainting for sending queries to plug" );
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
    default :
      Serial.println("Unknown Mode, application will stop");
      while (true);
  }

  // Checking if WiFi connection still up
  wTools.checkConnection();
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
  buttonState = digitalRead( BUTTON_PIN );

  // if button is pushed, send query to plug
  if(buttonState == HIGH){
    int success = merossPlug.sendSwitchWithMerossJson( serverMSS210, onOff );

    // if querying was a success, change OnOff indicator status
    if( success == REQUEST_OK ){
      onOff = !onOff;
    }

    // and wait if the button is still holded...
    while( digitalRead(BUTTON_PIN) == HIGH ){
      delay(100);
    }
  }
}
