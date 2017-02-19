/* 
 *  EEPROM Memory alocation plan
 *  Addresses: 0x0 to 0xfff

(dec)addr    bit(s)  Purpose
----------------------------------------------------
  00          0       dhcp client [0=off, 1=on]
              1       
              2
              3
              4
              5
              6
              7
  01 - 03     all     ---RESERVED---
  04 - 07     all     static IPv4 address
  08 - 11     all     static mask
  12 - 15     all     static default gateway
  16 - 19     all     static DNS
  20 - 23     all     NTP server IP address
  24 - 56     all     32byte SSID (+1 byte terminating 0)
  57 - 121    all     63byte Wpa/Wpa2 PSK (+1 byte terminating 0)
  119 - 255   all     ---RESERVED---


5 byte  - wep40 bit key
13 byte - wep104 bit key
6 byte - wlan security [open,wep64,wep128,wpa,wpa2]

--- Timezone and DST ---
https://github.com/JChristensen/Timezone
2byte - (int) timezone UTC -12 to +14
12 byte summer time start
12 byte summer time end

zbytek pro logy
*/
