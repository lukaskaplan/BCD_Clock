/*------------------------------------------*
 *---------------- I2C Funkce --------------*
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
    //Wire.write((int)(eeaddress & 0x00FF)); // odešle vyšší byte
    Wire.write((int)(eeaddress)); // odešle vyšší byte
    Wire.write((int)(eeaddress & 0xFF)); // odešle nižší byte
    Wire.write(rdata);
    Wire.endTransmission();
    delay(5);
}
 
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  /*
  * Slouzi pro precteni jednoho byte do i2c eeprom
  *
  * deviceaddress = i2c adresa eeprom
  * eeaddress = adresa konkretniho byte v pameti
  * return rdata = vraci byte (hodnota z pameti)
  */
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    //Wire.write((int)(eeaddress & 0x00FF)); // odešle vyšší byte
    Wire.write((int)(eeaddress)); // odešle vyšší byte
    Wire.write((int)(eeaddress & 0xFF)); // odešle nižší byte
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
}
  
/*------------------------------------------*
 *---------------- CLI Funkce --------------*
 *------------------------------------------*/
void showHelp() {
  //print cli help (possible commands with explanation)
  Serial.println();
  //Serial.println("show config");
  Serial.println("show mac\t\tShow wireless MAC address");
  //Serial.println("show log");
  Serial.println("show time");
  Serial.println("show wlans\t\tScan and show wireless networks");
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

byte decToBcd(byte val){
    return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
    return( (val/16*10) + (val%16) );
}