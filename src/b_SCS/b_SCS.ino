/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020
	Hardware : 
		- Arduino MKR Wifi 1010
		- Push button
		- 47k resistor
		- optionally light capacitor (1µF-10µF)
	All this hardware is compatible with the SCS project. (see below for more details)
	SCS Official electronic schema : https://github.com/AIREL46/SCAO/raw/master/kicad/e-p-wifi-shield/e-p-wifi.png 
	Files : 
		- SCS.ino
		- SCS.h
	Dependencies :
		- none
	License :
    This work is made available under the terms of the
    Creative Commons Attribution BY 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This application is driven by the SCS (Smart Cooking System) project
	developed by A3C Presqu'île association.
	
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
		Its algorithm is purely constructor and not replicated in this 
		POC.
	* 	So it's needed to reproduce sniff describe in this wiki :
		https://github.com/ClementOrtiz/meross-mkr1010/wiki/Retrieving-Meross'-plug-frame-from-an-Ubuntu-OS
		To ensure that this program has the right messageIP and token
\**************************************************************************/

#define BUTTON_PIN 20                  // Button pin number


#include "b_data.h" //!\\ you need to modify data describe in this file

#include "b_SCS.h"


void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );
  
  B_SCS_Setup();

}

void loop()
{
  B_SCS_Loop();
}





