/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 04/05/2020

	Files needed : 
		- B_SCS.h
	Dependencies :
		- SPI.h
		- WiFiNINA.h
		- B_SendToMeross.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This header file is made to grab into one place all the requirements to
	enable sending Meross's frame to a Meross plug when pushing a button.
	
\**************************************************************************/

#include <SPI.h>
#include <WiFiNINA.h>
#include "b_SendToMeross.h"

////////////////////////////////////////
// Data that you need to modify

#define SECRET_SSID "XXXX"  // to be replaced by your router SSID
#define SECRET_PASS "XXXX"

// Took from sniffed frames : (follow https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS to retrieve them)
#define MEROSS_FROM      "/app/551099-255910cc5b1fa6ae07c6955441e14c66/subscribe"
#define MEROSS_MSG_ID    "015a32224121606ab6c94f31e4bd8e81"
#define MEROSS_SIGN      "5f8f0d7ea7bd27b33e5920e9fec7425e"
#define MEROSS_HOSTNAME  "merosssmartplug"

///////////////////////////////////////
// Data that you can modify
bool waitForSerial = true;           // If 'true', application will wait on start-up that user connect to serial port console
bool isDebug 	     = true;           // Enable application to print on Serial
//////////////////////////////////////



////////////////
// WIFI
WiFiClient myClient;
WifiTools wTools;


////////////////
// PIN
boolean onOff = false;                 // Store the plug state (false = OFF by default)
SoftUnbouncedButton button(BUTTON_PIN, onOff, 20); // SoftUnbouncedButton on pin20=>A5, starting in OFF state, waiting 20ms before read


////////////////
// MEROSS PLUG
int portMSS210 = 80;                  // comon port for HTTP communication
IPAddress serverMSS210(0, 0, 0, 0);   // container for the plug's IP
WifiInterrupt merossPlug (myClient, MEROSS_FROM, MEROSS_MSG_ID, MEROSS_SIGN);



////////////////
// APPLICATION MODES
#define APPMODE_SEARCHING 0           // Application is searching for plug's IP
#define APPMODE_SENDING   1           // Application is waiting to send orders
#define APPMODE_BTN_TEST  2           // Application is in button test mode
int appMode = APPMODE_SENDING;      // Stores the Application mode => Here we define the default starting mode


////////////////
// GLOBAL VARIABLES
long waitingTime = 5000;               // Waiting time to retry hostByName



void B_SCS_Setup(){
  // Set on debug for lib's verbose on Serial
  merossPlug.setDebug(isDebug);
  wTools.setDebug(isDebug);
  button.setDebug(false); // we don't want to see button debug's informations
  
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

void B_SCS_Loop(){
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
