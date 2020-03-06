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


// ==============================================================================================================================
// ============================================== PART 2 : the void setup function ==============================================
// ==============================================================================================================================
// This is the function that will etablish the wifi connection using variables described above
// Once it's done, the lopp function is executed

// =============================================================================================================================
// ============================================== PART 3 : the void loop function ==============================================
// =============================================================================================================================
// At the begening, the function reads the button state and saves it within the variable buttonState
// t
// e.g. if ButtonState is HIGH, the if condition if(buttonState == HIGH) will be true
// Hence, the program will initialize the postData variable with the JSON string which will turns ON or OFF the Smart Wifi Plug
// Depending onOff variable which is a boolean.
// if(onOff == true) then onOff = false, then onOff = true; This condition will swap between ON and OFF
// then each time the user will press on the button, the smart plug will turn on, then off, then on, etc.
// The JSON object, within the string contain this :
// "togglex":
// {
//   "channel":0,
//   "onoff":1
// }
// The value onoff define the state of the Smart Wifi Plug
// Here, the value is "1" because the variable buttonStateOn is HIGH
// After this, the program will call the function requestFinal()
// Once the if condition is finish, the following code is executed
//    while(true)
//    {
//      buttonStateOn = digitalRead(1);
//      if(buttonStateOn == LOW)
//        break;
//    }
// It will check if the button state is still HIGH which means that the button is still held.
// Once the button is unpressed, the if condition if(buttonStateOn == LOW) will be true the programm will go out of the while(true) loop


// ==================================================================================================================================
// ============================================== PART 4 : the function requestFinal() ==============================================
// ==================================================================================================================================
// Firsty the function will close all connection between the MKR1010 and the Meross Smart Wifi Plug myClient.stop();
// The function will initialize a counter int cpt=0 for the HTTP response send back from the Smart Wifi Plug
// If the counter exceed a value, it will mean that the response hasn't be recepted by the MKR1010.
//
// The function will try to connect to the Meross server of the Smart Wifi Plug if(myClient.connect(serverMSS210, portMSS210))
// If the connection can't be established, Serial.println("Connection failed");
//
// If the connection is successfull, the function will notify the user with a message and will send the HTTP request
// The following HTTP request is send under a character string form.
//    myClient.print
//    (
//      String("POST ") + "/config" + " HTTP/1.1\r\n" +
//      "Content-Type: application/json\r\n" +
//      "Content-Length: " + postData.length() + "\r\n" +
//      "\r\n" +
//      postData
//    );
// This is the minimal code you have to send withing your request
// the body is the code just before the line "\r\n" + (Carriage Return and Line Feed)
// which corresponds to the end of the request body
// just after you have to send the JSON Object stringified
// Once it's done, the Smart Plug Will be turned ON or OFF and a response will be send back to the MKR1010
//
// The response can be captured with the following code :
//    while(myClient.available()==0)//wait here until data reception
//    {
//      cpt++;
//      delay(100);
//      if (cpt>49)
//      {
//        Serial.print("délai dépassé... aucune réponse reçu...");
//        break; // cpt > 50 we leave the while loop
//      }
//    }
//    while(myClient.available()) // write all incoming data characters on serial monitor
//    {
//      codeResponse = myClient.read();
//      Serial.write(codeResponse);
//    } // loop until myClient.available() == 0
//

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
#define SECRET_SSID "XXX" // to be replaced by your router SSID
#define SECRET_PASS "XXX"

WiFiClient myClient;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;


// PIN
#define BUTTON_PIN 0                // Button pin number
#define BUTTON_POWER_PIN 6          // Button power pin number => ensure that "BUTTON_PIN" receive 3.3v instead of 5v that it's not recommended


// COMMUNICATION
int portMSS210 = 80;                  // comon port for HTTP communication
char serverMSS210[] = "192.168.1.33"; // or IPAddress serverMSS210(192,168,1,183);
// Took from sniffed frames : (declared as String to simplify JSON integration)
String MEROSS_APP_TOKEN = "551099-ef1d9fe37442284be4a06684de36c43d";
String MEROSS_MSG_ID = "bdf5a8a37e18f8261ce7623687efcc21";
String MEROSS_SIGN = "09ee94a666bf3f322ab3240bec0a6bc0";


// GLOBAL VARIABLES
int compteur; //check the connection
int buttonState = LOW;
bool onOff = false;



void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(BUTTON_PIN,INPUT);
  pinMode(BUTTON_POWER_PIN,OUTPUT);

  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
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

  // Once every thing is done, power up the button
  digitalWrite( BUTTON_POWER_PIN, HIGH );
}

void loop()
{
  buttonState = digitalRead(BUTTON_PIN); // the electrical state of the pin '0' is read.

  if(buttonState == HIGH) // == true
  {
    Serial.print("Trying to send : ");
    String postData = "{\"header\":{\"from\":\"/app/"+ MEROSS_APP_TOKEN +"/subscribe\",\"messageId\":\""+ MEROSS_MSG_ID +"\",\"method\":\"SET\",\"namespace\":\"Appliance.Control.ToggleX\",\"payloadVersion\":1,\"sign\":\""+ MEROSS_SIGN +"\",\"timestamp\":1583487856},\"payload\":{\"togglex\":{\"channel\":0,\"onoff\":";

     // this IF condition will swap between ON and OFF
     if(onOff == false)
     {
       onOff = true;
       Serial.println("POWER ON");
       postData += "1";
     }
     else
     {
        onOff = false;
        Serial.println("POWER OFF");
        postData += "0";
     }
     postData += "}}}";

     requestFINAL(postData);

     while(true) //wait if you hold the button...
     {
       buttonState = digitalRead(BUTTON_PIN);
       if(buttonState == LOW) // if we don't push the button anymore buttonState will be equal to LOW and the while condition will end
         break;
     }
  }//end of the if condition

  delay(100);
  compteur++;
  if(compteur > 99) // check the connection
  {
    printCurrentNet();
    compteur =0;
  }
}

void requestFINAL( String jsonData ) // using WiFiNINA only
{
  myClient.stop();
  int cpt=0;

  Serial.print( "Connexion sur ");
  Serial.print( serverMSS210 );
  Serial.print( " et port ");
  Serial.println( portMSS210 );

  if (myClient.connect(serverMSS210, portMSS210))
  {
    Serial.print("Connected, sending json : ");
    Serial.println( jsonData );

    myClient.print // any spaces are important
    (
      String("POST ") + "/config" + " HTTP/1.1\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: " + jsonData.length() + "\r\n" + //this line is needed with the exact size value
      "\r\n" + // CR+LF pour signifier la fin du header
      jsonData
    );

    while(myClient.available()==0)//wait until data reception
    {
      cpt++;
      delay(100);
      if (cpt>49)
      {
        Serial.print("Response Time out ...");
        break;
      }
    }

    char codeResponse;
    while(myClient.available()) //write all incoming data characters on serial monitor
    {
      codeResponse = myClient.read();
      Serial.write(codeResponse);
    }
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
  }
}

void printWifiData()
{
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
  delay(1000);
}

void printCurrentNet()
{
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

void printMacAddress(byte mac[])
{
  for (int i = 5; i >= 0; i--)
  {
    if (mac[i] < 16)
    {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0)
    {
      Serial.print(":");
    }
  }
  Serial.println();
}
