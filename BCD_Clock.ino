/*--------- Libraries ---------*/
#include <Wire.h>         //for I2C communicaton
#include "ESP8266WiFi.h"  //for Wifi communication

/*---------- Weemos PIN mapping ----------*/
#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "../generic/common.h"

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

static const uint8_t LED_BUILTIN = 2;
static const uint8_t BUILTIN_LED = 2;

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;

#endif /* Pins_Arduino_h */

/*--------- Constants ---------*/
#define MEM_I2C_ADDR 0x57           //I2C address of AT24C32 (eeprom 32Kib) 4096 x 8 bits, addresses 0 - 4095
#define RTC_I2C_ADDR 0x68           //I2C address of DS3231 (Real Time Clock) 
const int sclPin = D3;              //SCL is normaly on pin D1, but I needed D1 for charlieplexing (because pins with internal pullup or pulldown resistors arent usable for charlieplex.)
const int sdaPin = D2;
const byte numChars = 96;           //max.length of the received data
const char enter = '\r';            //char representing "enter" key in CLI
const char help = '?';              //char representing "?" key in CLI
const char bckspc = 0x08;           //char representing "backspace" key in CLI
const char prompt[] = "Prompt# ";   //CLI prompt


char receivedChars[numChars]; //an array to store the received data
byte ndx = 0;
char rc;
String command = "";

void setup() {
    Wire.begin(sdaPin, sclPin);
    Serial.begin(9600);
    Serial.println(F("<Arduino is ready>"));
    Serial.print(prompt);
}

void loop() {        
    if(Serial.available() > 0) {
       // delay(100); //because of backspace, if there is no delay, whole row is deleted by one push of backspace key...
        rc = Serial.read();
        
        if ( rc == enter ) {
            Serial.println();
            // Zpracovani zadaneho prikazu
            
            if(strlen(receivedChars) > 0) {
                receivedChars[ndx] = '\0';
                command = String(receivedChars); //convert to String
                if( command == "show config" ) showConfig();
                else if( command == "show mac" ) showMac();
                else if( command == "show time" ) showTime();
                else if( command == "show wlans" ) showWlans();
                else if( command == "show memory hexdump" ) showMEMHexdump();
                else if( command == "show rtc hexdump" ) showRTCHexdump();
                else if( command.substring(0,14) == "set wifi ssid " ) {      
                    String ssid = command.substring(14);  //ssid = char 14 to end
                    i2c_eeprom_write_bulk( MEM_I2C_ADDR, 24, 33, ssid);
                }
                else if( command.substring(0,18) == "set wifi password " ) {      
                    String password = command.substring(18);
                    i2c_eeprom_write_bulk( MEM_I2C_ADDR, 57, 64, password);
                }
                else if( command.substring(0,15) == "set ip address " ) {      
                    String ip = command.substring(15);
                    StringToIP(ip);
                    //i2c_eeprom_write_bulk( MEM_I2C_ADDR, 57, 64, password);
                }
                else {
                    Serial.print("Unknown command: ");
                    Serial.println(receivedChars);
                    Serial.println("Press ? for help...");
                } 
            }
            
            Serial.print(prompt);
            for(int i = 0; i < numChars; i++) receivedChars[i] = '\0';      // clean of recivedChars and command string
            command = "";
            ndx = 0;
        } //end of "ENTER" section
    
        else if (rc == help) {
            receivedChars[ndx] = '\0';
            showHelp();
            Serial.print(prompt);
            Serial.print(receivedChars);
        } //end of help section
    
        else if (rc == bckspc) {
            ndx--;
            receivedChars[ndx] = '\0';
            Serial.print("\r\033[K"); //delete entire row
            Serial.print(prompt);
            Serial.print(receivedChars);
        } //end of backspace section
    
        else {
            receivedChars[ndx] = rc;
            Serial.print(rc);
            if(ndx < numChars) ndx++; //buffer overflow protection
        }
    
  } //end of "if(Serial.available() > 0)..."

} // end of loop
