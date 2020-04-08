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


#include <Arduino.h>
#include "SoftUnbouncedButton.h"



SoftUnbouncedButton::SoftUnbouncedButton( int pinNumber ):
	_pinNumber(pinNumber),
	_tsLastState(millis())
{
}

SoftUnbouncedButton::SoftUnbouncedButton( int pinNumber, bool initialState  ):
	_pinNumber(pinNumber),
	_tsLastState(millis()),
	_switchState(initialState)
{
}

SoftUnbouncedButton::SoftUnbouncedButton( int pinNumber, bool initialState, long pollingInterval ):
	_pinNumber(pinNumber),
	_tsLastState(millis()),
	_switchState(initialState),
	_pollingInterval(pollingInterval)
{
}

bool SoftUnbouncedButton::getUnbouncedState(){
	// Reading Button's pin state
	long tmpState = digitalRead( _pinNumber );

	// If button state has changed
	if( tmpState != _currentState ){
		// Are we recording? Are we waiting for bounce to stop?
		if( !_recordingCurrent ){
			if(_isDebug){
				Serial.println("Starting recording");
			}
			// Start recording
			_recordingCurrent = true;
			// and store current time stamp
			_tsLastState = millis();
		
		// If Recording , do we have passed the polling interval?
		}else if(millis() - _tsLastState > _pollingInterval ){
			if(_isDebug){
				Serial.println("Stop recording : Changing button state");
			}
			// Stop recording
			_recordingCurrent = false;
			// Change state value
			_currentState = !_currentState;
		}
	}
	
	return _currentState;
}

bool SoftUnbouncedButton::getSwitchState(){
	bool tmpSwtSt = getUnbouncedState();

	if( tmpSwtSt ){
		_recordingSwitch = true;
	}
	else if( _recordingSwitch ){
    _recordingSwitch = false;
		_switchState = !_switchState;
	
		if(_isDebug){
					Serial.print( "Switch changed " );
					Serial.println( _switchState );
		}
	}		

  return _switchState;
}


void SoftUnbouncedButton::setDebug( bool debug ){
	_isDebug = debug;
}
