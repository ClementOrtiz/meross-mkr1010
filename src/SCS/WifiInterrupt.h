/*
  WifiInterrupt.h - Library for Arduino MKR WIFI 1010.

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

#ifndef WifiInterrupt_h
#define WifiInterrupt_h

#define REQUEST_OK 0                  // request was a success
#define REQUEST_TIMEOUT_RESPONSE 1    // request has no response
#define REQUEST_CONNECTION_FAILED 2   // request failed to connect



#include "IPAddress.h"
#include "WiFiClient.h"

class WifiInterrupt{
  private:
    bool _isDebug;
    WiFiClient _client;

    /*
     * Prepare the payload from contextuals parameters
     */
    String preparePayLoad( boolean onOffStatus );

  public:
    WifiInterrupt( WiFiClient _client );
    WifiInterrupt( WiFiClient _client, String merossAppToken, String merossMsgId, String merossSign );

    /*
     * Send Meross'specific Json request to a defined IP and retrieve the RESPONSE
     */
    int sendSwitchWithMerossJson( IPAddress ip, boolean onOffStatus, String &response );


    /*
     * Send given JSON to IP and retrieve the RESPONSE
     */
    int sendJsonRequest( IPAddress ip, String jsonData, String &response );

}

#endif
