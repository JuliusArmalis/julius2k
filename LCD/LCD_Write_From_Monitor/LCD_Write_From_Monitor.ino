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

int Button = 2;
int buttonPushCount = 0;
int buttonState = 0;
int lastButtonState = 0;

//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

String line1 = "";
String line2 = "";

unsigned long  prevTime = 0;

void setup() 
{

  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  pinMode(Button, INPUT);
  Serial.begin(9600);
  //attachInterrupt(0, ButtonClear, CHANGE);
  //Timer1.initialize(100000);
  //Timer1.attachInterrupt(ButtonPush);
}

void loop() 
{  
  unsigned long currTime; 
  ButtonClear();
  
  ReadMonitor();
  
  if((line1.length() > 16 || line2.length() > 16) && (currTime = millis()) - prevTime >= 2000)
  {    
    Scroll(currTime);
    prevTime = millis();
  }
  
}

void ReadMonitor()
{
  if(Serial.available())
  {   
    line2 = line1;
    line1 = Serial.readString();
    line1 = line1.substring(0, line1.length() - 1);

    lcd.home();
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);

    prevTime = millis();  //The time is calculated from this point on
  }
}

void ButtonClear()
{
  buttonState = digitalRead(Button);
  
  if(buttonState != lastButtonState)
    {
      buttonPushCount++;
      line1 = "";
      line2 = "";
      lcd.clear();
      delay(100);   
    }
        
    lastButtonState = buttonState;  
}

void Scroll(unsigned long Time)
{
  unsigned long currTime;

  if(line1.length() >= line2.length())
   {
    for(int i = 0; i < int(line1.length() - 16); i++)
    {
      lcd.scrollDisplayLeft();
      Time = millis();
      while((currTime = millis()) - Time < 200)
      {
        ButtonClear();
        ReadMonitor();
      }
    }   
   }

  else
  {
    for(int i = 0; i < int(line2.length() - 16); i++)
      {
        lcd.scrollDisplayLeft();
        Time = millis();
        while((currTime = millis()) - Time < 200)
        {
          ButtonClear();
          ReadMonitor();
        }
      }   
  }
  
  Time = millis();
  while((currTime = millis()) - Time < 1500)
  {
    ButtonClear();
    ReadMonitor();
  }
  
  lcd.home(); 
  
}
