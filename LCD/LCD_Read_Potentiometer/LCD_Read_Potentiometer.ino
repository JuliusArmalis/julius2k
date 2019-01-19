/*
 * Program reads data from two potentiometers: one for capital letters
 * the other for symbols.
 * There are two buttons: one for confirming a symbol and movingthe cursor 
 * to the next spot, and one for deleting a single character or whole scree if
 * you press it for 2~4 secs.
 */

#include <Wire.h>

#include <TimerOne.h>

#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

#define knob1 A0
#define knob2 A1
#define confirmButton 2
#define deleteButton 3

volatile int knob1Value;        //  The first potentiometer's value
volatile int knob2Value;        //  The second potentiometer's value
volatile int prevValue1 = 0;
volatile int prevValue2 = 0;
volatile bool knob1ON = false;  //  Shows if the first potentiometer is in use 
volatile bool knob2ON = false;  //  Shows if the second potentiometer is in use

volatile char ch;               //  The character to print

const uint8_t n = 2;  //  LCD number of rows
const uint8_t m = 16; //  LCD number of collumns
volatile uint8_t r = 0; // Row number
volatile uint8_t c = 0; // Collumn number

unsigned long prevTime = 0;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(m, n);
  pinMode(confirmButton, INPUT);
  pinMode(deleteButton, INPUT);
  Timer1.initialize(100000);  
  Timer1.attachInterrupt(KnobValue);  //  Potentiometers can interrupt
  attachInterrupt(digitalPinToInterrupt(confirmButton), ConfirmButton, FALLING);  //  Button 1 can interrupt
  attachInterrupt(digitalPinToInterrupt(deleteButton), DeleteButton, FALLING);    //  Button 2 can interrupt
  
}

void loop() 
{
  // put your main code here, to run repeatedly:
  unsigned long currTime = millis();
  if(currTime - prevTime >= 50)
  {
    lcd.setCursor(c, r);
    if(c == 0 && r == 0)
      lcd.clear();
            
    lcd.print(ch);
    if(c == m - 1 && r > n - 1)
      {
        lcd.setCursor(0,r + 1);
        lcd.print(" ");    
      }
     else
     {
       lcd.setCursor(c + 1, r);
       lcd.print(" ");
     }
    prevTime = currTime;  
  }
}

void KnobValue ()
{
  
  knob1Value = analogRead(knob1);
  knob2Value = analogRead(knob2);
  
  if(knob1Value != prevValue1 || (knob1Value == prevValue1 && knob1ON))
  {
    knob1ON = true;
    knob2ON = false;
    prevValue1 = knob1Value;
    ch = map(knob1Value, 0, 1023, 65, 90);
  }
  if(knob2Value != prevValue2 || (knob2Value == prevValue2 && knob2ON))
  {
    knob2ON = true;
    knob1ON = false;
    prevValue2 = knob2Value;
    ch = map(knob2Value, 0, 1023, 32, 63);
  }
}

void ConfirmButton()
{
  static unsigned long prevTime = 0;
  unsigned long interrTime = millis();
  if(interrTime - prevTime > 150)
  {
    if(c + 1 > m - 1 && r > n - 1)
    {
      c = 0;
      r++;
    }
    else
    {
      if (c + 1 < m)
        c++;
    }
  }
  prevTime = interrTime;
}

void DeleteButton()
{
  static unsigned long prevTime = 0;
  
  unsigned long interrTime = millis();

  if(interrTime - prevTime > 150)
  {
    if(c != 0)
      c--;
    else
    {
      if(r != 0)
      {
        c = m - 1;
        r--;
      }
    }
  }

  if(interrTime - prevTime > 2000 && interrTime - prevTime < 4000)
  {
    c = 0;
    r = 0;
  }
    
  prevTime = interrTime;
}
