/*
  ************************************************************************************
  * MIT License
  *
  * Copyright (c) 2025 Crunchlabs LLC (LabelMaker Code)

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  ************************************************************************************
*/
//////////////////////////////////////////////////
    //  LIBRARIES  //
//////////////////////////////////////////////////
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ezButton.h>
#include <Servo.h>
#include <avr/pgmspace.h>
//#include <SparkFun_TB6612.h>


//////////////////////////////////////////////////
//  PINS AND PARAMETERS  //
//////////////////////////////////////////////////

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16x2 display

ezButton button1(14); //joystick button handler
#define INIT_MSG "Initializing..." // Text to display on startup
#define MODE_NAME "LABELMAKER" //these are variables for the text which is displayed in different menus. 
#define PRINT_CONF "  PRINT LABEL?  " //try changing these, or making new ones and adding conditions for when they are used
#define PRINTING "    PRINTING    " // NOTE: this text must be 16 characters or LESS in order to fit on the screen correctly
#define MENU_CLEAR ":                " //this one clears the menu for editing
#define LOADING_SPEED 1


//text variables
const int x_scale = 230;//these are multiplied against the stored coordinate (between 0 and 4) to get the actual number of steps moved
const int y_scale = 230;//for example, if this is 230(default), then 230(scale) x 4(max coordinate) = 920 (motor steps)
const int scale = x_scale;
const int space = x_scale * 5; //space size between letters (as steps) based on X scale in order to match letter width
//multiplied by 5 because the scale variables are multiplied against coordinates later, while space is just fed in directly, so it needs to be scaled up by 5 to match


// Joystick setup
const int joystickXPin = A2;  // Connect the joystick X-axis to this analog pin
const int joystickYPin = A1;  // Connect the joystick Y-axis to this analog pin
const int joystickButtonThreshold = 200;  // Adjust this threshold value based on your joystick
double totalVectorDrawing = 0;
double currentVectorDrawing = 0;
double drawingPrecentage;

// Menu parameters
const char alphabet[] = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?,.#@"; //alphabet menu
const int alphabetSize = sizeof(alphabet) - 1;
String text;  // Store the label text
String textLine1;
String textLine2;
bool isFirstline = true;

int currentCharacter = 0; //keep track of which character is currently displayed under the cursor
int cursorPosition = 0; //keeps track of the cursor position (left to right) on the screen
int currentPage = 0; //keeps track of the current page for menus
const int charactersPerPage = 16; //number of characters that can fit on one row of the screen

// Stepper motor parameters
const int stepCount = 200;
const int stepsPerRevolution = 2048;

// initialize the stepper library for both steppers:
Stepper xStepper(stepsPerRevolution, 6, 8, 7, 9);
Stepper yStepper(stepsPerRevolution, 2, 4, 3, 5); 

const int xPins[4] = {6, 8, 7, 9};  // pins for x-motor coils
const int yPins[4] = {2, 4, 3, 5};    // pins for y-motor coils

//Servo
const int SERVO_PIN  = 13;
Servo servo;
int angle = 30; // the current angle of servo motor


// Creates states to store what the current menu and joystick states are
// Decoupling the state from other functions is good because it means the sensor / screen aren't hardcoded into every single action and can be handled at a higher level
enum State { MainMenu, Editing, PrintConfirmation, Printing };
State currentState = MainMenu;
State prevState = Printing;

enum jState {LEFT, RIGHT, UP, DOWN, MIDDLE, UPRIGHT, UPLEFT, DOWNRIGHT, DOWNLEFT};
jState joyState = MIDDLE;
jState prevJoyState = MIDDLE;

boolean pPenOnPaper = false; // pen on paper in previous cycle
//int lineCount = 0;

const byte* invertedCharacters[128];

int xpos = 0;
int ypos = 0;
const int posS = 2;
const int posM = 7;
const int posL = 12;
bool joyUp;
bool joyDown;
bool joyLeft;
bool joyRight;
int button1State;
int joystickX;
int joystickY;

