/*
  Button.h - Library for Arduino MKR WIFI 1010.

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
