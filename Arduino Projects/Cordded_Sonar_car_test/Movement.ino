unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 100; // Check every 100ms
int speed= 128;

void run() {
  unsigned long currentTime = millis();
  if (currentTime - lastCheckTime >= checkInterval) {
    lastCheckTime = currentTime;
    
    if (Serial.available()) {
      Serial.read();
      String mo = Serial.readString();
      char mov = mo.charAt(0);
      switch(mov) {
        case 'n': stopMove(); digitalWrite(LED_BUILTIN, LOW); break;
        case 'w': forward(); digitalWrite(LED_BUILTIN, HIGH); break;
        case 'd': right(); digitalWrite(LED_BUILTIN, HIGH); break;
        case 's': back(); digitalWrite(LED_BUILTIN, HIGH); break;
        case 'a': left(); digitalWrite(LED_BUILTIN, HIGH); break;
      }
    }
  }
}
void stopMove(){
  digitalWrite(ENABLE1, LOW);
  digitalWrite(ENABLE2, LOW);
  digitalWrite(DIRA1, LOW);
  digitalWrite(DIRB1, LOW);
  digitalWrite(DIRA2, LOW);
  digitalWrite(DIRB2, LOW);
}

void forward(){
  analogWrite(ENABLE1, speed);
  analogWrite(ENABLE2, speed);
  digitalWrite(DIRA1, HIGH);
  digitalWrite(DIRA2, HIGH);
  digitalWrite(DIRB1, LOW);
  digitalWrite(DIRB2, LOW);
}

void right(){
  analogWrite(ENABLE1, speed);
  analogWrite(ENABLE2, speed);
  digitalWrite(DIRA1, HIGH);
  digitalWrite(DIRA2, LOW);
  digitalWrite(DIRB1, LOW);
  digitalWrite(DIRB2, HIGH);
}
void left(){
  analogWrite(ENABLE1, speed);
  analogWrite(ENABLE2, speed);
  digitalWrite(DIRA1, LOW);
  digitalWrite(DIRA2, HIGH);
  digitalWrite(DIRB1, HIGH);
  digitalWrite(DIRB2, LOW);
}
void back(){
  analogWrite(ENABLE1, speed);
  analogWrite(ENABLE2, speed);
  digitalWrite(DIRA1, LOW);
  digitalWrite(DIRA2, LOW);
  digitalWrite(DIRB1, HIGH);
  digitalWrite(DIRB2, HIGH);
}