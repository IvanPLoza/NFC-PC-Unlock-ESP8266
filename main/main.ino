#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <FS.h>
#include "configData.h"
#include "debugLog.h"

#define ssid      "dump"      // WiFi SSID
#define password  "Dump.12345"  // WiFi password
           
debugLog *dlog;
configData *cfgdat;
int save;
ESP8266WebServer server ( 80 );
WiFiClient client;

void setup() {
  delay(300); // Init delay
  
  Serial.begin ( 115200 );
  read_db();
  WiFi.begin ( ssid, password );
  while ( WiFi.status() != WL_CONNECTED ) { // Wifi connecting
    delay ( 500 ); Serial.print ( "." );
  }
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );

  if (!SPIFFS.begin())
  {
    // Serious problem
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
  }

  //Load index.html and listen to HTTP POST request
  server.serveStatic("/", SPIFFS, "/index.html");
  server.on("/login", HTTP_POST, handleLogin);

  // Start server
  server.begin();
  Serial.println ( "HTTP server started" );
  Serial.println ( "String START: " + EEPROM.read(0) );

}
void read_db(){
    cfgdat = new configData();
    String errMsg;
    if(!cfgdat->getError(errMsg)) {
        // no errors!
        
        // set up the logger, typically sent out via Serial
        // but can be modified as needed.
        dlog = new debugLog(cfgdat->getUID());
    
        // just to prove we read something from the configuration file
        dlog->println();
        dlog->println(cfgdat->getEMAILString());
        dlog->println(cfgdat->getPASSString());
        dlog->println(cfgdat->getUID());

        WiFi.mode(WIFI_STA);
        // Use the functions that return `const char *` because
        // WiFi.begin() doesn't take String as an arg.
        Serial.println(cfgdat->getEMAIL());
        Serial.println(cfgdat->getPASS());
    } else {
        // an error has occurred, the message will provide
        // additional information.
        dlog->println(errMsg);
    }
}
void loop() {
  //Client search and handler loop
  server.handleClient();
}

void handleLogin(){
  if( ! server.hasArg("email") || ! server.hasArg("password") 
      || server.arg("email") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  if(server.arg("email") == "John Doe") // Email matching 
  {
    server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
    EEPROM.write(0, 2);
    Serial.println("USPIA SI: " + save);
  } 
  else // Email not found
  {
    server.send(401, "text/plain", "404 Email nije pronaden. :/");
    Serial.println("nisi");
  }
}



