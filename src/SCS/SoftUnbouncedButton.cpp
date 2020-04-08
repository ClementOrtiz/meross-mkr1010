/*
  Button.cpp  - Library for Arduino MKR WIFI 1010.

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
