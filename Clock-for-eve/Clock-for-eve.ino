#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include "avdweb_VirtualDelay.h"

#define ALARM 5
#define UP_ARROW 2
#define DOWN_ARROW 3
#define TIME_BUTTON 4
#define BUZZER 10

VirtualDelay alarmCycle1;
VirtualDelay alarmCycle2;

bool twentyFour = false;
bool alarmActive = false;
bool backlightEnabled = true;

unsigned long previousMillis = 0;
const long interval = 1000;
unsigned long currentMillis = millis();

int hourToSet;
int minuteToSet;
int timeChangeDelay = 300;

int alarmMinute = 0;
int alarmHour = 6;
bool alarmEnabled = false;

// Initialize RTC and LCD
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change address to 0x3F if needed

void setup() {
  // Start Serial Monitor for debugging

  // Initialize RTC
  if (!rtc.begin()) {
    while (1);
  }

  // Uncomment the next line to set the RTC to the current compile time
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.adjust(DateTime(2025,1,22 ,21  ,56,50));
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  pinMode(ALARM, INPUT_PULLUP);
  pinMode(UP_ARROW, INPUT_PULLUP);
  pinMode(DOWN_ARROW, INPUT_PULLUP);
  pinMode(TIME_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  currentMillis = millis(); 
  DateTime now = rtc.now();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  


    hourToSet = now.hour();
    minuteToSet = now.minute();

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.day());
    lcd.print('/');
    lcd.print(now.year());

    // Display Time on the second row
    if (twentyFour == false) lcd.setCursor((16-(String(now.twelveHour()).length() + String(now.minute()).length() + String(now.second()).length() + 2))/2, 0);
    else lcd.setCursor((16-(String(now.hour()).length() + String(now.minute()).length() + String(now.second()).length() + 2))/2, 0);
    if (twentyFour == false)  lcd.print(now.twelveHour());
    else lcd.print(now.hour());
    lcd.print(':');
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute());
    lcd.print(':');
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second());
  }

  if(digitalRead(ALARM) == LOW){
    alarmActive = !alarmActive;
    lcd.clear();
    if(alarmActive == false){
      lcd.setCursor(3,0);
      lcd.print("ALARM OFF");
    } else {
      lcd.setCursor(4,0);
      lcd.print("ALARM ON");
    }
    delay(300);
  } else if (digitalRead(TIME_BUTTON) == LOW){
    if (backlightEnabled == true){lcd.noBacklight(); backlightEnabled = false;}
    else{lcd.backlight(); backlightEnabled = true;}
    delay(1000);
  } else if (digitalRead(UP_ARROW) == LOW){
    if (alarmActive == false){
      minuteToSet ++;
      if (minuteToSet >= 60){
        minuteToSet = 0;
        hourToSet ++;
        if(hourToSet >= 24) hourToSet = 0;
      }
      rtc.adjust(DateTime(2025, 1, 12, hourToSet, minuteToSet, 0));
      delay(timeChangeDelay);
    } else {
      alarmMinute ++;
      if (alarmMinute >= 60){
        alarmMinute = 0;
        alarmHour ++;
        if(alarmHour >= 24) alarmHour = 0;
      }
      lcd.clear();
      lcd.setCursor(0 ,0);
      lcd.print("ALARM: ");
      lcd.print(alarmHour);
      lcd.print(":");
      lcd.print(alarmMinute);
      delay(timeChangeDelay);
      
    }
  } else if (digitalRead(DOWN_ARROW) == LOW){
    if(alarmActive == false){
      minuteToSet --;
      if (minuteToSet <= -1){
        minuteToSet = 59;
        hourToSet --;
        if(hourToSet < 0) hourToSet = 23;
      }
      rtc.adjust(DateTime(2025, 1, 12, hourToSet, minuteToSet, 0));
      delay(timeChangeDelay);
    } else {
      alarmMinute --;
      if (alarmMinute <= -1){
        alarmMinute = 59;
        alarmHour --;
        if(alarmHour < 0) alarmHour = 23;
      }
      lcd.clear();
      lcd.setCursor(0 ,0);
      lcd.print("ALARM: ");
      lcd.print(alarmHour);
      lcd.print(":");
      lcd.print(alarmMinute);
      delay(timeChangeDelay);
      
    }
  }
  if(alarmActive == true){
    if(now.minute() == alarmMinute && now.hour() == alarmHour && now.second() == 0) {
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print("ALARM !!!!!!!");
      alarmCycle1.start(250);
      while (digitalRead(ALARM) == HIGH){
        if (alarmCycle1.elapsed()){
          tone(BUZZER, 1000);
          alarmCycle2.start(250);
          lcd.backlight();
        }
        if(alarmCycle2.elapsed()){
          noTone(BUZZER);
          alarmCycle1.start(250);
          lcd.noBacklight();
        }
      }
      if (backlightEnabled == true) lcd.backlight();
      else lcd.noBacklight();
      noTone(BUZZER);
      delay(500);
    }
  }
}