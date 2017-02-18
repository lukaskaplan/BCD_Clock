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
    delay(100);
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
          Serial.print("Nastaveni ssid: ");
          String ssid = command.substring(14);
          Serial.println(ssid);
          Serial.println(ssid.length());
          char ssidChars[33]; //ssid max length is 32 byte + terminating zero
          ssid.toCharArray(ssidChars, 33); 
          Serial.println("Zapis do pameti:");
          unsigned int y = 24;
          for(int i = 24; i < 56; i++) i2c_eeprom_write_byte( MEM_I2C_ADDR, i, (byte)'\0' );
          for(int i = 0; i < strlen(ssidChars); i++){
            Serial.print(ssidChars[i],DEC);
             //Serial.print("-");
              y = 24 + i;
              //Serial.print(y);
              Serial.print(' ');
            i2c_eeprom_write_byte( MEM_I2C_ADDR, y, (byte)ssidChars[i] );
          }
          Serial.println();
          Serial.println("Vypis pameti:");
          for(unsigned int x = 24; x < 55; x++){
            Serial.print(i2c_eeprom_read_byte( MEM_I2C_ADDR, x ), DEC);
            Serial.print(' ');
            //delay(1);
          }
        }
        else {
            Serial.print("Unknown command: ");
            Serial.println(receivedChars);
            Serial.println("Press ? for help...");
        } 
      }
      Serial.print(prompt);
      // clean of recivedChars and command string
      for(int i = 0; i < numChars; i++) receivedChars[i] = '\0';
      command = "";
      ndx = 0;
    } //end of "ENTER" section
    
    else if (rc == help) {
      receivedChars[ndx] = '\0';
      showHelp();
          
      //Serial.print("\r\033[K"); //smazat cely radek
      Serial.print(prompt);
      Serial.print(receivedChars);
    }
    
    else if (rc == bckspc) {
      ndx--;
      receivedChars[ndx] = '\0';
      Serial.print("\r\033[K"); //smazat cely radek
      Serial.print(prompt);
      //for(int i = 0; i < strlen(receivedChars); i++){
        //Serial.print(receivedChars[i]);
      //}
      Serial.print(receivedChars);
      
    }
    
    else {
      receivedChars[ndx] = rc;
      Serial.print(rc);
      if(ndx < numChars) ndx++; //ochrana proti preteceni
    }
  }   

} // end of loop

void setWifi() {
  Serial.println();
  Serial.print("SSID: ");
  
}
