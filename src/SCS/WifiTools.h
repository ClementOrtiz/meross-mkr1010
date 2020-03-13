/*
  WifiTools.h - Library for Arduino MKR WIFI 1010.

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

#ifndef WifiTools_h
#define WifiTools_h


#include <Arduino.h>
#include "WiFi.h"

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
