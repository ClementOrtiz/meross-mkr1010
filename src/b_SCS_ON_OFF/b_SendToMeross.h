/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 04/05/2020

	Files needed : 
		- B_SendToMeross.h
		- B_SendToMeross.cpp
	Dependencies :
		- Arduino.h
		- IPAddress.h
		- WiFiClient.h
		- WiFi.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This library is an encapsulation of all the functions and objects needed to use Meross's Smart Plug
	as an Interruptor.
	It facilitate the access of On/Off switching that allow this Meross' Smart
	Plug.
	
	Main goal is to reduce the number of files.
	Instead of having a .h and .cpp for each Object. Here we have only one file containing the
	declaration of the 3 objects needed.
	- SoftUnbouncedButton
	- WifiInterrupt
	- WifiTools
	
\**************************************************************************/

#ifndef B_SendToMeross_h
#define B_SendToMeross_h

#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "IPAddress.h"
#include "WiFiClient.h"
#include "WiFi.h"





/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- B_SendToMeross.h
		- B_SendToMeross.cpp
	Dependencies :
		- Arduino.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This class is an encapsulation of function that manage the bounce of a push-button.
	On first incoming front, it launch a timer to avoid picking bounced information.
	
	To use it, you need to instantiate a SoftUnbouncedButton giving at least
	the pin number on witch the button is connected.
	(You can also use 2 other constructor that allow you to set the initial state (On or Off)
	and an other allows to set the minimal interval to read final state.)
	
	Then call periodically the function "getUnbouncedState" will give you a boolean
	indicating the state of the pin.
	Calling periodically the function "getSwitchState" will give you a boolean
	indicating the "switch state".
	"Switch state" is a logical value that change each time a up front is detected
	with "getUnbouncedState" function.	
\**************************************************************************/
class SoftUnbouncedButton{
	private:
		int _pinNumber;
		bool _isDebug = false;
		bool _switchState = false;
		bool _currentState = false;
		bool _recordingCurrent = false;
		bool _recordingSwitch = false;
		long _tsLastState = 0;
		
		long _pollingInterval = 20;
		
	public :
		SoftUnbouncedButton( int pinNumber );
		SoftUnbouncedButton( int pinNumber, bool initialState  );
		SoftUnbouncedButton( int pinNumber, bool initialState, long pollingInterval );
		
		bool getUnbouncedState();
		bool getSwitchState();
		void setDebug( bool debug );
		
};




/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- B_SendToMeross.h
		- B_SendToMeross.cpp
	Dependencies :
		- Arduino.h
		- IPAddress.h
		- WiFiClient.h
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This Class is an encapsulation of the function to use Meross's Smart Plug
	as an Interruptor.
	It facilitate the access of On/Off switching that allow this Meross' Smart
	Plug.
	
	To use it, you need to instantiate an Object giving data from sniffed frame
	- from
	- message_id
	- sign
	
	Then you just have to call "sendSwitchWithMerossJson" function giving :
	- Meross Smart Plug IP address
	- a boolean indicating if you'd like to switch On or Off
	- an Optionnal reference of a String object to retrieve the respond of the plug
	
\**************************************************************************/

#define REQUEST_OK 0                  // request was a success
#define REQUEST_TIMEOUT_RESPONSE 1    // request has no response
#define REQUEST_CONNECTION_FAILED 2   // request failed to connect




class WifiInterrupt{
  private:
    int _port = 80;
    bool _isDebug = false;
    WiFiClient *_client;
		IPAddress _ip;
    String _merossFrom;
    String _merossMsgId;
    String _merossSign;

    /*
     * Prepare the payload from contextuals parameters
     */
    String preparePayLoad( boolean onOffStatus );

  public:
    WifiInterrupt( IPAddress ip );
    WifiInterrupt( IPAddress ip, String merossFrom, String merossMsgId, String merossSign );

    /*
     * Define if this lib will print on Serial
     */
    void setDebug( bool isDebug );

    /*
     * Define the conection port used to request
     */
    void setPort( int port );

    /*
     * Send Meross'specific Json request to a defined IP and retrieve the RESPONSE
     */
    int sendSwitchWithMerossJson( bool onOffStatus, String &response );

    /*
     * Send Meross'specific Json request to a defined IP and no matter the RESPONSE
     */
    int sendSwitchWithMerossJson( bool onOffStatus );

    /*
     * Send given JSON to IP and retrieve the RESPONSE
     */
    int sendJsonRequest( String jsonData, String &response );

};


/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- B_SendToMeross.h
		- B_SendToMeross.cpp
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
	
	This Class is an encapsulation of technical and graphical functions.
	It help for wifi connection and check if connection still alive.
	Also help to find host by name.
	3 functions are dedicated to print information about Wifi and Lan
	
\**************************************************************************/



class WifiTools{
  private:
    bool _isDebug = false;
    int _counterConnect = 0;
    String _ssid;
    String _pass;

  public:

    /*
     * Define if this lib will print on Serial
     */
    void setDebug( bool isDebug );

    /*
     * Check if connection still up and relaunch if not
     */
    void checkConnection();

    /*
     * Launch a connection on the Hotspot specified
     */
    void connectToWifi( const char ssid[] , const char pass[] );

    /*
     * Search the IP of the requested hostname
     */
    bool hostByName( char hostname[], IPAddress &plugIP );

    /*
     * Print subnet mask, local IP and MAC address
     */
    void printWifiData();

    /*
     * Print current Wifi connection informations
     */
    void printCurrentNet();

    /*
     * Print formatted MAC address
     */
    void printMacAddress( byte mac[] );
};
#endif
