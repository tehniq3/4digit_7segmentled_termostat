// // thermostat with 18B20 ver.4m5 - 17.09.2015, Craiova - Romania
// http://nicuflorica.blogspot.ro/2013/07/afisaje-led-cu-7-segmente-si-arduino-ii.html
/*
 6-13-2011
 Spark Fun Electronics 2011
 Nathan Seidle
 This code is public domain but you buy me a beer if you use this and we meet 
 someday (Beerware license).
 4 digit 7 segment display:
 http://www.sparkfun.com/products/9483
 Datasheet: 
 http://www.sparkfun.com/datasheets/Components/LED/7-Segment/YSD-439AR6B-35.pdf
 This is an example of how to drive a 7 segment LED display from an ATmega
 without the use of current limiting resistors. This technique is very common 
 but requires some knowledge of electronics - you do run the risk of dumping 
 too much current through the segments and burning out parts of the display. 
 If you use the stock code you should be ok, but be careful editing the 
 brightness values.
 This code should work with all colors (red, blue, yellow, green) but the 
 brightness will vary from one color to the next because the forward voltage 
 drop of each color is different. This code was written and calibrated for the 
 red color.
 This code will work with most Arduinos but you may want to re-route some of 
 the pins.
 7 segments
 4 digits
 1 colon
 =
 12 pins required for full control 
 */
// modified connexion by niq_ro from http://nicuflorica.blogspot.com
// for my CL5642BH-33 display
// dataseet: http://www.tme.eu/ro/Document/dfc2efde2e22005fd28615e298ea2655/KW4-563XSA.pdf

byte digit1 = 11; //PWM Display pin 12 (digit1 is common anonds A1 from right side)
byte digit2 = 10; //PWM Display pin 9 (digit2 is  common A2)
byte digit3 = 9; //PWM Display pin 8 (digit3 is common anods A3)
byte digit4 = 6; //PWM Display pin 6 (digit4 is common anods, from left side)

//Pin mapping from Arduino to the ATmega DIP28 if you need it
// http://www.arduino.cc/en/Hacking/PinMapping
byte segA = 2; //Display pin 11
byte segB = 3; //Display pin 7
byte segC = 4; //Display pin 4
byte segD = 5; //Display pin 2
byte segE = 12; //Display pin 1
byte segF = 7; //Display pin 10
byte segG = 8; //Display pin 5

byte dP = 13; // decimal point

//#define LMPIN A0     // what pin we're LM35 is connected
//#define LMPIN2 A1     // what pin we're LM35 is connected

#define BUT1 A1    // - switch
#define BUT2 A2    // + switch
#define BUT3 A3    // MENU switch

#define RECE A4    // output for lower temperature
#define CALD A5    // output for upper temperature
#define BINE A6    // output for ok temperature

//int tset = 225;  // temperature set (x10)
//int tset = 24;  // temperature set (x10)
//int tset = 0;  // temperature set (x10)
//int tset = -1;  // temperature set (x10)
//int tset = -10;  // temperature set (x10)
//int tset = -247;  // temperature set (x10)

float t, t1, t0, t2, k;
int tset, tset1, tset2, dt, tset0;
int semn;
int pornire;
int hitcul; 

// define pins for relay or leds
#define temppeste A4
#define tempsub A5


// http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/
#include <EEPROM.h>

//int dt = 5;      // dt temperature

byte meniu = 0; // if MENIU = 0 is clasical 
                // if MENIU = 1 is for temperature set
                // if MENIU = 2 is for dt temperature 
                // if MENU = 3 is for type of thermostat (cooler or heater)
//float t, t0, t2, k;

// for DS18B20 - http://arduinoprojects.ru/2014/08/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9-%D1%82%D0%B5%D1%80%D0%BC%D0%BE%D1%81%D1%82%D0%B0%D1%82-%D0%BD%D0%B0-arduino-%D0%B8-%D1%86%D0%B8%D1%84%D1%80%D0%BE%D0%B2%D0%BE%D0%BC-%D1%82%D0%B5%D1%80%D0%BC/
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

 

