#include <Servo.h>
String yay="";
int serv;
Servo s;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
s.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  s.write(90);

}
