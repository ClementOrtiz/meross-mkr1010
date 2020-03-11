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


extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
  #include "utility/debug.h"
}

   String WifiInterrupt::preparePayLoad( boolean onOffStatus ){
    }


    WifiInterrupt::WifiInterrupt( WiFiClient _client ){
    }
    WifiInterrupt::WifiInterrupt( WiFiClient _client, String merossAppToken, String merossMsgId, String merossSign ){
    }

    /*
     * Send Meross'specific Json request to a defined IP and retrieve the RESPONSE
     */
    int WifiInterrupt::sendSwitchWithMerossJson( IPAddress ip, boolean onOffStatus, String &response ){
    }


    /*
     * Send given JSON to IP and retrieve the RESPONSE
     */
    int WifiInterrupt::sendJsonRequest( IPAddress ip, String jsonData, String &response ){
    }

