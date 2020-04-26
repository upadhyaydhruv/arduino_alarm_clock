#include <LiquidCrystal.h>;
#include <Wire.h>;
#include "RTClib.h";
#include <dht11.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RTC_DS3231 rtc;
bool century = false;
dht11 DHT11;
const int DHT11PIN = 6;
const int buttonPin = 7;

int counter;
int buttonState = 0;

void setup() {
  lcd.begin(16, 2);
  if (!rtc.begin()){
    lcd.print("Couldn't Find RTC");
  }
  
  if (rtc.lostPower()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  pinMode(buttonPin, INPUT);
  counter = 0;
}

void loop() {
  buttonState = digitalRead(buttonPin);
  delay(100);
  if (buttonState == HIGH){
    counter++;
  }
  if (counter%3==0){
  showHumidity();
  }
  else if (counter%3==1){
    showTemp();
  }
  else if (counter%3==2){
    showTime();
  }
}

void showTime(){
  lcd.clear();
  lcd.print("Current Time: ");
  lcd.setCursor(0, 1);
  DateTime now = rtc.now();
  if (now.hour()<10){
    lcd.print("0");
  }
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute()<10){
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  if (now.second()<10){
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);
  delay(1000);
}

void showTemp(){
  int chk = DHT11.read(DHT11PIN);
  lcd.clear();
  lcd.print("Temperature: ");
  lcd.setCursor(0, 1);
  lcd.print((float) DHT11.temperature, 2);
  lcd.print(" C");
  delay(2000);  
}

void showHumidity(){
  int chk = DHT11.read(DHT11PIN);
  lcd.clear();
  lcd.print("Humidity: ");
  lcd.setCursor(0, 1);
  lcd.print((float) DHT11.humidity, 2);
  lcd.print("%");
  delay(2000);
}

