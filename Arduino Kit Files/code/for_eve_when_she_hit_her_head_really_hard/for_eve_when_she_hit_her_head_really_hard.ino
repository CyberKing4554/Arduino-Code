#include <LiquidCrystal.h>

#include "IRremote.h"

LiquidCrystal lcd(6, 7, 8, 9, 10, 11);

int receiver = 26; // Signal Pin of IR receiver to Arduino Digital Pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;
boolean yn = false;
void translateIR() // takes action based on IR code received

// describing Remote IR codes 

//function variables


{

  if (results.value == 0xFF02FD){
    if (yn == false) {
      yn = true;
    } else {
      yn = false;
    }
  }
  

 // Do not get immediate repeat


} //END translateIR


void setup() {
  // put your setup code here, to run once:

  lcd.begin(16, 2);
  irrecv.enableIRIn();
  pinMode(22,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)) // have we received an IR signal?

  {
    translateIR(); 
    irrecv.resume(); // receive the next value
  }  
  if (yn == true){
    lcd.setCursor(0,0);
    lcd.print("I'm sorry you ");
    lcd.setCursor(0,1);
    lcd.print("got hurt.");
    delay(5000);
    lcd.clear();
    lcd.print("However I'm glad ");
    lcd.setCursor(0,1);
    lcd.print("Your feeling...");
    delay(5000);
    lcd.clear();
    lcd.print("Better !!!");
    delay(3000);
    lcd.clear();
    lcd.print(":-)");
    delay(2500);
    lcd.clear();
    yn = false;
  }
}
