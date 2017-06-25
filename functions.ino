/*------------------------------------------*
 *-------------- I2C Functions -------------*
 *------------------------------------------*/
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  /*
  * Writes one byte to i2c eeprom memory
  *
  * deviceaddress = i2c address of eeprom memory
  * eeaddress = dec address of byte in memory
  * data = byte to write
  */
    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    //Wire.write((int)(eeaddress)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(rdata);
    Wire.endTransmission();
}

void i2c_eeprom_write_bulk(int i2caddr, unsigned int firstaddr, unsigned int bytes, String data){
 /*
  * Writes given bytes into i2c eeprom at once
  * 
  * i2caddr - address of i2c device (0-127)
  * firsaddr - address (dec) of first byte to write
  * bytes - bytes count 
  * data  -String to be written in memory
  *       -if it is longer than assigned memory range, it will be trimmed
  *       -if it is shorter than assigned memory range, free bytes will be filled by zeroes (\0)
  */
    char chars[bytes];
    data.toCharArray(chars, bytes);     //convert string to char array (last byte/char will be always 0)
    unsigned int lastaddr = firstaddr + bytes - 1;      
    unsigned int chi;      //chars index 
    
    for(int eeaddr = firstaddr; eeaddr <= lastaddr; eeaddr++) {
        chi = eeaddr - firstaddr;
        if( chi < strlen(chars) ) i2c_eeprom_write_byte( i2caddr, eeaddr, (byte)chars[chi] );
        else i2c_eeprom_write_byte( i2caddr, eeaddr, (byte)'\0' );
        delay(5);     // without this, there is problem with "bulk" write to memory (missing values etc.)
    }
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  /*
  * Reads one byte from i2c eeprom
  *
  * deviceaddress = i2c addres of memory
  * eeaddress = address of byte in memory
  * return rdata = return value of byte read from memory
  */
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8)); // MSB
    //Wire.write((int)(eeaddress)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
}
  
/*------------------------------------------*
 *--------------- CLI Fuctions -------------*
 *------------------------------------------*/
void showHelp() {
  //print cli help (possible commands with explanation)
  Serial.println();
  Serial.println("set wifi ssid <ssid>");
  Serial.println("set wifi password <password>");
  Serial.println("show config");
  Serial.println("show mac\t\tShow wireless MAC address");
  //Serial.println("show log");
  Serial.println("show time");
  Serial.println("show wlans\t\tScan and show wireless networks");
  Serial.println("show memory hexdump");
  Serial.println("show rtc hexdump");
  
  
}

void showMac() {
  // the MAC address of your Wifi shield
  byte mac[6];

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
  }
}

void showWlans() {
  Serial.println();
  Serial.print("Scanning, it can take few seconds, please wait...");
  int numSsid = WiFi.scanNetworks();      // scan for nearby networks
  Serial.print("\r\033[K");               //clear line
  
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    return;
  }

  // print the list of networks seen:
  Serial.print("Number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\t\t");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void readRTCtime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year){
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(0); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom(RTC_I2C_ADDR, 7);
    // request seven bytes of data from DS3231 starting from register 00h
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

void showTime(){
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    // retrieve data from DS3231
    readRTCtime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    // send it to the serial monitor
    Serial.print(hour, DEC);
    // convert the byte variable to a decimal number when displayed
    Serial.print(":");
    if (minute<10){
        Serial.print("0");
    }
    Serial.print(minute, DEC);
    Serial.print(":");
    if (second<10){
        Serial.print("0");
    }
    Serial.print(second, DEC);
    Serial.print(" ");
    Serial.print(dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(month, DEC);
    Serial.print("/");
    Serial.print(year, DEC);
    Serial.print(" Day of week: ");
    switch(dayOfWeek){
        case 1:
            Serial.println("Sunday");
            break;
        case 2:
            Serial.println("Monday");
            break;
        case 3:
            Serial.println("Tuesday");
            break;
        case 4:
            Serial.println("Wednesday");
            break;
        case 5:
            Serial.println("Thursday");
            break;
        case 6:
            Serial.println("Friday");
            break;
        case 7:
            Serial.println("Saturday");
            break;
    }
}

void showConfig(){
  Serial.print("SSID: ");
  for(unsigned int x = 24; x < 57; x++){Serial.print((char)i2c_eeprom_read_byte( MEM_I2C_ADDR, x ));}
  Serial.println();
  
  Serial.print("Password: ");
  for(unsigned int x = 57; x < 121; x++){Serial.print((char)i2c_eeprom_read_byte( MEM_I2C_ADDR, x ));}
  Serial.println();

  showMac();
}

void showMEMHexdump(){
  char text[15];
  Serial.println();
  
  for(int adr = 0; adr < 4096; adr=adr+16){
    Serial.print("0x");
    Serial.print(adr, HEX);
    Serial.print("\t");

    for(int y = 0; y < 16; y++){
      int B = i2c_eeprom_read_byte(  MEM_I2C_ADDR, adr+y );
      //int i = (adr + 16) % 16;
      text[y] = B;
      if(B < 16) Serial.print("0");
      Serial.print(B, HEX);
      Serial.print(" ");
    }
    Serial.print("\t");
    for(int i = 0; i < 16; i++){
      Serial.print(text[i]);
    }
    Serial.println();
    delay(5);      
  }
}

void showRTCHexdump(){
   
   char text[15];
  Serial.println();
  
  for(int adr = 0; adr < 19; adr=adr+16){
    Serial.print("0x");
    Serial.print(adr, HEX);
    Serial.print("\t");

    for(int y = 0; y < 16; y++){
      if((adr+y) == 19) continue;
      int B = i2c_eeprom_read_byte( RTC_I2C_ADDR, adr+y );
      //int i = (adr + 16) % 16;
      text[y] = B;
      if(B < 16) Serial.print("0");
      Serial.print(B, HEX);
      Serial.print(" "); 
    }
    Serial.println();
    delay(5);      
  }
      
}

byte StringToIP(String ip){
  Serial.print("StringToIP: ");
  Serial.println(ip);
  //max length of string is 16 chars (4x dot, 4x 1-3 cipher number)
  //if( string.length <= 16 ) {
    Serial.println(ip.length());
  //string have to contain four dots
  //substrings are 1-3 ciphers
}

byte decToBcd(byte val){
    return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
    return( (val/16*10) + (val%16) );
}


