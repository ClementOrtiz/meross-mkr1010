#include <Arduino.h>
#include "b_SendToMeross.h"


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





WifiInterrupt::WifiInterrupt( WiFiClient client ):
  _client(client)
{
}

WifiInterrupt::WifiInterrupt( WiFiClient client, String merossFrom, String merossMsgId, String merossSign ):
  _client(client),
  _merossFrom(merossFrom),
  _merossMsgId(merossMsgId),
  _merossSign(merossSign)
{
}

void WifiInterrupt::setDebug( bool isDebug ){
  _isDebug = isDebug;
}

void WifiInterrupt::setPort( int port ){
  _port = port;
}

String WifiInterrupt::preparePayLoad( boolean onOffStatus ){
  return String("{\"header\":{\"from\":\"")+
      _merossFrom +
      "\",\"messageId\":\""+
      _merossMsgId +
      "\",\"method\":\"SET\",\"namespace\":\"Appliance.Control.ToggleX\",\"payloadVersion\":1,\"sign\":\""+
       _merossSign +
       "\",\"timestamp\":1588682404},\"payload\":{\"togglex\":{\"channel\":0,\"onoff\":"+
       (onOffStatus ? "1" : "0")+
       "}}}";
}

int WifiInterrupt::sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus, String &response ){
  String payload = preparePayLoad( onOffStatus );

  // If we're on debug mode, print some verbose about sending query
  if( _isDebug ){
    Serial.print( "Sending ");
    Serial.print( onOffStatus ? "ON": "OFF");
    Serial.print( " query on ");
    Serial.print( ip );
    Serial.println( " with following payload :" );
    Serial.println( payload );
  }


  int responseStatus = sendJsonRequest(ip, payload, response);

  // If we're on debug mode, print some verbose about response status
  if( _isDebug ){
    switch (responseStatus) {
      case REQUEST_TIMEOUT_RESPONSE:
        Serial.println("Fail : Reponse timeout...");
        break;
      case REQUEST_CONNECTION_FAILED:
        Serial.println("Fail : Connection failed...");
        break;
      case REQUEST_OK:
        Serial.println("Success : Request sent to host");
        Serial.println("Received response : ");
        Serial.println(response);
        break;
      default:
        Serial.println("??? : Unknown request status");
        break;
    }
    Serial.println();
  }

  return responseStatus;
}

int WifiInterrupt::sendSwitchWithMerossJson( IPAddress ip, bool onOffStatus ){
  String response = "";
  return sendSwitchWithMerossJson(ip, onOffStatus, response);
}

int WifiInterrupt::sendJsonRequest( IPAddress ip, String jsonData, String &response ){
  int cpt=0;
  // reset previous connection
  _client.stop();

  // If connection is successful, load payload with HTTP header
  if( _client.connect( ip, _port )){
    _client.print( // any spaces are important
      String("POST ") + "/config" + " HTTP/1.1\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: " + jsonData.length() + "\r\n" + //this line is needed with the exact size value
      "\r\n" + // CR+LF for header's end
      jsonData
    );

    // wait until data reception
    while( _client.available() == 0 ){
      cpt ++;
      delay( 100 );
      if (cpt > 49){
        // After 5s, break and return TIMEOUT
        return REQUEST_TIMEOUT_RESPONSE;
      }
    }

    // write all incoming data characters to &response
    while( _client.available() ){
      response += (char)_client.read();
    }
    return REQUEST_OK;
  }
  else{
    // if it couldn't make a connection:
    return REQUEST_CONNECTION_FAILED;
  }
}








void WifiTools::setDebug( bool isDebug ){
  _isDebug = isDebug;
}


void WifiTools::checkConnection(){
  if( _counterConnect > 1000 ){
    // if not connected, relaunch
    if( WiFi.status() != WL_CONNECTED ){
      if( _isDebug ){
        Serial.println("*** Warning ***\nWiFi Connection lost ... Trying to reconnect");
      }

      connectToWifi( _ssid.c_str(), _pass.c_str() );
    }
    _counterConnect = 0;
  }
  _counterConnect++;
}


void WifiTools::connectToWifi( const char ssid[] , const char pass[] ){
  // store ids
  _ssid = String( ssid );
  _pass = String( pass );

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE){
    if( _isDebug ){
      Serial.println("Communication with WiFi module failed!");
    }
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION){
    if( _isDebug ){
      Serial.println("Please upgrade the firmware");
    }
    // don't continue
    while (true);
  }

  // attempt to connect to Wifi network:
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED){
    if( _isDebug ){
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  if( _isDebug ){
    Serial.print("You're connected to the network");
  }
  printCurrentNet();
  printWifiData();
}


bool WifiTools::hostByName( char hostname[], IPAddress &plugIP ){
  bool founded = false;

  if( _isDebug ){
    Serial.println( "Launching research of plug IP" );
  }

  WiFi.hostByName(hostname, plugIP);

  if( plugIP[0] != 0 ){
    founded = true;
    if( _isDebug ){
      Serial.print("Plug found at ");
      Serial.println(plugIP);
    }
  }else if( _isDebug ){
      Serial.println("Plug not found ");
    }
  return founded;
}

void WifiTools::printWifiData(){
  if( !_isDebug )
    return;
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print( subnet );

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(" IP Address: ");
  Serial.print(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print(" MAC address: ");
  printMacAddress(mac);
}

void WifiTools::printCurrentNet(){
  if( !_isDebug )
    return;

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void WifiTools::printMacAddress( byte mac[] ){
  if( !_isDebug )
    return;

  for (int i = 5; i >= 0; i--){
    if (mac[i] < 16){
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);

    if (i > 0){
      Serial.print(":");
    }
  }
  Serial.println();
}
