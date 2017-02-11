
const byte numChars = 32; //maximalni delka prikazu
char receivedChars[numChars]; // an array to store the received data
char prompt[] = "Prompt# ";

char enter = '\r';
char help = '?';
char bckspc = 0x08;

byte ndx = 0;
char rc;

void setup() {
 Serial.begin(9600);
 Serial.println(F("<Arduino is ready>"));
 Serial.print(prompt);
}

void loop() {
  
        
  if(Serial.available() > 0) {
    delay(100);
    rc = Serial.read();
    if (rc == enter) {
      Serial.println();
      if(strlen(receivedChars) > 0) {
        Serial.print("\tZadali jste: ");
        Serial.println(receivedChars);
      }
      Serial.print(prompt);
      // smazani stringu a zaciname na novo
      for(int i = 0; i < strlen(receivedChars); i++){
        receivedChars[i] = '\0';   
      }
      ndx = 0;
    }
    else if (rc == help) {
      receivedChars[ndx] = '\0';
      Serial.println();
      Serial.println("\tHELP");
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
      ndx++;
    }
  }   

} // ------------- end of loop --------------------------

