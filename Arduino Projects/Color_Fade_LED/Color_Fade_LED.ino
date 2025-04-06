#include "arduino_secrets.h"

//www.elegoo.com
//2016.12.8
#define RED 10
#define BLUE 12
#define GREEN 13

void setup() {
  Serial.begin(9600);
pinMode(RED, OUTPUT);
pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
    delay(5000);
}

int redValue;
int blueValue;
int greenValue;
int delayTime;

void loop() {
  
  Serial.println("Red Threshold");
  
  redValue = 225;
  blueValue = 0;
  greenValue = 0;
  
  for(int i = 0; i <225; i += 1){
    
    
    redValue -= 1;
    greenValue += 1;
    analogWrite(RED, redValue);
    analogWrite(GREEN, greenValue);
    delayTime = 10;
    delay(delayTime);
  }
    Serial.println("Green Threshold");
    
    
      for(int i = 0; i <225; i += 1){
    
    
    greenValue -= 1;
    blueValue += 1;
    analogWrite(GREEN, greenValue);
    analogWrite(BLUE, blueValue);
    delayTime = 10;
    delay(delayTime);
  }
    Serial.println("Blue Threshold");
    
      for(int i = 0; i <225; i += 1){
    
    
    blueValue -= 1;
    redValue += 1;
    analogWrite(BLUE, blueValue);
    analogWrite(RED, redValue);
    delayTime = 10;
    delay(delayTime);
  }
    Serial.println("Sequence Complete");
}
