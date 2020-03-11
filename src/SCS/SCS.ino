
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
IPAddress serverMSS210(0, 0, 0, 0);   //(192, 168, 1, 33); // or IPAddress serverMSS210(192,168,1,183);
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
      Serial.println();
      Serial.println( "=============================" );
      Serial.println( "Launching research of plug IP" );
      IPAddress plugIP(0, 0, 0, 0);
      WiFi.hostByName("Meross_Smart_Plug", plugIP);

      if( plugIP[0] != 0 ){
        Serial.print("Plug found at ");
        Serial.print(plugIP);

        serverMSS210 = plugIP;
        appMode = APPMODE_ORDERING;
        Serial.println();
        Serial.println( "=============================" );
        Serial.println("Application's now ordering plug");
      }

      break;
    }
    case APPMODE_ORDERING :
     onOffMode();
      break;
    default :
      Serial.println("Unknow Mode, application will stop");
      while (true);
  }

  // Checking WiFi connection
  if( counterConnect > 1000 ){
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
// Plug ordering Part

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
int sendRequestToIp( IPAddress ip, boolean onOffStatus, String &response ){

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
int sendFinalRequest( IPAddress ip, String jsonData, String & response ){
  int cpt=0;
  // Format String IP to CharArray IP
  //char ipCharArray[15];
  //ip.toCharArray(ipCharArray, ip.length()+1);

  // reset previous connection
  myClient.stop();

  // If connection is successfull, load payload with HTTP header
  //myClient.setConnectTimeout(500);
  if( myClient.connect( ip, portMSS210 )){
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
