String CW = "ZZMD04;" ;
String LSB = "ZZMD00;" ;
String USB = "ZZMD01;" ;
String b_160 = "ZZFA00001820000;" ;
String b_80  = "ZZFA00003500000;" ;
String b_40  = "ZZFA00007000000;" ;
String b_20  = "ZZFA00014000000;" ;
String b_15  = "ZZFA00021000000;" ;
String b_10  = "ZZFA00028000000;" ;
String band ;
String mode ;


void setup() {
  Serial.begin(9600);
  pinMode(12, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
}

void loop() {

  int  state = digitalRead(12);
  if (state == HIGH) {
   digitalWrite(13, HIGH);
   band = b_160 ;
  } else {
   digitalWrite(13, LOW);
   band = b_20 ;
  }

  int  state1 = digitalRead(11);
  if (state1 == HIGH) {
   mode = CW ;
  } else {
   mode = LSB ;
  }

  
  if (Serial.available() > 0)
  {
    Serial.print(mode);
    Serial.print(band);
  }
}
