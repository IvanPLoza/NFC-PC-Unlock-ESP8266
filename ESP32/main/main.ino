/****************************************************************************
 *  @project:     DUMP ESP32 NFC Office implementation
 *  @file_name:   main.ino
 *  @brief:
 *  @note:        PINOUT SCHEME
 *                SS    ->  GPIO4
 *                IRQ   ->  none
 *                RST   ->  none
 *                SCK   ->  GPIO18
 *                MISO  ->  GPIO19
 *                MOSI  ->  GPIO23
 *                ************* 
 *           
 *  @author:      Ivan Pavao Lozancic @dump
 *  @date:        07-28-2018
 ****************************************************************************/
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <FS.h>
#include "SPIFFS.h"
#include <WiFiClient.h>
#include <WebServer.h>

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>

//#include <PS2Keyboard.h>

#include<string.h>

/****************************************************************************
 *                            Public definitions
 ***************************************************************************/

//Program mode defines
#define PN532_CONNECTED //Enable if PN532 NFC Reader is used
#define TEST_MODE       //Enable if informations through serial communication is needed
#define DEBUG
#define TRINKET         //Enable if HID keyboard device is needed
#define SERIAL_START    //Enable if serial communication is needed
#define WEB_SERVER      //Enable if web server is needed
#define WIFI_CONNECT    //Enable if Wi-Fi connection is needed

//PN532 SPI CONFIGURATION
#ifdef PN532_CONNECTED
PN532_SPI pn532spi(SPI, 4);
PN532 nfc(pn532spi);
#endif //PN532_CONNECTED

//ESP32 WEB SERVER CONFIGURATION
#ifdef WEB_SERVER
WebServer server (80);
#endif//WEB_SERVER

//Choose Wi-Fi
#define DUMP 

#ifdef DUMP
#define SSID      "dump"
#define PASSWORD  "Dump.12345"
#endif //DUMP

#ifdef HOME
#define SSID      "Jonelo2"
#define PASSWORD  "172030ZN"
#endif //HOME

//Signal pin GPIO define 
#define SIGNAL_PIN 0x11

//User UIDs definitions
#define USERS_NUM 72

