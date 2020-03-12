
#include <SPI.h>
#include <WiFiNINA.h>
#include "WifiInterrupt.h"
#include "WifiTools.h"

// WIFI
#define SECRET_SSID "Bbox-FF996433"           // to be replaced by your router SSID
#define SECRET_PASS "34D476DC271ED15A1E165FFAF241AD"
WiFiClient myClient;
WifiTools wTools;


// PIN
#define BUTTON_PIN 0                // Button pin number
#define BUTTON_POWER_PIN 6          // Button power pin number => ensure that "BUTTON_PIN" receive 3.3v instead of 5v that it's not recommended


// COMMUNICATION
int portMSS210 = 80;                  // comon port for HTTP communication
IPAddress serverMSS210(0, 0, 0, 0);   //(192, 168, 1, 33); // or IPAddress serverMSS210(192,168,1,183);
// Took from sniffed frames : (declared as String to simplify JSON integration)
String MEROSS_APP_TOKEN = "551099-ef1d9fe37442284be4a06684de36c43d";
String MEROSS_MSG_ID    = "bdf5a8a37e18f8261ce7623687efcc21";
String MEROSS_SIGN      = "09ee94a666bf3f322ab3240bec0a6bc0";
WifiInterrupt merossPlug (myClient, MEROSS_APP_TOKEN, MEROSS_MSG_ID, MEROSS_SIGN);


// APPLICATION MODES
#define APPMODE_SEARCHING 0           // Application is searching for plug's IP
#define APPMODE_ORDERING  1           // Application is waiting to send orders
int appMode = APPMODE_SEARCHING;      // Stock the Application mode // Here we define the default starting mode


// GLOBAL VARIABLES
int buttonState = LOW;                // state of the push button
bool onOff = false;                   // current state of the plug
bool isDebug = true;                  // Enable application to print on Serial



void setup()
{
  merossPlug.setDebug(isDebug);
  wTools.setDebug(isDebug);

  //Initialize serial and wait for port to open:
  Serial.begin( 9600 );
  pinMode( BUTTON_PIN, INPUT );
  pinMode( BUTTON_POWER_PIN, OUTPUT );

  while (!Serial); // wait for serial port to connect. Needed for native USB port only

  wTools.connectToWifi( SECRET_SSID, SECRET_PASS );

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
  wTools.checkConnection();
}




////////////////////
// Plug ordering Part

void onOffMode(){
  // Reading Button's pin state
  buttonState = digitalRead( BUTTON_PIN );

  // if button is pushed, send query to plug
  if(buttonState == HIGH){
    int success = merossPlug.sendSwitchWithMerossJson( serverMSS210, onOff );

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
