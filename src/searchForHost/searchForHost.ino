// for WPA2 encrypted connections
// By Mickael Cherouise
// Thanks to Ilias Nafili alias IliShu for his help in understanding http protocol and node.js server.
// This programme will be divided into 4 parts, described below

// ==================================================================================================================================
// ============================================== PART 1 : Initialisation of variables ==============================================
// ==================================================================================================================================
// We need SPI.h and WiFiNINA.h labraries for this program
// SPI.h is needeed to display wifi information on the serial monitor
// WiFiNINA is needed to set up the wifi communication
// WiFiClient myClient is an object which is the client who will be connected to the network
// to be compliant with wifi standards, some packet have to be exchanged between the client and the router
// in order to keep the connection ACTIVE
//
// SECRET_SSID and SECRET_PASS are the Identifier and the Password of the internet access point you're trying to connect to.
// the variable status will contain a value which determine the status of the network, if you're connected or not
// the variable "compteur" will me incremented until it reachs 50 with a delay of 100ms
// When the variable reaches the value 50, 5 seconds have passed.
// The if condition is TRUE then the function printCurrentNet() is executed and display network informations
//
// portMSS210 and serverMSS210 are respectively the port and the IP adress of the internal Smart Wifi Plug web Meross server.
// We need those information to establish a connection with the Meross server and to send HTTP request
// buttonStateOff/On listen inputs on the MKR1010 and record button states
// if we press buttons, the value HIGH is saved within those variable, if buttons are unpressed, the value LOW is saved
//
// The variable postData is a string which contains the JSON object which has to be sent within a HTTP request to the web server of the Smart Wifi Plug.
// The JSON object string has be stringified https://onlinetexttools.com/json-stringify-text, double quote " have to be replaced by \" within main double quote "
// e.g. ""value1" = 50, "value2" = 25" --> "\"value1\" = 50, \"value2\" = 25"
// codeResponse is a variable which will contain incoming characters sent back to the MKR1010 from the Meross Smart Wifi Pluf after a HTTP request
// if the function myClient.available() return true, it means that characters are detected on the channel between the meross smart wifi plug and the MKR1010
// then while the function return true, we do codeResponse = myClient.read() to read incoming characters
// then we do a "serial.print(codeResponse)" to display incoming characters within the serial monitor window




// ==================================================================================================================================
// ==============================================       Modifications History         ==============================================
// ==================================================================================================================================
// 06/03/2020 - Clément ORTIZ :
// - Powering the button with pin 6 to avoid the high tension on reader pin BUTTON_PIN
//    MKR read 3.3v and previous connection was on main power that is on 5v
// - Externalisation of "Meross" variables as App Token, messageId and sign
// - Little refactoring
//      * Globalising PIN numbers
//      * removing useless global vars and code repetition

#include <SPI.h>
#include <WiFiNINA.h>

// WIFI
#define SECRET_SSID "Bbox-FF996433"           // to be replaced by your router SSID
#define SECRET_PASS "34D476DC271ED15A1E165FFAF241AD"
WiFiClient myClient;
int status = WL_IDLE_STATUS;


// PIN
#define BUTTON_PIN 0                // Button pin number
#define BUTTON_POWER_PIN 6          // Button power pin number => ensure that "BUTTON_PIN" receive 3.3v instead of 5v that it's not recommended


// COMMUNICATION
#define REQUEST_OK 0                  // Sending request was a success
#define REQUEST_TIMEOUT_RESPONSE 1    // Sending request has no response
#define REQUEST_CONNECTION_FAILED 2   // Sending request failed to connect
int portMSS210 = 80;                  // comon port for HTTP communication
String serverMSS210 = "192.168.1.33"; // or IPAddress serverMSS210(192,168,1,183);
// Took from sniffed frames : (declared as String to simplify JSON integration)
String MEROSS_APP_TOKEN = "551099-ef1d9fe37442284be4a06684de36c43d";
String MEROSS_MSG_ID    = "bdf5a8a37e18f8261ce7623687efcc21";
String MEROSS_SIGN      = "09ee94a666bf3f322ab3240bec0a6bc0";


// APPLICATION MODES
#define APPMODE_SEARCHING 0           // Application is searching for plug's IP
#define APPMODE_ORDERING  1           // Application is waiting to send orders
int appMode = APPMODE_SEARCHING;      // Stock the Application mode // Here we define the default starting mode


