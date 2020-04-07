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



Button::Button( int pinNumber ):
	_pinNumber(pinNumber),
	_tsLastState(millis())
{
}

Button::Button( int pinNumber, bool initialState  ):
	_pinNumber(pinNumber),
	_tsLastState(millis()),
	_switchState(initialState)
{
}

Button::Button( int pinNumber, bool initialState, long pollingInterval ):
	_pinNumber(pinNumber),
	_tsLastState(millis()),
	_switchState(initialState),
	_pollingInterval(pollingInterval)
{
}

bool Button::getUnbouncedState(){
	// Reading Button's pin state
	long tmpState = digitalRead( _pinNumber );

	// If button state has changed
	if( tmpState != _currentState ){
		// Are we recording? Are we waiting for bounce to stop?
		if( !_recording ){
			// Start recording
			_recording = true;
			// and store current time stamp
			_tsLastState = millis();
		
		// If Recording , do we have passed the polling interval?
		}else if(millis() - _tsLastState > _pollingInterval ){
			// Stop recording
			_recording = false;
			// Change state value
			_currentState = !_currentState;
		}
	}
	
	return _currentState;
}

bool Button::getSwitchState(){
	bool tmpSwtSt = getUnbouncedState();
	
	if( tmpSwtSt != _switchState ){
		_switchState = !_switchState;
	}		
}
