void setup() {
  Serial.begin(9600);
}

void docmd() {
  byte START = Serial.read();
  if( START != 0x80 ) return;
  byte DEVID = Serial.read();
  if( DEVID != 0x01 ) return;
  byte CMD = Serial.read();
  byte SERVO = Serial.read();
  byte DAT1 = Serial.read();
  Serial.print(0xFE,BYTE);
  Serial.print(0x01,BYTE);
  if( CMD == 0x00 )
    Serial.print("CFG ");
  else if( CMD == 0x01 )
    Serial.print("SPEED ");
  else if( CMD == 0x02 )
    Serial.print("POS7 ");
  else if( CMD == 0x03 )
    Serial.print("POS8 ");
  else if( CMD == 0x04 )
    Serial.print("POSABS ");
  else if( CMD == 0x05 )
    Serial.print("NEUTRAL ");
  Serial.print( "#" );
  Serial.print(SERVO,DEC);
  Serial.print(0xFE,BYTE);
  Serial.print(192,BYTE);
  Serial.print(DAT1,BIN);
  if( CMD == 0x03 || CMD == 0x04 || CMD == 0x05 ) {
    Serial.print( " " );
    Serial.print( Serial.read(), BIN );
  }
}

void loop() {
  if(Serial.available() >= 5) {
    docmd();
    while(Serial.available()) Serial.read();
  }
}
