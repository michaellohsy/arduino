
// References
// Clock (RTCLib) https://create.arduino.cc/projecthub/Tittiamo/clock-set-date-time-0d46a4
// Alarm Clock https://create.arduino.cc/projecthub/Tittiamo/alarm-clock-f61bad
// LCD https://learn.adafruit.com/adafruit-arduino-lesson-11-lcd-displays-1
// LCD https://learn.adafruit.com/adafruit-arduino-lesson-12-lcd-displays-part-2
// Buzzer (pitch) https://www.arduino.cc/en/Tutorial/ToneMelody

// includes
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include "pitches.h"

// initialize LCD
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// intialize RTC
RTC_DS3231 RTC;
String displayMonths []= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// 3 pins
// pin1 switch mode
// pin2 change switch type
// pin3 increment type
int p1 = 4;
int p2 = 5;
int p3 = 6;
bool p1_pressed = false;
bool p2_pressed = false;
bool p3_pressed = false;

int buzzer = 2;
int buzzer_period = 50;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int buzzer_sum = 0;

int DISPLAY_MODE = 1;
int ADJUST_MODE = 2;
int ALARM_MODE = 3;
int CURRENT_MODE = DISPLAY_MODE;

int flash_idx = 1;
bool flash_flag = false;
int flash_counter = 0;

int alarm_hour = 15;
int alarm_minute = 55;
int alarm_second = 0;
bool alarm_on = true;
bool alarm_triggered = false;

void setup() {

  Serial.begin(57600);
  Wire.begin();
  
  lcd.begin(16, 2);

  pinMode(p1,INPUT_PULLUP);
  pinMode(p2,INPUT_PULLUP);
  pinMode(p3,INPUT_PULLUP);

  //pinMode(buzzer,OUTPUT);
  
}

String getDisplayTime(int flash_idx) {
  
  DateTime now = RTC.now();
  int hour;
  int minute;
  int second;

  if(CURRENT_MODE == ALARM_MODE) {
    hour = alarm_hour;
  } else {
    hour = now.hour();
  }

  if(CURRENT_MODE == ALARM_MODE) {
    minute = alarm_minute;
  } else {
    minute = now.minute();
  }

  if(CURRENT_MODE == ALARM_MODE) {
    second = alarm_second;
  } else {
    second = now.second();
  }
  
  String time;
  if(flash_idx == 4){
    time.concat("  ");
  } else {
    if(hour <= 9) {
      time.concat("0");
    }
    time.concat(hour);
  }
  time.concat(":");

  if(flash_idx == 5) {
    time.concat("  ");
  } else {
    if(minute <= 9) {
      time.concat("0");
    }
    time.concat(minute);
  }
  time.concat(":");

  if(flash_idx == 6){
    time.concat("  ");
  } else {
    if(second <= 9) {
      time.concat("0");
    }
    time.concat(second);
  }

  return time;
}

String getDisplayDate(int flash_idx) {

  DateTime now = RTC.now();
  String date;

  if(flash_idx == 1) {
    date.concat("  ");
  } else {
    if(now.day() <= 9) {
      date.concat("0");
    }
    date.concat(now.day());
  }
  date.concat(" ");

  if(flash_idx == 2) {
    date.concat("   ");
  } else {
    date.concat(displayMonths[now.month()-1]);
  }
  date.concat(" ");

  if(flash_idx == 3) {
    date.concat("    ");
  } else {
    date.concat(now.year());
  }

  //Serial.println(date);
  
  return date;
}

void updateDateTime(int flash_idx) {

  Serial.println("Update time");

  DateTime now = RTC.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  if(flash_idx == 1) {
    day = day + 1;
    if(day > 31) {
      day = 1;
    }
  } else if (flash_idx == 2) {
    month = month + 1;
    if(month > 12) {
      month = 1;
    }
  } else if (flash_idx == 3) {
    year = year + 1;
    if(year > 2100) {
      year = 2018;
    }
  } else if (flash_idx == 4) {
    hour = hour + 1;
    if(hour > 24) {
      hour = 0;
    }
  } else if (flash_idx == 5) {
    minute = minute + 1;
    if(minute > 60) {
      minute = 0;
    }
  } else if (flash_idx == 6) {
    second = second + 1;
    if(second > 60) {
      second = 0;
    }
  }

  String datetime;
  datetime.concat(day);
  datetime.concat(" ");
  datetime.concat(month);
  datetime.concat(" ");
  datetime.concat(year);
  datetime.concat(" ");
  datetime.concat(hour);
  datetime.concat(" ");
  datetime.concat(minute);
  datetime.concat(" ");
  datetime.concat(second);

  Serial.println(datetime);
  RTC.adjust(DateTime(year,month,day,hour,minute,second));
}

void checkAlarm() {
  if(!alarm_on) {
    alarm_triggered = false;
  } else {
    DateTime now = RTC.now();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();

    if(!alarm_triggered) {
      alarm_triggered = hour == alarm_hour && minute == alarm_minute && second == alarm_second;
      if(alarm_triggered) {
        buzzer_sum = 0;
      }
    }
  }
}

