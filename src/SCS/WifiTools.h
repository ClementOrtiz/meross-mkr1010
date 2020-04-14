/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- WifiTools.h
		- WifiTools.cpp
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
	
	This Library is an encapsulation of technical and graphical functions.
	It help for wifi connection and check if connection still alive.
	Also help to find host by name.
	3 functions are dedicated to print information about Wifi and Lan
	
\**************************************************************************/

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
