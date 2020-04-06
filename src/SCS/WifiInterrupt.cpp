/*
  WifiInterrupt.cpp - Library for Arduino MKR WIFI 1010.

  Needs WifiNINA library under licence :
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "WifiInterrupt.h"

WifiInterrupt::WifiInterrupt( WiFiClient client ):
  _client(client)
{
}

WifiInterrupt::WifiInterrupt( WiFiClient client, String merossAppToken, String merossMsgId, String merossSign ):
  _client(client),
  _merossAppToken(merossAppToken),
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
      _merossAppToken +
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
