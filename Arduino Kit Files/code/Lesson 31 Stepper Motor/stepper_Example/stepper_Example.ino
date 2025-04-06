//www.elegoo.com
//2018.10.25

/*
  Stepper Motor Control - one revolution

  This program drives a unipolar or bipolar stepper motor.
  The motor is attached to digital pins 8 - 11 of the Arduino.

  The motor should revolve one revolution in one direction, then
  one revolution in the other direction.

*/

#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
const int rolePerMinute = 15;         // Adjustable range of 28BYJ-48 stepper is 0~17 rpm
int speed = 15;
int steps = 1;
int currentPosition = 0;
String input;
int stuf;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(rolePerMinute);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {  
  // step one revolution  in one direction:
  if(Serial.available()> 0){
    input = Serial.readString();
    stuf = input.toInt();
    steps = stuf;
  }

  myStepper.step(steps/16);
  currentPosition += steps/16;
  delay(100);

  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

  myStepper.step(steps/8);
  currentPosition += steps/8;
  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

    myStepper.step(steps/4);
  currentPosition += steps/4;

  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

  myStepper.step(steps/2);
  currentPosition += steps/2;
  delay(100);
  steps += 50;
  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

  myStepper.step(steps/4);
  currentPosition += steps/4;

  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

    myStepper.step(steps/8);
  currentPosition += steps/8;

  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

  myStepper.step(steps/16);
  currentPosition += steps/16;
  delay(100);
  Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);
  // step one revolution in the other direction:


    myStepper.step(-steps/16);
    currentPosition -= steps/16;
  delay(100);
  steps += 50;
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

      myStepper.step(-steps/8);
    currentPosition -= steps/8;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

      myStepper.step(-steps/4);
    currentPosition -= steps/4;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

      myStepper.step(-steps/2);
    currentPosition -= steps/2;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

        myStepper.step(-steps/4);
    currentPosition -= steps/4;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

      myStepper.step(-steps/8);
    currentPosition -= steps/8;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);

      myStepper.step(-steps/16);
    currentPosition -= steps/16;
  delay(100);
    Serial.print("s:");
  Serial.println(steps);
  Serial.print("p:");
  Serial.println(currentPosition);
}
