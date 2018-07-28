/****************************************************************************
 * @Project: DUMP ESP8266 NFC Office implementation
 * @File name: main.ino
 * @Brief:
 * @Note
 * @Author: Ivan Pavao Lozancic
 * @Date: 07-28-2018
 ****************************************************************************/
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <FS.h>

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>

#define ssid      "dump"        // WiFi SSID
#define password  "Dump.12345"  // WiFi password

ESP8266WebServer server ( 80 );
WiFiClient client;

PN532_SPI pn532spi(SPI, D2);
PN532 nfc(pn532spi); //Set SPI communication with PN532 board

void setup() {
  
  delay(300); // Init delay
  
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password ); //Connect to wifi
  
  #ifdef TEST_MODE
  while ( WiFi.status() != WL_CONNECTED ) { // Wifi connecting
    delay ( 500 ); Serial.print ( "." );
  }
  
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );
  
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  #endif //TEST_MODE
  
  //Load index.html and listen to HTTP POST request
  server.serveStatic("/", SPIFFS, "/index.html");

  // Start server
  server.begin();
  #ifdef TEST_MODE
  Serial.println ( "HTTP server started" );
  Serial.println ( "String START: " + EEPROM.read(0) );
  #endif //TEST_MODE

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
void loop() {
  
  server.handleClient();
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