// GLOBAL VARIABLES
int counterConnect;                   // check the connection
int buttonState = LOW;                // state of the push button
bool onOff = false;                   // current state of the plug
bool isDebug = true;                  // Enable verbose



void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );
  pinMode( BUTTON_PIN, INPUT );
  pinMode( BUTTON_POWER_PIN, OUTPUT );

  while (!Serial); // wait for serial port to connect. Needed for native USB port only

  connectToWifi( SECRET_SSID, SECRET_PASS );

  // Once every thing is done, power up the button
  digitalWrite( BUTTON_POWER_PIN, HIGH );
}

void loop()
{
  switch( appMode ){
    case APPMODE_SEARCHING :{
      Serial.println( "Launching research of plug IP" );
      WiFi.setConnectTimeout(500);
      WiFi.test();
      IPAddress plugIP(0, 0, 0, 0);
      if( searchForIp( plugIP ) ){
        serverMSS210 = (String) plugIP;
        appMode = APPMODE_ORDERING;
      }
      break;
    }
    case APPMODE_ORDERING :
      WiFi.setConnectTimeout(2000);
      onOffMode();
      break;
    default :
      Serial.println("Unknow Mode, application will stop");
      while (true);
  }

  // Checking WiFi connection
  if( counterConnect > 1000){
    // if not connected, relaunch
    if( WiFi.status() != WL_CONNECTED ){
      Serial.println("*** Warning ***\nWiFi Connection lost ... Trying to reconnect");
      connectToWifi( SECRET_SSID, SECRET_PASS );
    }
    counterConnect = 0;
  }
  counterConnect++;
}

////////////////////
// Search for IP Part

bool searchForIp( IPAddress &ipResult ){


  IPAddress localIp = WiFi.localIP();
  IPAddress minIp = WiFi.localIP();
  IPAddress maxIp = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();

  // retrieve min and max ip from the subnet mask
  int maskOffSet = basicIpRangeFromMaskAndLocalIp( localIp, subnet, minIp, maxIp );

  if( isDebug ){
    Serial.print("minIp: ");
    Serial.print( minIp );
    Serial.print(" maxIp: ");
    Serial.println( maxIp );
  }

  // Clearest way to move around IP
  // It's possible to do it with only one loop... but hard to undestand and debug
  bool finded = false;
  for (int ip1 = minIp[0]; ip1 <= maxIp[0] && !finded; ip1++) {
    for (int ip2 = minIp[1]; ip2 <= maxIp[1] && !finded; ip2++) {
      for (int ip3 = minIp[2]; ip3 <= maxIp[2] && !finded; ip3++) {
        for (int ip4 = minIp[3]; ip4 <= maxIp[3] && !finded; ip4++) {
          IPAddress ipTested (ip1, ip2, ip3, ip4);
          Serial.print( "Testing : " );
          Serial.println( ipTested );

          if( testIpForFrameInjection( ipTested ) ){
            ipResult = ipTested;
            finded = true;
          }
          delay(1000); // to avoid to flood network
        }
      }
    }
  }

  return finded;
}

/**
* As written in function name, a basic calculation of min and max IP from a local IP and a subnet.
* Here we consider that a non zero digit on subnet mask refer to a 0.
* As this, the calculated range will be larger than the real range and we'll don't forget ip that need to be scanned
* return the offset of the zeroSubNetPos
**/
int basicIpRangeFromMaskAndLocalIp( IPAddress localIp, IPAddress subnet, IPAddress &minIp, IPAddress &maxIp ){
  int maxSizeAdress = 4;
  int zeroSubNetPos = maxSizeAdress;
  // Search for '0' pos (==> really search for a none 255 to simplify subnet calculation)
  for (int i = 0; i < maxSizeAdress; i++) {
    if( subnet[i] != 255 ){
      zeroSubNetPos = i;
      break;
    }
  }

  // formating minIp and maxIp
  for (int i = 0; i < maxSizeAdress; i++) {
    // if we haven't passed the 0 position on the subnet, basis adress is the same as localIp
    if( i < zeroSubNetPos ){
      minIp[i] = localIp[i];
      maxIp[i] = localIp[i];
    }else{
      // if zeroSubNetPos is reach, min is 0 and max is 255
      minIp[i] = 0;
      maxIp[i] = 255;
    }
  }
  return zeroSubNetPos;
}

