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
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
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

#include <Arduino.h>
#include "WifiInterrupt.h"

WifiInterrupt::WifiInterrupt( WiFiClient client ):
  _client(client)
{
}

WifiInterrupt::WifiInterrupt( WiFiClient client, String merossFrom, String merossMsgId, String merossSign ):
  _client(client),
  _merossFrom(merossFrom),
  _merossMsgId(merossMsgId),
  _merossSign(merossSign)
{
}

void WifiInterrupt::setDebug( bool isDebug ){
  _isDebug = isDebug;
}

void WifiInterrupt::setPort( int port ){
  _port = port;
}

String WifiInterrupt::preparePayLoad( boolean onOffStatus ){
  return String("{\"header\":{\"from\":\"")+
      _merossFrom +
      "\",\"messageId\":\""+
      _merossMsgId +
      "\",\"method\":\"SET\",\"namespace\":\"Appliance.Control.ToggleX\",\"payloadVersion\":1,\"sign\":\""+
       _merossSign +
       "\",\"timestamp\":1583487856},\"payload\":{\"togglex\":{\"channel\":0,\"onoff\":"+
       (onOffStatus ? "1" : "0")+
       "}}}";
}

int WifiInterrupt::sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus, String &response ){
  String payload = preparePayLoad( onOffStatus );

  // If we're on debug mode, print some verbose about sending query
  if( _isDebug ){
    Serial.print( "Sending ");
    Serial.print( onOffStatus ? "ON": "OFF");
    Serial.print( " query on ");
    Serial.print( ip );
    Serial.println( " with following payload :" );
    Serial.println( payload );
  }


  int responseStatus = sendJsonRequest(ip, payload, response);

  // If we're on debug mode, print some verbose about response status
  if( _isDebug ){
    switch (responseStatus) {
      case REQUEST_TIMEOUT_RESPONSE:
        Serial.println("Fail : Reponse timeout...");
        break;
      case REQUEST_CONNECTION_FAILED:
        Serial.println("Fail : Connection failed...");
        break;
      case REQUEST_OK:
        Serial.println("Success : Request sent to host");
        Serial.println("Received response : ");
        Serial.println(response);
        break;
      default:
        Serial.println("??? : Unknown request status");
        break;
    }
    Serial.println();
  }

  return responseStatus;
}

int WifiInterrupt::sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus ){
  String response = "";
  return sendSwitchWithMerossJson(ip, onOffStatus, response);
}

int WifiInterrupt::sendJsonRequest( IPAddress ip, String jsonData, String &response ){
  int cpt=0;
  // reset previous connection
  _client.stop();

  // If connection is successfull, load payload with HTTP header
  if( _client.connect( ip, _port )){
    _client.print( // any spaces are important
      String("POST ") + "/config" + " HTTP/1.1\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: " + jsonData.length() + "\r\n" + //this line is needed with the exact size value
      "\r\n" + // CR+LF for header's end
      jsonData
    );

    // wait until data reception
    while( _client.available() == 0 ){
      cpt ++;
      delay( 100 );
      if (cpt > 49){
        // After 5s, break and return TIMEOUT
        return REQUEST_TIMEOUT_RESPONSE;
      }
    }

    // write all incoming data characters to &response
    while( _client.available() ){
      response += (char)_client.read();
    }
    return REQUEST_OK;
  }
  else{
    // if it couldn't make a connection:
    return REQUEST_CONNECTION_FAILED;
  }
}
