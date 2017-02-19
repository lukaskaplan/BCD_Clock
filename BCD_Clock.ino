/*--------- Libraries ---------*/
#include <Wire.h>         //for I2C communicaton
#include "ESP8266WiFi.h"  //for Wifi communication

/*--------- Constants ---------*/
#define MEM_I2C_ADDR 0x57           //I2C address of AT24C32 (eeprom 32Kib) 4096 x 8 bits, addresses 0 - 4095
#define RTC_I2C_ADDR 0x68           //I2C address of DS3231 (Real Time Clock) 

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
    Wire.begin();
    Serial.begin(9600);
    Serial.println(F("<Arduino is ready>"));
    Serial.print(prompt);
}

void loop() {        
    if(Serial.available() > 0) {
        delay(100); //because of backspace, if there is no delay, whole row is deleted by one push of backspace key...
        rc = Serial.read();
        
        if ( rc == enter ) {
            Serial.println();
            // Zpracovani zadaneho prikazu
            
            if(strlen(receivedChars) > 0) {
                receivedChars[ndx] = '\0';
                command = String(receivedChars); //convert to String
                if( command == "show mac" ) showMac();
                else if( command == "show time" ) showTime();
                else if( command == "show wlans" ) showWlans();
                else if( command.substring(0,14) == "set wifi ssid " ) {      
                    String ssid = command.substring(14);
                    i2c_eeprom_write_bulk( MEM_I2C_ADDR, 24, 33, ssid);
                    
                    /* for DEBUGING
                    Serial.println();
                    Serial.println("Vypis pameti:");
                    for(unsigned int x = 24; x < 55; x++){
                        Serial.print(i2c_eeprom_read_byte( MEM_I2C_ADDR, x ), DEC);
                        Serial.print(' ');
                    }
                    Serial.println();
                    */
                }
                else if( command.substring(0,18) == "set wifi password " ) {      
                    String password = command.substring(18);
                    i2c_eeprom_write_bulk( MEM_I2C_ADDR, 57, 64, password);
                    
                    /* for DEBUGING*/
                    Serial.println();
                    Serial.println("Vypis pameti:");
                    for(unsigned int x = 57; x < 122; x++){
                        Serial.print(i2c_eeprom_read_byte( MEM_I2C_ADDR, x ), DEC);
                        Serial.print(' ');
                    }
                    Serial.println();
                    
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
