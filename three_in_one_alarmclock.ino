#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <dht11.h>
#include <IRremote.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RTC_DS3231 rtc;
bool century = false;
dht11 DHT11;
const int DHT11PIN = 6;
const int buzzerPin = 7;
const int buttonPin = 8;
const int RECV_PIN = 9;
IRrecv irrecv(RECV_PIN);
decode_results results;

int buttonState = LOW;
int ledState = -1;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 100;
int counter = 0;
int hour = 0;
int minute = 0;
int times[3] = {0, 0, 0};
boolean alarmPlaying = false;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  lcd.begin(16, 2);
  if (!rtc.begin()){
    lcd.print("Couldn't Find RTC");
  }
  
  if (rtc.lostPower()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
   //sample the state of the button - is it pressed or not?

  updateTime();
  playAlarm();
  buttonState = digitalRead(buttonPin);

  //filter out any noise by setting a time buffer
  if ( (millis() - lastDebounceTime) > debounceDelay) {

    //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
    if ((buttonState == HIGH) && (ledState<0)) {
      counter++;
      lastDebounceTime = millis(); //set the current time
    }
    else if (buttonState==HIGH && ledState>0){
      ledState = -ledState;
      lastDebounceTime = millis();
    }
  }//close if(time buffer)
  
  Serial.println(counter);
  if (counter%4==0){
  showHumidity();
  debounceDelay = 200;
  }
  else if (counter%4==1){
    showTemp();
    debounceDelay = 200;
  }
  else if (counter%4==2){
    showTime();
    debounceDelay = 200;
  }
  else if (counter%4==3){
    setAlarm();
    debounceDelay = 200;
  }
}

void updateTime() {
  DateTime now = rtc.now();
  times[0] = now.hour();
  times[1] = now.minute();
  times[2] = now.second();
}
void showTime(){
  lcd.clear();
  lcd.print("Current Time: ");
  lcd.setCursor(0, 1);
  if (times[0]<10){
    lcd.print("0");
  }
  lcd.print(times[0]);
  lcd.print(":");
  if (times[1]<10){
    lcd.print("0");
  }
  lcd.print(times[1], DEC);
  lcd.print(":");
  if (times[2]<10){
    lcd.print("0");
  }
  lcd.print(times[2], DEC);
  delay(5);
}

void showTemp(){
  int chk = DHT11.read(DHT11PIN);
  lcd.clear();
  lcd.print("Temperature: ");
  lcd.setCursor(0, 1);
  lcd.print((float) DHT11.temperature, 2);
  lcd.print(" C"); 
}

void showHumidity(){
  int chk = DHT11.read(DHT11PIN);
  lcd.clear();
  lcd.print("Humidity: ");
  lcd.setCursor(0, 1);
  lcd.print((float) DHT11.humidity, 2);
  lcd.print("%");
}

void setAlarm() {
  lcd.clear();
  lcd.print("Input hour:");
  while (true){
    if (irrecv.decode(&results)){
      int value = results.value;
      if (value == 6375){
        hour++;
        if (hour==24){
          hour = 0;
        }
      }  
      irrecv.resume();    
    }
    lcd.clear();
    lcd.print("Set Alarm Time:");
    lcd.setCursor(0, 1);
    if (hour<10){
      lcd.print(0);
    }
    lcd.print(hour);
    lcd.print(":");
    if (minute<10){
      lcd.print(0);
    }
    lcd.print(minute);
    delay(100);

    if (irrecv.decode(&results)){
      int value = results.value;
      if (value == 14535){
        break;
      }
    }
  }
  lcd.clear();

  while (true) {
    if (irrecv.decode(&results)){
      int value = results.value;
      if (value == 6375){
        minute++;
        if (minute==60){
          minute = 0;
        }
      }  
      irrecv.resume();    
    }
    lcd.clear();
    lcd.print("Set Alarm Time:");
    lcd.setCursor(0,1);
    if (hour<10){
      lcd.print(0);
    }
    lcd.print(hour);
    lcd.print(":");
    if (minute<10){
      lcd.print(0);
    }
    lcd.print(minute);
    delay(100);

    if (irrecv.decode(&results)){
      int value = results.value;
      if (value == 14535){
        break;
      }
    }
  }
  lcd.clear();
  lcd.print("Alarm set for: ");  
  lcd.setCursor(0,1);
  if (hour<10){
    lcd.print(0);
  }
  lcd.print(hour);
  lcd.print(":");
  if (minute<10){
    lcd.print(0);
  }
  lcd.print(minute);
  delay(5000);
  counter+=1;
}


void playAlarm() {
  if(times[0]==hour && times[1]==minute){
    alarmPlaying = true;   
  }
  if (alarmPlaying){
    lcd.clear();
    lcd.print("WAKE UP");
    tone(buzzerPin, 1000); 
    delay(1000);        
    noTone(buzzerPin);     
    delay(1000);         
    while (true) {
      if (irrecv.decode(&results)) {
        int value = results.value;
        if (value == 14535){
          alarmPlaying = false;
          break;
        }
        else if (value == 6375){
          if (minute<55){
            minute+=5;
          }
          else{
            hour++;
            minute = 5 - (60-minute);
          }
          lcd.clear();
          lcd.print("Snoozed");
          delay(1000);
          break;
        }
      }
    }
  }
}

void printIn() {
  while (true) {
    if (irrecv.decode(&results)) {
      int value = results.value;
      lcd.clear();
      lcd.print(value);
      delay(100);
    }
  }
}