void loop() {

  flash_counter = flash_counter + 1;
  if(flash_counter > 100) {
    flash_flag = !flash_flag;
    flash_counter = 0;
  }
  
  checkAlarm();
  
  if(alarm_triggered) {

//    buzzer_sum = buzzer_sum + 1;
//    if(buzzer_sum > (sizeof(melody) + 1) * buzzer_period) {
//      buzzer_sum = 0;
//    }
    
//    Serial.println("Alarm triggered");

    DateTime now = RTC.now();
    int minute = now.minute();
    int second = now.second();

    // stop alarm on any button press
    if(digitalRead(p1) == LOW) {
      Serial.println("Button 1 stop alarm.");
      alarm_triggered = false;
      delay(500);
    } else if(digitalRead(p2) == LOW) {
      Serial.println("Button 2 stop alarm.");
      alarm_triggered = false;
      delay(500);
    } else if(digitalRead(p3) == LOW) {
      Serial.println("Button 3 stop alarm.");
      alarm_triggered = false;
      delay(500);
    } else if(minute == alarm_minute + 1 && second == alarm_second) { // stop alarm after 1 minute
      Serial.println("Auto stop after 1 minute.");
      alarm_triggered = false;
    }

    lcd.setCursor(0,0);
    if(flash_flag) {
      lcd.print("               "); 
    } else {
      lcd.print("WAKE UP         ");
    }
    lcd.setCursor(0,1);
    lcd.print(getDisplayTime(0));

//    int note_idx = buzzer_sum/buzzer_period;
//    bool pause = (buzzer_sum % buzzer_period) > (buzzer_period * 0.67) || buzzer_sum > (sizeof(melody) * buzzer_period); // short delay before each tone or after the whole melody
//    Serial.println(buzzer_sum);
//    Serial.print(" ");
//    Serial.print(note_idx);
//    Serial.print(" ");
//    Serial.println(pause);
    
//    if(!pause) {
//      noTone(buzzer);
//    } else {
      tone(buzzer, melody[0]);
//    }

//    // iterate over the notes of the melody:
//    for (int thisNote = 0; thisNote < 8; thisNote++) {
//    
//      // to calculate the note duration, take one second divided by the note type.
//      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//      int noteDuration = 1000 / noteDurations[thisNote];
//      tone(8, melody[thisNote], noteDuration);
//    
//      // to distinguish the notes, set a minimum time between them.
//      // the note's duration + 30% seems to work well:
//      int pauseBetweenNotes = noteDuration * 1.30;
//      delay(pauseBetweenNotes);
//      // stop the tone playing:
//      noTone(8);
//    }
    
    return;
  }

  noTone(buzzer);

  if(digitalRead(p1)  == LOW) {
    if(!p1_pressed) {
      p1_pressed = true;
      Serial.println("Button 1 pressed");
      CURRENT_MODE = CURRENT_MODE + 1;
      if(CURRENT_MODE > ALARM_MODE) {
        CURRENT_MODE = DISPLAY_MODE;
      }
      if(CURRENT_MODE == ADJUST_MODE) {
        flash_idx = 1;
      } else if (CURRENT_MODE == ALARM_MODE) {
        flash_idx = 4;
      }
      Serial.print("Mode: ");
      Serial.println(CURRENT_MODE); 
    }
  } else {
    p1_pressed = false;
  }

  if(digitalRead(p3)  == LOW) {
    if(!p3_pressed) {
      p3_pressed = true;
      Serial.println("Button 3 pressed");
    }
  } else {
    p3_pressed = false;
  }
  
  if(digitalRead(p2)  == LOW) {
    if(!p2_pressed) {
      p2_pressed = true;
      Serial.println("Button 2 pressed");

      if(p2_pressed && p3_pressed) {
        Serial.println("Turn alarm on off");
        alarm_on = !alarm_on;
        delay(200);
      } else {
        flash_idx = flash_idx + 1;
        if(CURRENT_MODE == ADJUST_MODE) {
          if(flash_idx > 6) {
            flash_idx =  1;
          }
        } else if (CURRENT_MODE == ALARM_MODE) {
          if(flash_idx > 6) {
            flash_idx = 4;
          }
        }
        Serial.print("Edit index: "); 
        Serial.println(flash_idx); 
      }
    }
  } else {
    p2_pressed = false;
  }

  if(CURRENT_MODE == DISPLAY_MODE) {
    lcd.setCursor(0,0);
    lcd.print(getDisplayDate(0));
    lcd.setCursor(0,1);
    lcd.print(getDisplayTime(0));
  } else if (CURRENT_MODE == ADJUST_MODE) {
   
    if(!flash_flag) {
      lcd.setCursor(0,0);
      lcd.print(getDisplayDate(0));
      lcd.setCursor(0,1);
      lcd.print(getDisplayTime(0));
    } else { 
      lcd.setCursor(0,0);
      lcd.print(getDisplayDate(flash_idx));
      lcd.setCursor(0,1);
      lcd.print(getDisplayTime(flash_idx));
    }

    if(p3_pressed) {
      updateDateTime(flash_idx);
      p3_pressed = false;
      delay(200);
    }

  } else if (CURRENT_MODE == ALARM_MODE) {

    lcd.setCursor(0,0);
    lcd.print("Alarm           ");

    if(alarm_on) {
      if(!flash_flag) {
        lcd.setCursor(0,1);
        lcd.print(getDisplayTime(0));
      } else {
        lcd.setCursor(0,1);
        lcd.print(getDisplayTime(flash_idx));
      }  
    } else {
      lcd.setCursor(0,1);
      lcd.print("Off             ");
    }
    
    if(alarm_on && p3_pressed) {
      if(flash_idx == 4) {
        alarm_hour = alarm_hour + 1;
        if(alarm_hour > 24) {
          alarm_hour = 0;
        }
      } else if (flash_idx == 5) {
        alarm_minute = alarm_minute + 1;
        if(alarm_minute > 60) {
          alarm_minute = 0;
        }
      } else if (flash_idx == 6) {
        alarm_second = alarm_second + 1;
        if(alarm_second > 60) {
          alarm_second = 0;
        }
      }
      
      Serial.print("Alarm: ");
      Serial.print(alarm_hour);
      Serial.print(":");
      Serial.print(alarm_minute);
      Serial.print(":");
      Serial.println(alarm_second);
      
      p3_pressed = false;
      delay(200);
    }
  }
  
}