//////////////////////////////////////////////////
//  CHARACTER VECTORS  //
//////////////////////////////////////////////////
const uint8_t vector[63][14] PROGMEM = { //this alphabet set comes from a great plotter project you can find here: 
  /*
    encoding works as follows:
    ones     = y coordinate;
    tens     = x coordinate;
    hundreds = draw/don't draw ..
    200      = end
    222      = plot point
    !! for some reason leading zeros cause problems !!
  */
  {  0,  124,  140,  32,  112,   200,  200,  200,  200,  200,  200,  200,  200,  200}, //my A character
  {  0,  104,  134,  132,    2,  142,  140,  100,  200,  200,  200,  200,  200,  200}, /*B*/ // the 2 was originally 002, not sure why
  { 41,  130,  110,  101,  103,  114,  134,  143,  200,  200,  200,  200,  200,  200}, /*C*/
  {  0,  104,  134,  143,  141,  130,  100,  200,  200,  200,  200,  200,  200,  200}, /*D*/
  { 40,  100,  104,  144,   22,  102,  200,  200,  200,  200,  200,  200,  200,  200}, /*E*/
  {  0,  104,  144,   22,  102,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*F*/
  { 44,  104,  100,  140,  142,  122,  200,  200,  200,  200,  200,  200,  200,  200}, /*G*/
  {  0,  104,    2,  142,   44,  140,  200,  200,  200,  200,  200,  200,  200,  200}, /*H*/
  {  0,  104,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*I*/
  {  1,  110,  130,  141,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*J*/
  {  0,  104,    2,  142,  140,   22,  144,  200,  200,  200,  200,  200,  200,  200}, /*K*/
  { 40,  100,  104,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*L*/
  {  0,  104,  122,  144,  140,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*M */
  {  0,  104,  140,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*N*/
  { 10,  101,  103,  114,  134,  143,  141,  130,  110,  200,  200,  200,  200,  200}, /*O*/
  {  0,  104,  144,  142,  102,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*P*/
  {  0,  104,  144,  142,  120,  100,   22,  140,  200,  200,  200,  200,  200,  200}, /*Q*/
  {  0,  104,  144,  142,  102,   22,  140,  200,  200,  200,  200,  200,  200,  200}, /*R*/
  {  0,  140,  142,  102,  104,  144,  200,  200,  200,  200,  200,  200,  200,  200}, /*S*/
  { 20,  124,    4,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*T*/
  {  4,  101,  110,  130,  141,  144,  200,  200,  200,  200,  200,  200,  200,  200}, /*U*/
  {  4,  120,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*V*/
  {  4,  100,  122,  140,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*W*/
  {  0,  144,    4,  140,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*X*/
  {  4,  122,  144,   22,  120,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*Y*/
  {  4,  144,  100,  140,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*Z*/
  {  0,  104,  144,  140,  100,  144,  200,  200,  200,  200,  200,  200,  200,  200}, /*0*/
  {  0,  140,   20,  124,  104,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*1*/
  {  4,  144,  142,  102,  100,  140,  200,  200,  200,  200,  200,  200,  200,  200}, /*2*/
  {  0,  140,  144,  104,   12,  142,  200,  200,  200,  200,  200,  200,  200,  200}, /*3*/
  { 20,  123,   42,  102,  104,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*4*/
  {  0,  140,  142,  102,  104,  144,  200,  200,  200,  200,  200,  200,  200,  200}, /*5*/
  {  2,  142,  140,  100,  104,  144,  200,  200,  200,  200,  200,  200,  200,  200}, /*6*/
  {  0,  144,  104,   12,  132,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*7*/
  {  0,  140,  144,  104,  100,    2,  142,  200,  200,  200,  200,  200,  200,  200}, /*8*/
  {  0,  140,  144,  104,  102,  142,  200,  200,  200,  200,  200,  200,  200,  200}, /*9*/
  { 200, 200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /* */
  { 200, 200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /* */
  {  0,  144,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*/*/
  {  0,  102,  124,  142,  140,   42,  102,    4,  103,   44,  143,  200,  200,  200}, /*Ä*/
  {  0,  102,  142,  140,  100,    2,   14,  113,   34,  133,  200,  200,  200,  200}, /*Ö*/
  {  4,  100,  140,  144,   14,  113,   34,  133,  200,  200,  200,  200,  200,  200}, /*Ü*/
  {  0,  111,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*,*/
  {  2,  142,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*-*/
  {  0,  222,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*.*/
  {  0,  222,    1,  104,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*!*/
  {  20, 222,   21,  122,  142,  144,  104,  200,  200,  200,  200,  200,  200,  200}, /*?*/ 
  {  0,  104,  134,  133,  122,  142,  140,  110,  200,  200,  200,  200,  200,  200}, /*ß*/
  {  23, 124,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*'*/
  {  42, 120,  100,  101,  123,  124,  104,  103,  130,  140,  200,  200,  200,  200}, /*&*/
  {  2,  142,   20,  124,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*+*/
  {  21, 222,   23,  222,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*:*/
  {  10, 121,   22,  222,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*;*/
  {  14, 113,   33,  134,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*"*/
  {  10, 114,   34,  130,   41,  101,    3,  143,  200,  200,  200,  200,  200,  200}, /*#*/
  {  34, 124,  120,  130,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*(*/
  {  10, 120,  124,  114,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*)*/
  {  1,  141,   43,  103,  200,  200,  200,  200,  200,  200,  200,  200,  200,  200}, /*=*/
  {  31, 133,  113,  111,  141,  144,  104,  100,  140,  200,  200,  200,  200,  200}, /*@*/
  {  2,  142,   20,  124,    4,  140,  0,    144,  200,  200,  200,  200,  200,  200}, /***/
  {  0,  140,  144,  104,  100,   12,  113,   33,  132,   31,  111,  200,  200,  200}, /*} Smiley*/
  {  0,  140,  144,  104,  100,   13,  222,   33,  222,   32,  131,  111,  112,  132}, /*~ Open mouth Smiley*/
  {  20, 142,  143,  134,  123,  114,  103,  102,  120,  200,  200,  200,  200,  200} /*$ Heart*/
};

//////////////////////////////////////////////////
    //  C U S T O M   L C D   C H A R S  //
//////////////////////////////////////////////////


const byte loading1[8] PROGMEM = {
    B00000,
    B00000,
    B00000,
    B00000,
    B01110,
    B01110,
    B00000
};

const byte loading2[8] PROGMEM = {
    B00000,
    B01110,
    B01110,
    B00000,
    B00000,
    B00000,
    B00000
};

const byte home1[8] PROGMEM = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
};

const byte bottomLeftHome1[8] PROGMEM = {
    B00000,
    B11000,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
};

const byte bottomLeftHome2[8] PROGMEM = {
    B00000,
    B00000,
    B00000,
    B10000,
    B11100,
    B11111,
    B11111,
    B11111
};

const byte bottomLeftHome3[8] PROGMEM = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10000,
    B11100,
    B11111
};

const byte bottomRightHome1[8] PROGMEM = {
    B00000,
    B00011,
    B01111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
};

const byte bottomRightHome2[8] PROGMEM = {
    B00000,
    B00000,
    B00000,
    B00011,
    B01111,
    B11111,
    B11111,
    B11111
};

const byte bottomRightHome3[8] PROGMEM = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00001,
    B00111,
    B11111
};

const byte invertRightArrow[8] PROGMEM = {
    B10111,
    B11011,
    B11101,
    B11110,
    B11101,
    B11011,
    B10111
};

const byte inverted_[8] PROGMEM = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000
};

const byte invertedA[8] PROGMEM = {
    B10001,
    B01110,
    B01110,
    B01110,
    B00000,
    B01110,
    B01110
};

const byte invertedB[8] PROGMEM = {
    B00001,
    B01110,
    B01110,
    B00001,
    B01110,
    B01110,
    B00001
};

const byte invertedC[8] PROGMEM = {
    B10001,
    B01110,
    B01111,
    B01111,
    B01111,
    B01110,
    B10001
};

const byte invertedD[8] PROGMEM = {
    B00011,
    B01101,
    B01110,
    B01110,
    B01110,
    B01101,
    B00011
};

const byte invertedE[8] PROGMEM = {
    B00000,
    B01111,
    B01111,
    B00001,
    B01111,
    B01111,
    B00000
};

const byte invertedF[8] PROGMEM = {
    B00000,
    B01111,
    B01111,
    B00001,
    B01111,
    B01111,
    B01111
};

const byte invertedG[8] PROGMEM = {
    B10001,
    B01110,
    B01111,
    B01000,
    B01110,
    B01110,
    B10000
};

const byte invertedH[8] PROGMEM = {
    B01110,
    B01110,
    B01110,
    B00000,
    B01110,
    B01110,
    B01110
};

const byte invertedI[8] PROGMEM = {
    B10001,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B10001
};

const byte invertedJ[8] PROGMEM = {
    B11000,
    B11101,
    B11101,
    B11101,
    B11101,
    B01101,
    B10011
};

const byte invertedK[8] PROGMEM = {
    B01110,
    B01101,
    B01011,
    B00111,
    B01011,
    B01101,
    B01110
};

const byte invertedL[8] PROGMEM = {
    B01111,
    B01111,
    B01111,
    B01111,
    B01111,
    B01111,
    B00000
};

const byte invertedM[8] PROGMEM = {
    B01110,
    B00100,
    B01010,
    B01110,
    B01110,
    B01110,
    B01110
};

const byte invertedN[8] PROGMEM = {
    B01110,
    B01110,
    B00110,
    B01010,
    B01100,
    B01110,
    B01110
};

const byte invertedO[8] PROGMEM = {
    B10001,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B10001
};

const byte invertedP[8] PROGMEM = {
    B00001,
    B01110,
    B01110,
    B00001,
    B01111,
    B01111,
    B01111
};

const byte invertedQ[8] PROGMEM = {
    B10001,
    B01110,
    B01110,
    B01110,
    B01010,
    B01101,
    B10010
};

const byte invertedR[8] PROGMEM = {
    B00001,
    B01110,
    B01110,
    B00001,
    B01011,
    B01101,
    B01110
};

const byte invertedS[8] PROGMEM = {
    B10000,
    B01111,
    B01111,
    B10001,
    B11110,
    B11110,
    B00001
};

const byte invertedT[8] PROGMEM = {
    B00000,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011
};

const byte invertedU[8] PROGMEM = {
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B10001
};

const byte invertedV[8] PROGMEM = {
    B01110,
    B01110,
    B01110,
    B01110,
    B01110,
    B10101,
    B11011
};

const byte invertedW[8] PROGMEM = {
    B01110,
    B01110,
    B01110,
    B01010,
    B01010,
    B01010,
    B10101
};

const byte invertedX[8] PROGMEM = {
    B01110,
    B01110,
    B10101,
    B11011,
    B10101,
    B01110,
    B01110
};

const byte invertedY[8] PROGMEM = {
    B01110,
    B01110,
    B01110,
    B10101,
    B11011,
    B11011,
    B11011
};

const byte invertedZ[8] PROGMEM = {
    B00000,
    B11110,
    B11101,
    B11011,
    B10111,
    B01111,
    B00000
};

const byte inverted0[8] PROGMEM = {
    B10001,
    B01110,
    B01100,
    B01010,
    B00110,
    B01110,
    B10001
};

const byte inverted1[8] PROGMEM = {
    B11011,
    B10011,
    B11011,
    B11011,
    B11011,
    B11011,
    B10001
};

const byte inverted2[8] PROGMEM = {
    B10001,
    B01110,
    B11110,
    B11101,
    B11011,
    B10111,
    B00000
};

const byte inverted3[8] PROGMEM = {
    B00000,
    B11101,
    B11011,
    B11101,
    B11110,
    B01110,
    B10001
};

const byte inverted4[8] PROGMEM = {
    B11101,
    B11001,
    B10101,
    B01101,
    B00000,
    B11101,
    B11101
};

const byte inverted5[8] PROGMEM = {
    B00000,
    B01111,
    B00001,
    B11110,
    B11110,
    B01110,
    B10001
};

const byte inverted6[8] PROGMEM = {
    B11001,
    B10111,
    B01111,
    B00001,
    B01110,
    B01110,
    B10001
};

const byte inverted7[8] PROGMEM = {
    B00000,
    B11110,
    B11101,
    B11011,
    B10111,
    B10111,
    B10111
};

const byte inverted8[8] PROGMEM = {
    B10001,
    B01110,
    B01110,
    B10001,
    B01110,
    B01110,
    B10001
};

const byte inverted9[8] PROGMEM = {
    B10001,
    B01110,
    B01110,
    B10000,
    B11110,
    B11101,
    B10011
};

const byte invertedExlamation[8] PROGMEM = {
    B11011,
    B11011,
    B11011,
    B11011,
    B11111,
    B11111,
    B11011
};

const byte invertedQuestion[8] PROGMEM = {
    B10001,
    B01110,
    B11110,
    B11101,
    B11011,
    B11111,
    B11011
};

const byte invertedComma[8] PROGMEM = {
    B11111,
    B11111,
    B11111,
    B11111,
    B10011,
    B11011,
    B10111
};

const byte invertedPeriod[8] PROGMEM = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B10011,
    B10011
};

const byte invertedHashtag[8] PROGMEM = {
    B10101,
    B10101,
    B00000,
    B10101,
    B00000,
    B10101,
    B10101
};

const byte invertedAt[8] PROGMEM = {
    B10001,
    B01110,
    B11110,
    B10010,
    B01010,
    B01010,
    B10001
};

const byte twenty[8] PROGMEM = {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000
};

const byte fourty[8] PROGMEM = {
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000
};

const byte sixty[8] PROGMEM = {
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100
};

const byte eighty[8] PROGMEM = {
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110
};


void setupInvertedChars(){
  invertedCharacters['_'] = inverted_;
  invertedCharacters['A'] = invertedA;
  invertedCharacters['B'] = invertedB;
  invertedCharacters['C'] = invertedC;
  invertedCharacters['D'] = invertedD;
  invertedCharacters['E'] = invertedE;
  invertedCharacters['F'] = invertedF;
  invertedCharacters['G'] = invertedG;
  invertedCharacters['H'] = invertedH;
  invertedCharacters['I'] = invertedI;
  invertedCharacters['J'] = invertedJ;
  invertedCharacters['K'] = invertedK;
  invertedCharacters['L'] = invertedL;
  invertedCharacters['M'] = invertedM;
  invertedCharacters['N'] = invertedN;
  invertedCharacters['O'] = invertedO;
  invertedCharacters['P'] = invertedP;
  invertedCharacters['Q'] = invertedQ;
  invertedCharacters['R'] = invertedR;
  invertedCharacters['S'] = invertedS;
  invertedCharacters['T'] = invertedT;
  invertedCharacters['U'] = invertedU;
  invertedCharacters['V'] = invertedV;
  invertedCharacters['W'] = invertedW;
  invertedCharacters['X'] = invertedX;
  invertedCharacters['Y'] = invertedY;
  invertedCharacters['Z'] = invertedZ;
  invertedCharacters['0'] = inverted0;
  invertedCharacters['1'] = inverted1;
  invertedCharacters['2'] = inverted2;
  invertedCharacters['3'] = inverted3;
  invertedCharacters['4'] = inverted4;
  invertedCharacters['5'] = inverted5;
  invertedCharacters['6'] = inverted6;
  invertedCharacters['7'] = inverted7;
  invertedCharacters['8'] = inverted8;
  invertedCharacters['9'] = inverted9;
  invertedCharacters['!'] = invertedExlamation;
  invertedCharacters['?'] = invertedQuestion;
  invertedCharacters[','] = invertedComma;
  invertedCharacters['.'] = invertedPeriod;
  invertedCharacters['#'] = invertedHashtag;
  invertedCharacters['@'] = invertedAt;
}



//////////////////////////////////////////////////
//  S E T U P  //
//////////////////////////////////////////////////
void setup() {
  lcd.init();
  lcd.backlight();
  createCustomChar(0,loading1);
  createCustomChar(1,loading2);
  lcd.setCursor(0, 0);
  lcd.print(INIT_MSG);  // print start up message

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  button1.setDebounceTime(50);  //debounce prevents the joystick button from triggering twice when clicked

  servo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  servo.write(angle);

  plot(false);  //servo to tape surface so pen can be inserted

  // set the speed of the motors
  yStepper.setSpeed(12);  // set first stepper speed (these should stay the same)
  xStepper.setSpeed(10);  // set second stepper speed (^ weird stuff happens when you push it too fast)

  penUp();      //ensure that the servo is lifting the pen carriage away from the tape
  homeYAxis();  //lower the Y axis all the way to the bottom

  ypos = 0;
  xpos = 0;
  setupInvertedChars();
  releaseMotors();
  lcd.clear();
}

////////////////////////////////////////////////
//  L O O P  //
////////////////////////////////////////////////
void loop() {

  button1.loop();
  button1State = button1.getState();

  joystickX = analogRead(joystickXPin);
  joystickY = analogRead(joystickYPin);
  joyUp = joystickY < (512 - joystickButtonThreshold);
  joyDown = joystickY > (512 + joystickButtonThreshold);
  joyLeft = joystickX < (512 - joystickButtonThreshold);
  joyRight = joystickX > (512 + joystickButtonThreshold);

  switch (currentState) {  //state machine that determines what to do with the input controls based on what mode the device is in

    case MainMenu:
      {
        if (prevState != MainMenu) {
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print(MODE_NAME);
          lcd.setCursor(6, 1);
          lcd.print("START");
          createCustomChar(0,home1);
          lcd.setCursor(0,1);
          lcd.write(0);
          lcd.setCursor(15,1);
          lcd.write(0);

          createCustomChar(1, bottomLeftHome1);
          createCustomChar(2, bottomLeftHome2);
          createCustomChar(3, bottomLeftHome3);
          createCustomChar(4, bottomRightHome1);
          createCustomChar(5, bottomRightHome2);
          createCustomChar(6, bottomRightHome3);

          lcd.setCursor(1,1);
          lcd.write(1);
          lcd.write(2);
          lcd.write(3);
          lcd.print(" ");
          lcd.setCursor(11,1);
          lcd.print(" ");
          lcd.write(6);
          lcd.write(5);
          lcd.write(4);

          createCustomChar(7, invertRightArrow);

          cursorPosition = 5;
          prevState = MainMenu;
        }

        lcd.setCursor(cursorPosition, 1);

        if (millis() % 600 < 400) {  // Blink every 500 ms
          lcd.print(">");
        } else {
          lcd.write(7);
        }

        if (button1.isPressed()) {  //handles clicking options in text size setting
          lcd.clear();
          currentState = Editing;
          prevState = MainMenu;
        }
      }
      break;

    case Editing:  //in the editing mode, joystick directional input adds and removes characters from the string, while up and down changes characters
      //pressing the joystick button will switch the device into the Print Confirmation mode

      // Editing mode
      if (prevState != Editing) {
        lcd.clear();
        prevState = Editing;
      }
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(":");
      lcd.setCursor(1, 0);
      lcd.print(text);


      // Check if the joystick is moved up (previous letter) or down (next letter)

      if (joyUp) {  //UP (previous character)
        ////Serial.println(currentCharacter);
        if (currentCharacter > 0) {
          currentCharacter--;
          lcd.print(alphabet[currentCharacter]);
          createCustomChar(2,invertedCharacters[alphabet[currentCharacter]]);
          //////Serial.println("Character UP");
        }
        delay(250);  // Delay to prevent rapid scrolling

      } else if (joyDown) {  //DOWN (next character)
        ////Serial.println(currentCharacter);
        if (currentCharacter < (alphabetSize - 1)) {
          currentCharacter++;  //increment character value
          lcd.print(alphabet[currentCharacter]);
          createCustomChar(2,invertedCharacters[alphabet[currentCharacter]]);
          //////Serial.println("Character DOWN");
        }
        delay(250);  // Delay to prevent rapid scrolling
      } else {
        if (millis() % 600 < 450) {
          lcd.print(alphabet[currentCharacter]);
        } else {
          
          lcd.write(2);
        }
      }

      // Check if the joystick is moved left (backspace) or right (add space)
      if (joyLeft) {
        // LEFT (backspace)
        if (text.length() > 0) {
          text.remove(text.length() - 1);
          lcd.setCursor(0, 0);
          lcd.print(MENU_CLEAR);  //clear and reprint the string so characters dont hang
          lcd.setCursor(1, 0);
          lcd.print(text);
        }
        delay(250);  // Delay to prevent rapid multiple presses

      } else if (joyRight) {  //RIGHT adds a space or character to the label
        if (text.length() < 14){
          if (currentCharacter == 0) {
            text += ' ';  //add a space if the character is _
          } else {
            text += alphabet[currentCharacter];  //add the current character to the text
            createCustomChar(2,inverted_);
            currentCharacter = 0;
          }
        }
        delay(250);  // Delay to prevent rapid multiple presses
      }

      if (button1.isPressed()) {
        // Single click: Add character and reset alphabet scroll
        if (currentCharacter == 0) {
          text += ' ';  //add a space if the character is _
        } else {
          text += alphabet[currentCharacter];  //add the current character to the text
          currentCharacter = 0;                // reset for the next character
        }
        lcd.clear();
        currentState = PrintConfirmation;
        prevState = Editing;
      }

      break;

    case PrintConfirmation:
      // Print confirmation mode
      if (prevState == Editing) {
        lcd.setCursor(0, 0);    //move cursor to the first line
        lcd.print(PRINT_CONF);  //print menu text
        lcd.setCursor(0, 1);    // move cursor to the second line
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        prevState = PrintConfirmation;
      }

      //the following two if statements help move the blinking cursor from one option to the other.
      if (joyLeft) {  //left
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        delay(200);
      } else if (joyRight) {  //right
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(10, 1);
        cursorPosition = 10;
        delay(200);
      }

      lcd.setCursor(cursorPosition, 1);

      if (millis() % 600 < 400) {  // Blink every 500 ms
        lcd.print(">");
      } else {
        lcd.print(" ");
      }

      if (button1.isPressed()) {    //handles clicking options in print confirmation
        if (cursorPosition == 2) {  //proceed to printing if clicking yes
          lcd.clear();
          currentState = Printing;
          prevState = PrintConfirmation;

        } else if (cursorPosition == 10) {  //return to editing if you click no
          lcd.clear();
          currentState = Editing;
          prevState = PrintConfirmation;
        }
      }

      break;

    case Printing:
      // Printing mode
      if (prevState == PrintConfirmation) {
        lcd.setCursor(0, 0);
        lcd.print(PRINTING);  //update screen
        initPrintingProgress();
        totalVectorDrawing = text.length() * 14;
      }

      // ----------------------------------------------- plot text
      plotText(text, xpos, ypos);

      line(xpos + space, 0, 0);  // move to new line
      xpos = 0;
      ypos = 0;

      text = "";
      yStepper.step(-2250);
      releaseMotors();
      lcd.clear();
      currentState = Editing;
      prevState = Printing;

      break;
  }
}


void plotText(String &str, int x, int y) {  //takes in our label as a string, and breaks it up by character for plotting
  int pos = 0;
  ////Serial.println("plot string");
  ////Serial.println(str);
  for (int i = 0; i < str.length(); i++) {  //for each letter in the string (expressed as "while i is less than string length")
    char c = char(str.charAt(i));           //store the next character to plot on it's own
    //currentCharDrawing = i;
    if (byte(c) != 195) {
      if (c == ' ') {  //if it's a space, add a space.
        pos += space;
      } else {
        plotCharacter(c, x + pos, y);
        pos += space;  //scale is multiplied by 4 here to convert it to steps (because it normally get's multiplied by a coordinate with a max of 4)
        if (c == 'I' || c == 'i') pos -= (scale * 4) / 1.1;
        if (c == ',') pos -= (scale * 4) / 1.2;
      }
    }
  }
  ////Serial.println();
  releaseMotors();
}

void plotCharacter(char c, int x, int y) {  //this receives info from plotText for which character to plot,
  // first it does some logic to make specific tweaks depending on the character, so some characters need more space, others less,
  // and some we even want to swap (in the case of space, we're swapping _ (underscore) and space so that we have something to show on the screen)

  // and once we've got it all worked out right, this function passes the coordinates from that character though line() function to draw it

  ////Serial.print(uint8_t(c));  //print the received character to monitor
  ////Serial.print(">");

  //the following if statements handle character specific changes by shifting / swapping prior to drawing
  uint8_t character = 38;
  if (uint8_t(c) > 64 and uint8_t(c) < 91) {  //A...Z
    character = uint8_t(c) - 65;
  }
  if (uint8_t(c) > 96 and uint8_t(c) < 123) {  //A...Z
    character = uint8_t(c) - 97;
  }
  if (uint8_t(c) > 47 and uint8_t(c) < 58) {  //0...9
    character = uint8_t(c) - 22;
  }
  if (uint8_t(c) == 164 || uint8_t(c) == 132) {  //ä,Ä
    character = 39;
  }
  if (uint8_t(c) == 182 || uint8_t(c) == 150) {  //ö,Ö
    character = 40;
  }
  if (uint8_t(c) == 188 || uint8_t(c) == 156) {  //ü,Ü
    character = 41;
  }
  if (uint8_t(c) == 44) {  // ,
    character = 42;
  }
  if (uint8_t(c) == 45) {  // -
    character = 43;
  }
  if (uint8_t(c) == 46) {  // .
    character = 44;
  }
  if (uint8_t(c) == 33) {  // !
    character = 45;
  }
  if (uint8_t(c) == 63) {  // ?
    character = 46;
  }

  if (uint8_t(c) == 123) { /*{ ß*/
    character = 47;
  }
  if (uint8_t(c) == 39) { /*'*/
    character = 48;
  }
  if (uint8_t(c) == 38) { /*&*/
    character = 49;
  }
  if (uint8_t(c) == 43) { /*+*/
    character = 50;
  }
  if (uint8_t(c) == 58) { /*:*/
    character = 51;
  }
  if (uint8_t(c) == 59) { /*;*/
    character = 52;
  }
  if (uint8_t(c) == 34) { /*"*/
    character = 53;
  }
  if (uint8_t(c) == 35) { /*#*/
    character = 54;
  }
  if (uint8_t(c) == 40) { /*(*/
    character = 55;
  }
  if (uint8_t(c) == 41) { /*)*/
    character = 56;
  }
  if (uint8_t(c) == 61) { /*=*/
    character = 57;
  }
  if (uint8_t(c) == 64) { /*@*/
    character = 58;
  }
  if (uint8_t(c) == 42) { /***/
    character = 59;
  }
  if (uint8_t(c) == 125) { /*} Smiley*/
    character = 60;
  }
  if (uint8_t(c) == 126) { /*~ Open mouth Smiley*/
    character = 61;
  }
  if (uint8_t(c) == 36) { /*$ Heart*/
    character = 62;
  }
  ////Serial.print("letter: ");
  ////Serial.println(c);

  for (int i = 0; i < 14; i++) {  // go through each vector of the character
    int v = pgm_read_byte(&vector[character][i]);
    if (v == 200) {  // no more vectors in this array
      //Serial.println("now");
      break;
    }
    if (v == 222) {  // plot single point
      plot(true);
      delay(50);
      plot(false);
      
    } else {
      int draw = 0;
      if (v > 99) {
        draw = 1;
        v -= 100;
      }
      int cx = v / 10;       // get y ...
      int cy = v - cx * 10;  // and x

      //if(cx > 0) cx = 1;

      // 1: Normalize
      int x_start = x;
      int x_end = x + cx * x_scale;
      int y_start = y;
      int y_end = y + cy * y_scale * 3.5;  //we multiply by 3.5 here to equalize the Y output to match X,
      //this is because the Y lead screw covers less distance per-step than the X motor wheel (about 3.5 times less haha)
      bool switched = false;

      ////Serial.print("Scale: ");
      ////Serial.print(scale);
      ////Serial.print("  ");
      ////Serial.print("X Goal: ");
      ////Serial.print(x_end);
      ////Serial.print("  ");
      ////Serial.print("Y Goal: ");
      ////Serial.print(y_end);
      ////Serial.print("  ");
      ////Serial.print("Draw: ");
      ////Serial.println(draw);

      line(x_end, y_end, draw);
      
    }
    currentVectorDrawing ++;
    printingProgress();
  }
}

void line(int newx, int newy, bool drawing) {
  //this function is an implementation of bresenhams line algorithm
  //this algorithm basically finds the slope between any two points, allowing us to figure out how many steps each motor should do to move smoothly to the target
  //in order to do this, we give this function our next X (newx) and Y (newy) coordinates, and whether the pen should be up or down (drawing)

  if (drawing < 2) {  //checks if we should be drawing and puts the pen up or down based on that.
    plot(drawing);    // dashed: 0= don't draw / 1=draw / 2... = draw dashed with variable dash width
  } else {
    plot((stepCount / drawing) % 2);  //can do dashed lines, but for now this isn't doing anything since we're only sending 0 or 1.
  }

  int i;
  long over = 0;

  long dx = newx - xpos;  //calculate the difference between where we are (xpos) and where we want to be (newx)
  long dy = newy - ypos;
  int dirx = dx > 0 ? -1 : 1;  //this is called a ternary operator, it's basically saying: if dx is greater than 0, then dirx = -1, if dx is less than or equal to 0, dirx = 1.
  int diry = dy > 0 ? 1 : -1;  //this is called a ternary operator, it's basically saying: if dy is greater than 0, then diry = 1, if dy is less than or equal to 0, diry = -1.
  //the reason one of these ^ is inverted logic (1/-1) is due to the direction these motors rotate in the system.

  dx = abs(dx);  //normalize the dx/dy values so that they are positive.
  dy = abs(dy);  //abs() is taking the "absolute value" - basically it removes the negative sign from negative numbers

  //the following nested If statements check which change is greater, and use that to determine which coordinate (x or y) get's treated as the rise or the run in the slope calculation
  //we have to do this because technically bresenhams only works for the positive quandrant of the cartesian coordinate grid,
  // so we are just flipping the values around to get the line moving in the correct direction relative to it's current position (instead of just up an to the right)
  if (dx > dy) {
    over = dx / 2;
    for (i = 0; i < dx; i++) {  //for however much our current position differs from the target,
      xStepper.step(dirx);      //do a step in that direction (remember, dirx is always going to be either 1 or -1 from the ternary operator above)

      // ////Serial.print("Xsteps: ");
      // ////Serial.print(dirx);
      // ////Serial.print("  ");

      over += dy;
      if (over >= dx) {
        over -= dx;

        // ////Serial.print("Ysteps: ");
        // ////Serial.println(diry);

        yStepper.step(diry);
      }
      //delay(1);
    }
  } else {
    over = dy / 2;
    for (i = 0; i < dy; i++) {
      yStepper.step(diry);
      // ////Serial.print("Ysteps: ");
      // ////Serial.print(diry);
      // ////Serial.print("  ");
      over += dx;
      if (over >= dy) {
        over -= dy;
        // ////Serial.print("Xsteps: ");
        // ////Serial.println(dirx);
        xStepper.step(dirx);
      }
      //delay(1);
    }
  }
  xpos = newx;  //store positions
  ypos = newy;  //store positions
}


void plot(boolean penOnPaper) {  //used to handle lifting or lowering the pen on to the tape
  if (penOnPaper) {              //if the pen is already up, put it down
    angle = 80;
  } else {  //if down, then lift up.
    angle = 25;
  }
  servo.write(angle);                        //actuate the servo to either position.
  if (penOnPaper != pPenOnPaper) delay(50);  //gives the servo time to move before jumping into the next action
  pPenOnPaper = penOnPaper;                  //store the previous state.
}

void penUp() {  //singular command to lift the pen up
  servo.write(25);
}

void penDown() {  //singular command to put the pen down
  servo.write(80);
}

void releaseMotors() {
  for (int i = 0; i < 4; i++) {  //deactivates all the motor coils
    digitalWrite(xPins[i], 0);   //just picks each motor pin and send 0 voltage
    digitalWrite(yPins[i], 0);
  }
  plot(false);
}

void homeYAxis() {
  lcd.setCursor(4, 1);
  lcd.print("Loading");

  int i = 0;
  int b = 0;
  int d = 0;
  while (d < 16){
    lcd.setCursor(d,0);
    lcd.write(0);
    d++;
  }
  while (b < 100){
    delay(LOADING_SPEED);
    yStepper.step(-30);  //lowers the pen holder to it's lowest position.
    b ++;
    
    if (i == 0){
      lcd.setCursor(15,0);
    } else {
      lcd.setCursor(i-1,0);
    }
    lcd.write(0);
    lcd.setCursor(i,0);
    lcd.write(1);
    i++;
    if (i>15){
      i=0;
    }

  }
}

void resetScreen() {
  lcd.clear();          // clear LCD
  lcd.setCursor(0, 0);  // set cursor to row 0 column 0
  lcd.print(": ");
  lcd.setCursor(1, 0);  //move cursor down to row 1 column 0
  cursorPosition = 1;
}

byte* getInvertLetter(char input){
  return (const byte*)pgm_read_word(&invertedCharacters[(uint8_t)input]);
}



void createCustomChar(int index, const byte* pgmData) {
    byte charData[8];  // LCD characters are 8 bytes each (assuming 5x8 font)
    for (int i = 0; i < 8; i++) {
        charData[i] = pgm_read_byte(&pgmData[i]);  // Read data from PROGMEM correctly
    }
    lcd.createChar(index, charData);  // Create custom character on LCD
}

void initPrintingProgress(){
  lcd.clear();
  createCustomChar(0,home1);
  createCustomChar(1,twenty);
  createCustomChar(2,fourty);
  createCustomChar(3,sixty);
  createCustomChar(4,eighty);

}

void printingProgress() {
  // Ensure proper percentage calculation
  float drawingPrecentage = (currentVectorDrawing / (float)totalVectorDrawing) * 100;

  // Convert percentage to char array (avoiding String)
  char dp[6];
  dtostrf(drawingPrecentage, 4, 1, dp);

  // Display progress bar
  lcd.setCursor(0, 0);
  for (int z = 1; z <= 16; z++) {
    float threshold = 6.25 * z;

    if (drawingPrecentage >= threshold) {
      lcd.write(0);  // Full block (home1)
    } else if (drawingPrecentage >= threshold - 1.25) {
      lcd.write(4);  // 80% filled
    } else if (drawingPrecentage >= threshold - 2.5) {
      lcd.write(3);  // 60% filled
    } else if (drawingPrecentage >= threshold - 3.75) {
      lcd.write(2);  // 40% filled
    } else if (drawingPrecentage >= threshold - 5) {
      lcd.write(1);  // 20% filled
    } else {
      lcd.print(" ");  // Empty space
    }
  }

  // Display percentage value
  lcd.setCursor(6, 1);  // Adjust position as needed
  lcd.print(dp);
  lcd.print("%");
}
