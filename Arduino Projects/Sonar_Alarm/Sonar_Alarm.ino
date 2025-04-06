#include <Servo.h>

#define ENABLE1 8
#define ENABLE2 10
#define DIRA1 24
#define DIRB1 25
#define DIRA2 26
#define DIRB2 27

const int trigPin=12;
const int echoPin=11;
long  duration;
int distance;
Servo s1;

void setup() {
  Serial1.begin(9600);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  s1.attach(9);
  pinMode(LED_BUILTIN, OUTPUT);
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
          delay(10);
        run();
     }
    for(int i=180;i>0;i=i-1){  
        s1.write(i);
        delay(30);
        distance = calDist();
        Serial1.print(i);
        Serial1.print(",");
        Serial1.print(distance);
        Serial1.print(".");
          delay(10);
      run();
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
