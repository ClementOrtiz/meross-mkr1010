/**************************************************************************\
	Author : Clément ORTIZ
	Society : Decasoft
	Date : 13/03/2020

	Files needed : 
		- SoftUnbouncedButton.h
		- SoftUnbouncedButton.cpp
	Dependencies :
		- Arduino.h
	Copyright :
    This work is made available under the terms of the
    Creative Commons Attribution 4.0 International License.
    http://creativecommons.org/licenses/by/4.0
	___________________________________________________________________
	
	This Arduino file is the continuation of Mickaël Cherouise's work.
	https://github.com/isosel/meross-mkr1010
	___________________________________________________________________
	
	This Library is an encapsulation of function that manage the bounce of a push-button.
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

#ifndef SoftUnbouncedButton_h
#define SoftUnbouncedButton_h

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

#endif