/**
* Send a ping to know if there's a host behind
* and then send a "OFF frame" and analyse the data answer
**/
bool testIpForFrameInjection( IPAddress ip ){
  // int result = WiFi.ping( ip );
  // Serial.print( "ping result : " );
  // Serial.print( result );
  // Serial.print( " ==? " );
  // Serial.println( WL_SUCCESS );
  // if( result > 0 ){
  //   Serial.print("Ping ok on ");
  //   Serial.println(ip);
    return testPayLoadInjection(ip);
  // }
  //
  // return false;
}

/**
* Launch a payload on the given IP and return true if the MEROSS_MSG_ID is found in the response.
**/
bool testPayLoadInjection( IPAddress ip ){
  String response = "";
  int success = sendRequestToIp( (String)ip, false, response );
  if( success == REQUEST_OK ){
      if( response.indexOf( MEROSS_MSG_ID ) > 0){
        Serial.println("Plug found");
        return true;
      }
  }

  return false;
}

////////////////////
// Plug Manipulation Part

void onOffMode(){
  // Reading Button's pin state
  buttonState = digitalRead( BUTTON_PIN );

  // if button is pushed, send query to plug
  if(buttonState == HIGH){
    String response = "";
    int success = sendRequestToIp( serverMSS210, onOff, response );

    // if querying was a success, change OnOff indicator status
    if( success == REQUEST_OK ){
      onOff = !onOff;
    }

    // and wait if the button is still holded...
    while( digitalRead(BUTTON_PIN) == HIGH ){
      delay(100);
    }
  }
}

/**
* Send the full HTTP Meross' request to a defined IP
**/
int sendRequestToIp( String ip, boolean onOffStatus, String &response ){

  String payload = preparePayLoad( onOffStatus );

  // If we're on debug mode, print some verbose about sending query
  if( isDebug ){
    Serial.print( "Sending ");
    Serial.print( onOffStatus ? "ON": "OFF");
    Serial.print( " query on ");
    Serial.print( ip );
    Serial.println( " with following payload :" );
    Serial.println( payload );
  }


  int responseStatus = sendFinalRequest(ip, payload, response);

  // If we're on debug mode, print some verbose about response status
  if( isDebug ){
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

/**
* Prepare the payload from contextuals parameters
**/
String preparePayLoad( boolean onOffStatus ){
  return String("{\"header\":{\"from\":\"/app/")+
      MEROSS_APP_TOKEN +
      "/subscribe\",\"messageId\":\""+
      MEROSS_MSG_ID +
      "\",\"method\":\"SET\",\"namespace\":\"Appliance.Control.ToggleX\",\"payloadVersion\":1,\"sign\":\""+
       MEROSS_SIGN +
       "\",\"timestamp\":1583487856},\"payload\":{\"togglex\":{\"channel\":0,\"onoff\":"+
       (onOffStatus ? "1" : "0")+
       "}}}";
}


/**
* Technically send to IP the payload JSONDATA and retrieve the RESPONSE
**/
int sendFinalRequest( String ip, String jsonData, String & response ){
  int cpt=0;
  // Format String IP to CharArray IP
  char ipCharArray[15];
  ip.toCharArray(ipCharArray, ip.length()+1);

  // reset previous connection
  myClient.stop();

  // If connection is successfull, load payload with HTTP header
  if( myClient.connect( ipCharArray, portMSS210 )){
    myClient.print( // any spaces are important
      String("POST ") + "/config" + " HTTP/1.1\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: " + jsonData.length() + "\r\n" + //this line is needed with the exact size value
      "\r\n" + // CR+LF for header's end
      jsonData
    );

    //wait until data reception
    while( myClient.available() == 0 ){
      cpt ++;
      delay( 100 );
      if (cpt > 49){
        // After 5s, break and return TIMEOUT
        return REQUEST_TIMEOUT_RESPONSE;
      }
    }

    // write all incoming data characters to &response
    while( myClient.available() ){
      response += (char)myClient.read();
    }
    return REQUEST_OK;
  }
  else{
    // if you couldn't make a connection:
    return REQUEST_CONNECTION_FAILED;
  }
}



////////////////////
// Tools Part

void connectToWifi( char ssid[] , char pass[] ){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE){
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION){
    Serial.println("Please upgrade the firmware");
    // don't continue
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED){
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
}


void printWifiData(){
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NETMASK: ");
  Serial.print( subnet );

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
  delay(1000);
}

void printCurrentNet(){
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

void printMacAddress( byte mac[] ){
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
