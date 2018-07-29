/****************************************************************************
 *  @project:     DUMP ESP8266 NFC Office implementation
 *  @file_name:   main.ino
 *  @brief:
 *  @note:
 *  @author:      Ivan Pavao Lozancic @dump
 *  @date:        07-28-2018
 ****************************************************************************/
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <FS.h>

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>

#include<string.h>

#define ssid      "dump"        // WiFi SSID
#define password  "Dump.12345"  // WiFi password

/****************************************************************************
 *                            Public functions
 ***************************************************************************/

/****************************************************************************
 *  @name:        SPIFFS_check
 *  *************************************************************************
 *  @brief:       Check if SPIFFS memory mount is succesfull
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      [true]:  SPIFFS mount was succesfull
 *                [flase]: SPIFFS mount failed 
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
bool SPIFFS_check(){
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount failed");
    return true;
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
    return false;
  }
}

/****************************************************************************
 *  @name:        startESPServer
 *  *************************************************************************
 *  @brief:       Configure and start ESP8266 as web server
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
void startESPServer(){
  
  ESP8266WebServer server ( 80 );

  WiFiClient client;

  server.serveStatic("/", SPIFFS, "/index.html");
}

/****************************************************************************
 *  @name:        WifiConnect
 *  *************************************************************************
 *  @brief:       Connect ESP8266 to WiFi network
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      [true]:  WiFi connection is established
 *                [flase]: no connection
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
void WifiConnect(String ssid, String pass){

  uint8_t connectTimeOverflow = 0;

  WiFi.begin ( ssid, password ); //Connect to wifi

  //Check if device is connected
  while ( WiFi.status() != WL_CONNECTED  && connectTimeOverflow < 20) {
    delay ( 500 );

    #ifdef TEST_MODE
    Serial.print ( "." );
    #endif

    connectTimeOverflow++;
  }

  #ifdef TEST_MODE
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  #endif //TEST_MODE

  return true;
}

/****************************************************************************
 *  @name:        PN532_connect
 *  *************************************************************************
 *  @brief:       Connect ESP8266 to PN532 board
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
void PN532_connect(){
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  
  if (! versiondata) {
    
    #ifdef TEST_MODE
    Serial.print("Didn't find PN53x board");
    #endif //TEST_MODE
    
    while (1); // halt
  }

  #ifdef TEST_MODE
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  #endif //TEST_MODE

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  //nfc.setPassiveActivationRetries(0xFF);
  nfc.setPassiveActivationRetries(10);

  // configure board to read RFID tags
  nfc.SAMConfig();

  #ifdef TEST_MODE
  Serial.println("Waiting for an ISO14443A card");
  Serial.println("\n-----------\n");
  #endif //TEST_MODE
}

/****************************************************************************
 *  @name:        readCard
 *  *************************************************************************
 *  @brief:       Reads ISO14443A card and returns UID
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      [pointer] array of UID values
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
uint8_t * readCard(){

  boolean success;

  uint8_t uidLength;   // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {
    #ifdef TEST_MODE
    Serial.println("Found a card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++)
    {
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");
    #endif //TEST_MODE
  }
    // wait until the card is taken away
   while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength)) yield(); //let ESPcore handle wifi stuff
}

/****************************************************************************
 *  @name:        matchUser
 *  *************************************************************************
 *  @brief:       Match read card with saved cards
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      [true] If card is match
 *                [false] If card is not match
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
bool matchUser(){

}

/****************************************************************************
 *  @name:        Unlock_PCUser
 *  *************************************************************************
 *  @brief:       Unlock PC user
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
void Unlock_PCUser(){

}

/****************************************************************************
 *                            Setup function
 ***************************************************************************/
void setup() {
  
  Serial.begin ( 115200 );  //Begin serial communication

  WifiConnect(ssid, password);  //Connect to WIFI
  startESPServer();        //Start server
  PN532_connect();              //Connect to PN532 board

}

/****************************************************************************
 *                            Main function
 ***************************************************************************/
void loop() {
  server.handleClient();
  readCrad();
}

/****************************************************************************
 *                            End of the file
 ***************************************************************************/


