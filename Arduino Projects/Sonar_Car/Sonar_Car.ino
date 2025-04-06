#include <Servo.h>
#define BUZ 5

unsigned long previousMillis = 0;
const long interval = 750; // Interval of 1 second
bool isFirstAction = true;   // Flag to track which action to perform

const int trigPin=12;
const int echoPin=11;
long  duration;
int distance;
Servo s1;
int almDis= 30;

void setup() {
  Serial1.begin(9600);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  s1.attach(9);
  pinMode(BUZ, OUTPUT);
}

void  loop(){
    for(int i=0;i<180;i=i+1){            
        s1.write(i);
        delay(30);
        distance = calDist();
        Serial1.print(i);                      
        Serial1.print(",");                   
        Serial1.print(distance);              
        Serial1.print("."); 
        if (distance < almDis){
          alarm();
        } else {
          noTone(BUZ);
        }

     }
    for(int i=180;i>0;i=i-1){  
        s1.write(i);
        delay(30);
        distance = calDist();
        Serial1.print(i);
        Serial1.print(",");
        Serial1.print(distance);
        Serial1.print(".");
        if (distance < almDis){
          alarm();
        } else {
          noTone(BUZ);
        }

    }
}

    int calDist(){
       digitalWrite(trigPin, LOW);
       delayMicroseconds(2);
       digitalWrite(trigPin, HIGH);
       delayMicroseconds(10);
       digitalWrite(trigPin,  LOW);                                                     
       duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout                           
       distance= duration*0.034/2;                  
       return distance;
}