void setup() {

//  analogReference(INTERNAL); // for Uno (1,1V)
  //analogReference(INTERNAL1V1); // for Mega (1,1V)


// just first time... after must put commnent (//)
/*
EEPROM.write(601,0);   // tset1 = for store the temperature set    
EEPROM.write(602,1); // tset2 = for store the temperature set
//tset = 256 * tset1 + tset2;  // recover the number
EEPROM.write(603,5);   // dt = histeresys
EEPROM.write(604,1);   // sign for temperature set (0 for negative, 1 for positive)
EEPROM.write(605,1);   // heater or cooler (0 for cooler, 1 for heater
*/
  
tset1 = EEPROM.read(601);
tset2 = EEPROM.read(602);
dt = EEPROM.read(603);
semn = EEPROM.read(604);
tset = 256 * tset1 + tset2;  // recover the number
if (semn == 0) tset = -tset;
hitcul = EEPROM.read(605);
  
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  
  pinMode(dP, OUTPUT);
  
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);

  pinMode(temppeste, OUTPUT);
  pinMode(tempsub, OUTPUT);

  digitalWrite(BUT1, HIGH); // pull-ups on
  digitalWrite(BUT2, HIGH);
  digitalWrite(BUT3, HIGH);

 
  
 Serial.begin(9600);
 Serial.println("test for niq_ro");

k = 1;  // factor corectie citire la multiplexare (corection factor)
delay(100);
/*
t = analogRead(A0);
t1 = analogRead(A1);
//t0 = t * 110 / 1023;  // pentru referinta interna de 1,1V;
t0 = k*(t-t1)*500/1023;
t2 = t0;
*/
pornire = 0; // centrala nu e pornita

// for DS18B20 sensor
sensors.begin(); 

// 9 bit resolution by default 
  // Note the programmer is responsible for the right delay
  // we could do something usefull here instead of the delay
  int resolution = 10;
  sensors.setResolution(resolution);
  delay(750/ (1 << (12-resolution)));

// using DS18B20 ( http://arduinoprojects.ru/2014/08/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9-%D1%82%D0%B5%D1%80%D0%BC%D0%BE%D1%81%D1%82%D0%B0%D1%82-%D0%BD%D0%B0-arduino-%D0%B8-%D1%86%D0%B8%D1%84%D1%80%D0%BE%D0%B2%D0%BE%D0%BC-%D1%82%D0%B5%D1%80%D0%BC/ )
sensors.requestTemperatures(); // запрос на получение температуры
float t2=(sensors.getTempCByIndex(0)); 
t0 = t2;
}

