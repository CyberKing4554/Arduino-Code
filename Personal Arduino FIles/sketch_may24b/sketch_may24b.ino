#include <URTouch.h>
#include <URTouchCD.h>
#include <UTFT.h>
#include <memorysaver.h>


//Initalize Classes
UTFT myGLCD (ILI9341_16, 38, 39, 40, 41);
URTouch myTouch (6,5,4,3,2);

//Font Setup
extern uint8_t SmallFont [];
extern uint8_t BigFont [];
extern uint8_t SevenSegNumFont [];

//Borb Varialbes.
int borX;
int borY;

//Pipe Variables.
int pipX;
int pipY;

//Touch Variables
int x;
int y;

void setup() {
  // put your setup code here, to run once:

myGLCD.InitLCD(1);
myGLCD.clrScr();
myTouch.InitTouch();
myTouch.setPrecision(PREC_MEDIUM);
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
startScreen();
}

void loop() {
  // put your main code here, to run repeatedly:
delay(1);
}



void startScreen (){
  myGLCD.fillScr(0,0,0);
  myGLCD.setColor(255,255,255);
  myGLCD.setFont(BigFont);
  myGLCD.print("Flappy Borb", CENTER, 10);
  myGLCD.fillScr(0,0,0);
}



