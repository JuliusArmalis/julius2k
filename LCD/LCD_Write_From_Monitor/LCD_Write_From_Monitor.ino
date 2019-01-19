/*
 * Reads text from serial monitor and prints it to lcd
 * Currently I'm using 16*2 lcd, but later I might make the code more universal
 * I've attached a button to clear the screen at any time, but
 * I haven't found a way to make it as interrupt signal. Therefore,
 * right now I'm just using a button clear and read monitor 
 * functions in every loop
 */

#include <TimerOne.h>

#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>

#include <Wire.h>

#define Button 2
const uint8_t m = 16;  //  LCD number of collumns
const uint8_t n = 2;   //  LCD number of rows

int buttonPushCount = 0;
int buttonState = 0;
int lastButtonState = 0;

//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

String lines[n];
uint8_t maxLength = 0;
uint8_t maxI = 0;

unsigned long  prevTime = 0;

void setup() 
{

  lcd.begin(m, n);
  pinMode(Button, INPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(Button), ButtonClear, FALLING);
  Nullify();
}

void loop() 
{  
  unsigned long currTime; 
  
  if(maxLength == 0)
    lcd.clear();
  
  ReadMonitor();

  if(maxLength > m && (currTime = millis()) - prevTime >= 2000)
  {  
    Scroll();
    prevTime = millis();
  }
  
}

void ReadMonitor()
{
  if(Serial.available())
  {
    for(int i = n - 1; i > 0; i--)  //  Pushes all lines down by one
        lines[i] = lines[i-1];
        
    lines[0] = Serial.readString();
    lines[0] = lines[0].substring(0, lines[0].length() - 1);    

    lcd.home();
    
    lcd.clear();

    for(int i = 0; i < n; i++)  //  Prints all lines
    {
      lcd.setCursor(0,i);
      lcd.print(lines[i]);
    }

    prevTime = millis();  //The time is calculated from this point on

    findLongest();
  }
}

void findLongest()
{
  maxLength = 0;
  
  for(int i = 0; i < n; i++)
  {
    if(lines[i].length() > maxLength)
    {
      maxLength = lines[i].length();
      maxI = i;      
    }
  }
}

void ButtonClear()
{
  static unsigned long prevTime = 0;
  unsigned long interrTime = millis();
  if(interrTime - prevTime > 150)
  {
    Nullify();
    maxLength = 0;
    maxI = 0;
  }

  prevTime = interrTime;  
}

void Nullify()  //  Nullifies all lines in an array
{
  for(int i = 0; i < n; i++)
      lines[i] = "";
}

void Scroll()
{
  unsigned long currTime;
  unsigned long Time;
  for(int i = 0; i < int(lines[maxI].length() - m); i++)
  {
    lcd.scrollDisplayLeft();
    Time = millis();
    while((currTime = millis()) - Time < 200)
    {
    }
  }

  Time = millis();
  while((currTime = millis()) - Time < 1500)
  {
  } 
  
  lcd.home(); 
  
}
