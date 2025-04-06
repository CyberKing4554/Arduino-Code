


void alarm(){
unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (isFirstAction) {
      tone(BUZ, 1000);
    } else {
      tone(BUZ, 500);
    }
    isFirstAction = !isFirstAction;
 }
}