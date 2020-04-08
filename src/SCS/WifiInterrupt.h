/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- WifiInterrupt.h
		- WifiInterrupt.cpp
	Dependencies :
		- Arduino.h
		- IPAddress.h
		- WiFiClient.h
	Copyright :
    This work is made available under the terms of the
    Creative Commons Attribution 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This Library is an encapsulation of the function to use Meross's Smart Plug
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

#ifndef WifiInterrupt_h
#define WifiInterrupt_h

#define REQUEST_OK 0                  // request was a success
#define REQUEST_TIMEOUT_RESPONSE 1    // request has no response
#define REQUEST_CONNECTION_FAILED 2   // request failed to connect


#include <Arduino.h>
#include "IPAddress.h"
#include "WiFiClient.h"

class WifiInterrupt{
  private:
    int _port = 80;
    bool _isDebug = false;
    WiFiClient _client;
    String _merossFrom;
    String _merossMsgId;
    String _merossSign;

    /*
     * Prepare the payload from contextuals parameters
     */
    String preparePayLoad( boolean onOffStatus );

  public:
    WifiInterrupt( WiFiClient client );
    WifiInterrupt( WiFiClient client, String merossFrom, String merossMsgId, String merossSign );

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
    int sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus, String &response );

    /*
     * Send Meross'specific Json request to a defined IP and no matter the RESPONSE
     */
    int sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus );

    /*
     * Send given JSON to IP and retrieve the RESPONSE
     */
    int sendJsonRequest( IPAddress ip, String jsonData, String &response );

};

#endif
