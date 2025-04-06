/* 
Code created by: Spencer Lewis
*/
#define RED 11
#define BLUE 12
#define BUZZ 10
#define CLIK1 8
#define CLIK2 9

bool led = false;
int randVal;
int button;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZ, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(CLIK1, INPUT_PULLUP);
  pinMode(CLIK2, INPUT_PULLUP);
  button = 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  randVal =random(0,3);

  clearScreen();
    Serial.println(String(randVal));
  Serial.println("Enter Your guess:\n Heads or Tails");
  while (button == 0){
    clikCheck();
  }
  Serial.println("Setting up digital environment....");
  delay(random(500,1500));
  Serial.println("You Chose:");
  if(button == 1){
    Serial.println("Red");
  } else {
    Serial.println("Blue");
  }
  clearScreen();
  Serial.println("Begining Sequence...");
  delay(100);
  for (int i =0;i<=1000; i = i+50 ){
    
    if(led == false){
      digitalWrite(BLUE, HIGH);
      digitalWrite(RED, LOW);  
    } else {
      digitalWrite(BLUE, LOW);
      digitalWrite(RED, HIGH); 
    }
    led = !led;
    delay(i);
  }
  if(randVal = 1){
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH); 
  } else if(randVal = 2){
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
  } else{
    Serial.println("CRIICAL_ERROR:  Random Value was equivelent to unknown value");
    Serial.println("Please Restart Program---raVal--Err");
    while(0==0){}
  }
    if(randVal == button){
      Serial.println("You Guessed Correctly!! 🎉🎉🎉");
      tone(BUZZ, 1318, 150);   //E6
      delay(175);
      tone(BUZZ, 1567, 150);   //G6
      delay(175);
      tone(BUZZ, 2637, 150);   //E7
      delay(175);
      tone(BUZZ, 2093, 150);   //C7
      delay(175);
      tone(BUZZ, 2349, 150);   //D7
      delay(175);
      tone(BUZZ, 3135, 500);   //G7
      delay(500);  
      noTone(BUZZ);
    } else {
      Serial.println("You Guessed Incorrectly ):");
      tone(BUZZ, 130, 250);   //E6
      delay(275);
      tone(BUZZ, 73, 250);   //G6
      delay(275);
      tone(BUZZ, 65, 150);   //E7
      delay(175);
      tone(BUZZ, 98, 500);   //C7
      delay(500);
      noTone(BUZZ);
    }
    button = 0;
}

void clikCheck(){
  if(digitalRead(CLIK1) == LOW){
    button = 1;
  } else if (digitalRead(CLIK2) == LOW){
    button = 2;
  } else {
    button = 0;
  }


}
void clearScreen() {
  for(int i = 0; i < 50; i++) {
    Serial.println();
  }
}