//USERS
uint8_t usersUID[72][8] = {

//| #1_UID | #2_UID | #3_UID | #4_UID | #5_UID | #6_UID | #7_UID | #8_UIDLENGTH

  {0xF4, 0x6D, 0x23, 0xD9, 0, 0, 0, 4},           //  Andrea Sikimic        0

  {0x11, 0x94, 0x19, 0x2E, 0, 0, 0, 4},           //  Ani Stjepic           1   Card
  {0x54, 0x47, 0x24, 0xD9, 0, 0, 0, 4},           //  Ani Stjepic           2   Keychain

  {0xB, 0x58, 0x23, 0xD9, 0, 0, 0, 4},            //  Boris Borovic         3

  {0x12, 0x55, 0x23, 0xD9, 0, 0, 0, 4},           //  Izabela Domazet       4

  {0xE1, 0x19, 0x24, 0xD9, 0, 0, 0, 4},           //  Josipa Braica         5

  {0xE1, 0x4, 0xF0, 0x2D, 0, 0, 0, 4},            //  Mario Ceprnja         6   Card 1
  {0xE1, 0x52, 0xCF, 0x2D, 0, 0, 0, 4},           //  Mario Ceprnja         7   Card 2
  {0xE1, 0x2B, 0xBC, 0x2D, 0, 0, 0, 4},           //  Mario Ceprnja         8   Card 3
  {0x3, 0x94, 0x10, 0x21, 0, 0, 0, 4},            //  Mario Ceprnja         9   Card 4
  {0x1, 0x91, 0x35, 0x2E, 0, 0, 0, 4},            //  Mario Ceprnja         10  Card 5
  {0x11, 0x8F, 0x77, 0x2E, 0, 0, 0, 4},           //  Mario Ceprnja         11  Card 6
  {0x6, 0x5A, 0x23, 0xD9, 0, 0, 0, 4},            //  Mario Ceprnja         12  Keychain 1
  {0xAD, 0x28, 0x24, 0xD9, 0, 0, 0, 4},           //  Mario Ceprnja         13  Keychain 2
  {0xE2, 0x48, 0x23, 0xD9, 0, 0, 0, 4},           //  Mario Ceprnja         14  Keychain 3
  {0x7D, 0x54, 0x23, 0xD9, 0, 0, 0, 4},           //  Mario Ceprnja         15  Keychain 4
  {0xF7, 0xF7, 0x22, 0xD9, 0, 0, 0, 4},           //  Mario Ceprnja         16  Keychain 5
  {0xBE, 0x44, 0x23, 0xD9, 0, 0, 0, 4},           //  Mario Ceprnja         17  Keychain 6
  {0x4, 0xF1, 0x88, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         18  Sticker 1
  {0x4, 0xE9, 0x88, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         19  Sticker 2
  {0x4, 0xE1, 0x88, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         20  Sticker 3
  {0x4, 0xC4, 0x86, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         21  Sticker 4
  {0x4, 0xD0, 0x85, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         22  Sticker 5
  {0x4, 0xE0, 0x86, 0x82, 0x31, 0x4D, 0x80, 7},   //  Mario Ceprnja         23  Sticker 6

  {0x81, 0x83, 0x23, 0xD9, 0, 0, 0, 4},           //  Mate Rajcic           24

  {0x72, 0xB9, 0x23, 0xD9, 0, 0, 0, 4},           //  Gabrijel Boduljak     25

  {0x77, 0x41, 0x23, 0xD9, 0, 0, 0, 4},           //  Nikola Dadic          26

  {0x26, 0xE6, 0x23, 0xD9, 0, 0, 0, 4},           //  Duje Dakovic          27
  
  {0x36, 0xF1, 0x23, 0xD9, 0, 0, 0, 4},           //  Roko Radanovic        28

  {0xE2, 0x1A, 0x24, 0xD9, 0, 0, 0, 4},           //  Klara Dapic           29

  {0x6D, 0x50, 0x23, 0xD9, 0, 0, 0, 4},           //  Stipe Lelas           30

  {0x9E, 0xB1, 0x6E, 0x20, 0, 0, 0, 4},           //  Josip Svalina         31  Card 1
  {0x8D, 0xC8, 0xB5, 0x2D, 0, 0, 0, 4},           //  Josip Svalina         32  Card 2
  {0x8D, 0xD1, 0xD5, 0x2D, 0, 0, 0, 4},           //  Josip Svalina         33  Card 3
  {0x8D, 0xB3, 0x8C, 0x2D, 0, 0, 0, 4},           //  Josip Svalina         34  Card 4
  {0x8D, 0xD6, 0xE8, 0x2D, 0, 0, 0, 4},           //  Josip Svalina         35  Card 5
  {0x64, 0x53, 0x24, 0xD9, 0, 0, 0, 4},           //  Josip Svalina         36  Keychain 1
  {0x17, 0x12, 0x24, 0xD9, 0, 0, 0, 4},           //  Josip Svalina         37  Keychain 2
  {0x7B, 0xBB, 0x23, 0xD9, 0, 0, 0, 4},           //  Josip Svalina         38  Keychain 3
  {0x43, 0x81, 0x23, 0xD9, 0, 0, 0, 4},           //  Josip Svalina         39  Keychain 4
  {0x16, 0xBB, 0x23, 0xD9, 0, 0, 0, 4},           //  Josip Svalina         40  Keychain 5
  {0x4, 0xE8, 0x86, 0x82, 0x31, 0x4D, 0x80, 7},   //  Josip Svalina         41  Sticker 1
  {0x4, 0xFC, 0x84, 0x82, 0x31, 0x4D, 0x80, 7},   //  Josip Svalina         42  Sticker 2
  {0x4, 0x5, 0x85, 0x82, 0x31, 0x4D, 0x81, 7},    //  Josip Svalina         43  Sticker 3
  {0x4, 0xD, 0x85, 0x82, 0x3, 0x4D, 0x81, 7},     //  Josip Svalina         44  Sticker 4
  {0x4, 0x15, 0x85, 0x82, 0x31, 0x4D, 0x81, 7},   //  Josip Svalina         45  Sticker 5

  {0x9D, 0x47, 0xAA, 0x2D, 0, 0, 0, 4},           //  Drazen Baric          46  Card

  {0x11, 0x34, 0x36, 0x2E, 0, 0, 0, 4},           //  Noa Baric             47   Card
  {0xA2, 0x6B, 0x23, 0xD9, 0, 0, 0, 4},           //  Noa Baric             48   Keychain 1
  {0x63, 0xC6, 0x23, 0xD9, 0, 0, 0, 4},           //  Noa Baric             49   Keychain 2
  {0x4, 0xDA, 0x87, 0x82, 0x31, 0x4D, 0x80, 7},   //  Noa Baric             50   Sticker

  {0x11, 0x8E, 0x95, 0x2E, 0, 0, 0, 4},           //  Kresimir Condic       51

  {0x8D, 0xC0, 0x5A, 0x2D, 0, 0, 0, 4},           //  Zvonimir Delas        52  Card
  {0xE0, 0x7F, 0x23, 0xD9, 0, 0, 0, 4},           //  Zvonimir Delas        53  Keychain 1
  {0x4C, 0xC0, 0x23, 0xD9, 0, 0, 0, 4},           //  Zvonimir Delas        54  Keychain 2
  {0x4, 0xC8, 0x87, 0x82, 0x31, 0x4D, 0x80, 7},   //  Zvonimir Delas        55  Sticker

  {0xC9, 0x51, 0x24, 0xD9, 0, 0, 0, 4},           //  Luka Ivanovic         56

  {0x1, 0xCD, 0x91, 0x2E, 0, 0, 0, 4},            //  Karlo Koscal          57  Card
  {0xD2, 0x51, 0x23, 0xD9, 0, 0, 0, 4},           //  Karlo Koscal          58  Keychain

  {0x9D, 0x2D, 0x7E, 0x2D, 0, 0, 0, 4},           //  Ivan Pavao Lozancic   59  Card
  {0xB4, 0xC9, 0x23, 0xD9, 0, 0, 0, 4},           //  Ivan Pavao Lozancic   60  Keychain
  {0xFD, 0x8A, 0x24, 0xD9, 0, 0, 0, 4},           //  Ivan Pavao Lozancic   61  Keychain
  {0x36, 0x29, 0x24, 0xD9, 0, 0, 0, 4},           //  Ivan Pavao Lozancic   62  Keychain

  {0x9D, 0x18, 0xB2, 0x2D, 0, 0, 0, 4},           //  Toma Puljak           63  Card
  {0x74, 0xC1, 0x23, 0xD9, 0, 0, 0, 4},           //  Toma Puljak           64  Keychain

  {0x21, 0x3A, 0x23, 0xD9, 0, 0, 0, 4},           //  Meri Svragulja        65

  {0x8C, 0xBD, 0x23, 0xD9, 0, 0, 0, 4},            //  Bruno Vego           66  Keychain
  {0x4, 0xFD, 0x86, 0x82, 0x31, 0x4D, 0x80, 7},    //  Bruno Vego           67  Sticker 1
  {0x4, 0x6, 0x87, 0x82, 0x31, 0x4D, 0x81, 7},     //  Bruno Vego           68  Sticker 2
  {0x4, 0xE, 0x87, 0x82, 0x31, 0x4D, 0x81, 7},     //  Bruno Vego           69  Sticker 3
  {0x4, 0x16, 0x87, 0x82, 0x31, 0x4D, 0x81, 7},    //  Bruno Vego           70  Sticker 4
  {0x4, 0xF0, 0x86, 0x82, 0x31, 0x4D, 0x80, 7}     //  Bruno Vego           71  Sticker 5
  
  //TODO @ivan.pavao.lozancic Add Bruno Radan cards
};

//Read card UID
uint8_t uid[8] = { 0, 0, 0, 0, 0, 0, 0, 0};

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
#ifdef WEB_SERVER
bool SPIFFS_check(){
  if (!SPIFFS.begin())
  {
    #ifdef TEST_MODE
    Serial.println("SPIFFS Mount failed");
    #endif//TESTMODE
    return true;
  } 
  else {
    #ifdef TEST_MODE
    Serial.println("SPIFFS Mount succesfull");
    #endif//TESTMODE
    return false;
  }
}
#endif//WEB_SERVER

/****************************************************************************
 *  @name:        startESPServer
 *  *************************************************************************
 *  @brief:       Load index.html from ESP32 flash memory and host it
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
#ifdef WEB_SERVER
void startESPServer(){
  server.serveStatic("/", SPIFFS, "/index.html");
}
#endif//WEB_SERVER

/****************************************************************************
 *  @name:        WifiConnect
 *  *************************************************************************
 *  @brief:       Connect ESP32 to WiFi network
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
#ifdef WIFI_CONNECT
void WifiConnect(char ssid[], char pass[]){

  #ifdef TEST_MODE
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  #endif//TESTMODE

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {

    #ifdef TEST_MODE
    Serial.println("Connection Failed! Rebooting...");
    #endif//TEST_MODE

    //Wait 2 seconds before restarting the device
    delay(2000);

    ESP.restart();
  }

  #ifdef TEST_MODE
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  #endif //TEST_MODE

}
#endif//WifiConnect

/****************************************************************************
 *  @name:        PN532_connect
 *  *************************************************************************
 *  @brief:       Connect ESP32 to PN532 board
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        30-07-2018
 ***************************************************************************/
#ifdef PN532_CONNECTED
void PN532_connect(){
  
  //Start configured SPI communication with PN53x board
  nfc.begin();

  //Get PN53x board version
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
  nfc.setPassiveActivationRetries(10);

  // Configure board to read RFID tags
  nfc.SAMConfig();

  #ifdef TEST_MODE
  Serial.println("Waiting for an ISO14443A card");
  Serial.println("\n-----------\n");
  #endif //TEST_MODE
}
#endif //PN532_CONNECTED

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
#ifdef PN532_CONNECTED
bool readCard(){

  //Return value of NFC read function
  bool success;
  
  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t uidLength;

  // Wait for an ISO14443A type cards
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {

    //Store read UID length to global variable
    uid[7] = uidLength;

    #ifdef TEST_MODE
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("Found a card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++){
      Serial.print(" 0x");
      Serial.print(uid[i], HEX);
    }
    Serial.println("");
    Serial.println("");
    Serial.println("");
    #endif //TEST_MODE
    return true;
  }

  //PN532 didn't find card
   return false;
}
#endif //PN532_CONNECTED

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
 *  @date:        20-09-2018
 ***************************************************************************/
#ifdef PN532_CONNECTED
bool matchUser(uint8_t readUID[]){

  uint8_t COUNTER_USERS;
  uint8_t COUNTER_UID;
  uint8_t readUID_LENGTH = readUID[7];
  uint8_t matchFactor;
  uint8_t matchFactor_MAXVAL = readUID_LENGTH;

  #ifdef TEST_MODE
  Serial.println("");
  Serial.println("LENGTH: ");
  Serial.print(readUID_LENGTH);
  Serial.println("");
  #endif //TEST_MODE

  for(COUNTER_USERS = 0; COUNTER_USERS < USERS_NUM; COUNTER_USERS++){
    if(usersUID[COUNTER_USERS][7] == readUID_LENGTH){

      matchFactor = 0; //Set matchFactor to zero for new user for check

      for(COUNTER_UID = 0; COUNTER_UID < readUID_LENGTH; COUNTER_UID++){
        if(usersUID[COUNTER_USERS][COUNTER_UID] == readUID[COUNTER_UID]){
          
          #ifdef TEST_MODE
          Serial.println("");
          Serial.print("USER: ");
          Serial.println(usersUID[COUNTER_USERS][COUNTER_UID]);
          Serial.print("READ: ");
          Serial.println(readUID[COUNTER_UID]);
          Serial.print("USER ID: ");
          Serial.println(COUNTER_USERS);
          #endif //TEST_MODE

          //Byte is match!
          matchFactor++;

        }//if(usersUID - byte pair check)
      }//for(COUNTER_UID)

      //If the read card is match with user card
      if(matchFactor == matchFactor_MAXVAL){
        
        #ifdef TEST_MODE
        Serial.println("SUCCESS");
        Serial.println("=====================");
        Serial.println("");
        Serial.println("");
        Serial.println("");
        #endif //TEST_MODE

        return true;

      }//if(matchFactor - check if match)
    }//if(usersUID - length check)
  }//for(COUNTER_USERS)

  //NO MATCH!
  return false;
}
#endif//PN532_CONNECTED

/****************************************************************************
 *  @name:        signalTrinketBoard
 *  *************************************************************************
 *  @brief:       Sends digital signal to trinket input pin
 *  @note:
 *  *************************************************************************
 *  @param[in]:
 *  @param[out]:   
 *  @return:      nothing
 *  *************************************************************************
 *  @author:      Ivan Pavao Lozancic
 *  @date:        21-08-2018
 ***************************************************************************/
#ifdef TRINKET
void signalTrinketBoard(){

  //Send digital signal to trinket board
  digitalWrite(SIGNAL_PIN, LOW);
  delay(500);

  //Stop the signal
  digitalWrite(SIGNAL_PIN, HIGH);

  //Wait for PC to be unlocked
  delay(3000);
}
#endif//TRINKET

/****************************************************************************
 *                            Setup function
 ***************************************************************************/
void setup() {

  #ifdef DEBUG
  Serial.begin ( 115200 );  //Begin serial communication
  #endif //DEBUG

  pinMode(SIGNAL_PIN, OUTPUT);      //Set signal pin
  digitalWrite(SIGNAL_PIN, HIGH);   //Important for Trinket Board keyboard 

  //Wifi
  #ifdef WIFI_CONNECT
  WifiConnect(SSID, PASSWORD);        //Connect to WIFI
  #endif//WIFI_CONNECT

  //Connect to PN532 board
  #ifdef PN532_CONNECTED
  PN532_connect();
  #endif    

  
}

/****************************************************************************
 *                            Main function
 ***************************************************************************/
void loop() {

  if(readCard() == true){
    if(matchUser(uid) == true){
      signalTrinketBoard();
    }
  }

}

/****************************************************************************
 *                            End of the file
 ***************************************************************************/