void loop() {

  Serial.print("meniu = ");
  Serial.println(meniu);

  Serial.print("tset = ");
  Serial.println(tset/10);
  Serial.print("dt = ");
  Serial.println(dt/10);

if (digitalRead(BUT3) == LOW) 
{ meniu = meniu + 1;
delay(250);
}
if (meniu == 4) meniu = 0;

if (millis() < 2000) citire();
leduri();
  Serial.print(t);
  Serial.print("/1023 --> ");
  Serial.print("t = ");
  Serial.println(t0);
    
if (meniu == 0) {
int numara = 0;
  while (meniu == 0) {
numara = numara + 1;
  if (numara == 5000)  // seconds x 384 (empiric formula)
    {
// using DS18B20 ( http://arduinoprojects.ru/2014/08/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9-%D1%82%D0%B5%D1%80%D0%BC%D0%BE%D1%81%D1%82%D0%B0%D1%82-%D0%BD%D0%B0-arduino-%D0%B8-%D1%86%D0%B8%D1%84%D1%80%D0%BE%D0%B2%D0%BE%D0%BC-%D1%82%D0%B5%D1%80%D0%BC/ )
sensors.requestTemperatures(); // запрос на получение температуры
float t2=(sensors.getTempCByIndex(0)); 
Serial.println(t2);
 t0 = (t0 + t2) /2;

// only for test:
//t0 = 23.4;
// t0 = 1.4;
// t0 = 0;
// t0 = -3.6;
// t0 = -23.4;


leduri();
 // delay(5);
  numara = 0;
  Serial.print("t = ");
  Serial.println(t0);
//  displayNumber(t0*10); // this is number to diplay
    }
if (t0 >= 0) te(t0*10); // this is number to diplay
if ((t0 > -10) and (t0 < 0)) te1(-t0*10); // this is number to diplay
if (t0 <= -10) te2(-t0*10); // this is number to diplay
  t2 = t0;
if (digitalRead(BUT3) == LOW) 
{ meniu = 1;
delay(250);
}
//Serial.print("t = ");
//Serial.println(t0);
//delay(5);
} 
delay(5);  
}



if (meniu == 1) {
   while (meniu == 1) {
//     minim(tset/10); // this is number to diplay
 
if (tset >= 0) teset(tset);
if (tset < 0) teset2(-tset);

   if (digitalRead(BUT1) == LOW)
  { tset = tset - 1; 
  delay(250);
  }
   if (digitalRead(BUT2) == LOW)
  { tset = tset + 1;
  delay(250);
  }
  if (digitalRead(BUT3) == LOW) 
  { meniu = 2;
  delay(250);
  }
//  delay(15);
     }

if (tset >= 0)
{
  semn = 1;
  tset0 = tset;
}
if (tset < 0)
{
  semn = 0;
  tset0 = -tset;  // use absolute value
}
  tset1 = tset0 / 256;
  tset2 = tset0 - tset1 * 256;
  EEPROM.write(601, tset1);  // partea intreaga
  EEPROM.write(602, tset2);   // rest
  EEPROM.write(604, semn);   // semn
  Serial.print("tset1z = ");
  Serial.println(tset1);
  Serial.print("tset2u = ");
  Serial.println(tset2);
  
  Serial.print("tset = ");
  Serial.println(tset/10);
   delay (100);
}

if (meniu == 2) {
 // if (tmax <= tmin) tmax = tmin + 10;
   while (meniu ==2) {
 //    maxim(dt/10); // this is number to diplay
     dete(dt); // this is number to diplay
   if (digitalRead(BUT1) == LOW) 
   { dt = dt - 1;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { dt = dt + 1;
   delay(250);
  }
   if (digitalRead(BUT3) == LOW) 
   { meniu = 3;
   delay(250);
  }
//   delay(15);
if (dt < 1) dt = 1;
    }
  EEPROM.write(603,dt);      
  Serial.print("dt = ");
  Serial.println(dt/10);
    delay(100);
}

if (meniu == 3) {
   while (meniu ==3) {
  
     if (hitcul == 0) culer();
     else hiter();
     
   if (digitalRead(BUT1) == LOW) 
   { hitcul = 0;
   delay(250);
  }
   if (digitalRead(BUT2) == LOW) 
   { hitcul = 1;
   delay(250);
  }
   if (digitalRead(BUT3) == LOW) 
   { meniu = 0;
   delay(250);
  }
//   delay(15);
if (dt < 1) dt = 1;
    }
  EEPROM.write(605,hitcul);      
  Serial.print("hitcul = ");
  Serial.println(hitcul);
    delay(100);
}
}



float citire() {
//  t = analogRead(A0);
//  t1 = analogRead(A1);
//  t0 = t * k *110 / 1023;  // pentru referinta interna de 1,1V;
//  t2 = k*(t-t1) *500 / 1023;

// 9 bit resolution by default 
  // Note the programmer is responsible for the right delay
  // we could do something usefull here instead of the delay
  int resolution = 10;
  sensors.setResolution(resolution);
  delay(750/ (1 << (12-resolution)));

// using DS18B20 ( http://arduinoprojects.ru/2014/08/%D0%BF%D1%80%D0%BE%D1%81%D1%82%D0%BE%D0%B9-%D1%82%D0%B5%D1%80%D0%BC%D0%BE%D1%81%D1%82%D0%B0%D1%82-%D0%BD%D0%B0-arduino-%D0%B8-%D1%86%D0%B8%D1%84%D1%80%D0%BE%D0%B2%D0%BE%D0%BC-%D1%82%D0%B5%D1%80%D0%BC/ )
sensors.requestTemperatures(); // запрос на получение температуры
float t2=(sensors.getTempCByIndex(0)); 
Serial.println(t2);
  return t2;}


//Given a number, we display 10:22
//After running through the 4 numbers, the display is left turned off

//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  // all segment are ON
  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;
  
  // degree symbol made by niq_ro
  case 11:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  // C letter made by niq_ro
  case 12:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;
  
  // c letter made by niq_ro
  case 13:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 14:  // lower line (d segment)
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;

case 15:  // upper line (a segment)
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;


  // c letter with _ made by niq_ro
  case 16:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 17:  // (-) midle line (g segment)
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 18:  // d letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;


  case 19:  // H letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 20:  // E letter
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 21:  // A letter
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 22:  // t letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 23:  // c letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 24:  // o letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    digitalWrite(13, SEGMENT_OFF);
    break;

  case 25:  // l letter
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    digitalWrite(13, SEGMENT_OFF);
    break;

    
  }
}



void leduri() {
if ( t0*10 > tset ) 
{
if (hitcul == 1) 
{
  digitalWrite(temppeste, HIGH); 
  digitalWrite(tempsub, LOW); 
}
if (hitcul == 0) 
{
  digitalWrite(temppeste, LOW); 
  digitalWrite(tempsub, HIGH); 
}
}
if ( t0*10 < tset - dt ) 
{
if (hitcul == 1)
{
  digitalWrite(temppeste, LOW); 
  digitalWrite(tempsub, HIGH); 
}
if (hitcul == 0)
{
  digitalWrite(temppeste, HIGH); 
  digitalWrite(tempsub, LOW); 
}
}
}


