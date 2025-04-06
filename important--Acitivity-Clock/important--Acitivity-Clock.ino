#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <Time.h>

//RTC Variables
#define DS1307_ADDRESS 0x68
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}
//Clock Variables
int currentSecond;
int currentMinute;
int currentHour;
int currentDay;

int firstHour = 17;
int firstMinute = 0;

int secondHour = 13;
int secondMinute = 0;

bool lastHour = false;
int lcdCenter = 0;
int displayHour;

int alarmDay;

//Timer Variables
bool timer = false;
int prevSec;
int timeMin;
int timeSec;
int timeDurMin = 30;

//State Control Variables

bool timeStart = false;

LiquidCrystal lcd(7,8,9,10,11,12);

void setup() {
  pinMode(13, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  lcd.begin(16,2);
  setTime(14, 59, 58, 1, 4, 2025); // Set time to 12:00:00 January 1, 2024
  RTC.set(now()); // Set the RTC to the time just set
  lcd.clear();
}

void loop() {

  time_t t = now();
  currentDay = day(t);
  currentMinute = minute(t);
  currentHour = hour(t);
  currentSecond = second(t);

  if (currentHour > 12 ){
   displayHour = currentHour-12;
  }

  switch(timer){
    case false:
    if (currentHour > 12) {
      displayHour = currentHour - 12;
    } else if (currentHour == 0) {
      displayHour = 12;
    } else {
      displayHour = currentHour;
    }
    
      lcdCenter = 0;
      lcdCenter = String(displayHour).length() + String(currentMinute).length() + String(currentSecond).length() + 2;
      lcdCenter = (16-lcdCenter)/2;
      lcd.clear();
      if(currentHour > 12){
        lcd.setCursor(lcdCenter,1);
      } else {
        lcd.setCursor(lcdCenter,0);
      }
    
    lcd.print(displayHour);
    lcd.print(":");
    lcd.print(currentMinute);
    lcd.print(":");
    lcd.print(currentSecond);
    
        if(digitalRead(13) == LOW){
          if(currentHour == firstHour && currentMinute == firstMinute){
            
              timeStart = false;
              timer = true;
            
          } else if (currentHour == secondHour && currentMinute == firstMinute){
            
              timeStart = false;
              timer = true;
            
          }
        }

    delay(1000);
    break;
    case true:
      switch(timeStart){
        case false:
          timeMin = timeDurMin;
          timeSec = 0;
          prevSec = currentSecond;
          tone(3, 500);
          delay(500);
          noTone(3);
          delay(500);
          tone(3, 500);
          delay(500);
          noTone(3);
          delay(500);
          tone(3, 500);
          delay(500);
          noTone(3);
          timeStart = true;
        break;
        case true:
            if(prevSec != currentSecond){
            prevSec = currentSecond;
            if (timeSec == 0){
              timeSec = 60;
              timeMin --;
            }
            timeSec --;
            lcd.clear();
            lcd.setCursor(6,1);
            lcd.print(timeMin);
            lcd.print(":");
            lcd.print(timeSec);

            if (timeMin == 0 && timeSec == 0){
              tone(3, 500);
              delay(500);
              noTone(3);
              delay(500);
              tone(3, 500);
              delay(500);
              noTone(3);
              delay(500);
              tone(3, 500);
              delay(500);
              noTone(3);
              timer = false;
            }
          }
          delay(100);
        break;
      }

    break;
  }
}