void te(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
 for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
    //  digitalWrite(13, HIGH);
      digitalWrite(13, LOW);
      if (toDisplay % 10 > 0) lightNumber(toDisplay % 10);
      if (toDisplay % 10 == 0) lightNumber(10);
      toDisplay /= 10;
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
//      digitalWrite(13, LOW);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      lightNumber(12); // display C letter
      digitalWrite(13, HIGH);
      break;
    }
 //   lightNumber(toDisplay % 10);
 //   toDisplay /= 10;
    delayMicroseconds(DISPLAY_BRIGHTNESS); 

     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 


void te1(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
 for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      digitalWrite(13, LOW);
      lightNumber(17);
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
//      digitalWrite(13, LOW);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      lightNumber(12); // display C letter
      digitalWrite(13, HIGH);
      break;
    }
 //   lightNumber(toDisplay % 10);
 //   toDisplay /= 10;
    delayMicroseconds(DISPLAY_BRIGHTNESS); 

     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 




void te2(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
 for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      digitalWrite(13, HIGH);
    //  digitalWrite(13, LOW);
      lightNumber(17);
    //   lightNumber(toDisplay % 10);
    //  toDisplay /= 10;
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
//      digitalWrite(13, LOW);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
  //    digitalWrite(13, HIGH);
      digitalWrite(13, LOW);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
     // lightNumber(12); // display C letter
//      lightNumber(16); // display C letter with _
lightNumber(toDisplay % 10);
      toDisplay /= 10;

      digitalWrite(13, HIGH);
      break;
    }
 //   lightNumber(toDisplay % 10);
 //   toDisplay /= 10;
    delayMicroseconds(DISPLAY_BRIGHTNESS); 

     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 






void teset(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
 for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      lightNumber(13); // display C letter
      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
//      lightNumber(toDisplay % 10);
//      toDisplay /= 10;
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
//      digitalWrite(13, LOW);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
  //    digitalWrite(13, HIGH);
      digitalWrite(13, LOW);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
     // lightNumber(12); // display C letter
//      lightNumber(16); // display C letter with _
lightNumber(toDisplay % 10);
      toDisplay /= 10;

      digitalWrite(13, HIGH);
      break;
    }
 //   lightNumber(toDisplay % 10);
 //   toDisplay /= 10;
    delayMicroseconds(DISPLAY_BRIGHTNESS); 

     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);

}
} 


void teset2(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW
 for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      lightNumber(16); // display c letter with _
      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
//      lightNumber(toDisplay % 10);
//      toDisplay /= 10;
      break;
   case 2:
      digitalWrite(digit2, DIGIT_ON);
//      digitalWrite(13, LOW);
      digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
  //    digitalWrite(13, HIGH);
      digitalWrite(13, LOW);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
     // lightNumber(12); // display C letter
//      lightNumber(16); // display C letter with _
lightNumber(toDisplay % 10);
      toDisplay /= 10;

      digitalWrite(13, HIGH);
      break;
    }
 //   lightNumber(toDisplay % 10);
 //   toDisplay /= 10;
    delayMicroseconds(DISPLAY_BRIGHTNESS); 

     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);

}
} 




void dete(int toDisplay) {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
  //    lightNumber(15); // display degree symbol
      lightNumber(18); // display d lettr
      digitalWrite(13, LOW);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;

    case 2:
      digitalWrite(digit2, DIGIT_ON);
   //   digitalWrite(13, LOW);
  //   digitalWrite(13, HIGH);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 3:
      digitalWrite(digit3, DIGIT_ON);
//      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
      lightNumber(toDisplay % 10);
      toDisplay /= 10;
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 4:
      digitalWrite(digit4, DIGIT_ON);
      lightNumber(13); // display C letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 


void hiter () {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      lightNumber(19); // display h letter
//      digitalWrite(13, LOW);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;

    case 2:
      digitalWrite(digit2, DIGIT_ON);
   //   digitalWrite(13, LOW);
  //   digitalWrite(13, HIGH);
        lightNumber(20); // display e letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 3:
      digitalWrite(digit3, DIGIT_ON);
//      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
        lightNumber(21); // display a letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 4:
      digitalWrite(digit4, DIGIT_ON);
        lightNumber(22); // display t letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 


void culer () {
#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      lightNumber(23); // display c letter
//      digitalWrite(13, LOW);
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;

    case 2:
      digitalWrite(digit2, DIGIT_ON);
   //   digitalWrite(13, LOW);
  //   digitalWrite(13, HIGH);
        lightNumber(24); // display o letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 3:
      digitalWrite(digit3, DIGIT_ON);
//      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
        lightNumber(24); // display o letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
   case 4:
      digitalWrite(digit4, DIGIT_ON);
        lightNumber(25); // display l letter
      delayMicroseconds(DISPLAY_BRIGHTNESS); 
      break;
    }
     //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
}
} 
