/*
***************************************************************************
**  Program  : rotaryPulse328
**
**  TERMS OF USE: MIT License. See bottom of file.
** 
**  rotary Encoder signals:
**    +--+     +--+
**  A |  |     |  |
**  --+  +-----+  +-----
** 
**      +--+     +--+
**    B |  |     |  |
**  ----+  +-----+  +----
** 
*/

#define SET(a,b)      ((a) |= _BV(b))
#define CLEAR(a, b)   ((a) &= ~_BV(b))
#define SET_LOW(_port, _pin)    ((_port) &= ~_BV(_pin))
#define SET_HIGH(_port, _pin)   ((_port) |= _BV(_pin))

#define _DEBUG  1

#define pinA    8     // PB0
#define pinB    9     // PB1
#define pinPot  A0

float     frequency;
int       potValue = 0;
uint32_t  readTimer;
bool      upCount = true;
uint32_t  pulseDuration;
uint32_t  pulseHIGH, pulseLOW;

//====================================================================
void makePulse()
{
  //digitalWrite(pinA, HIGH);
  SET_HIGH(PORTB, 0); // PB0 = D8
  delayMicroseconds(pulseHIGH/2);
  SET_HIGH(PORTB, 1); // PB1 = D9
  delayMicroseconds(pulseHIGH/2);
  //digitalWrite(pinA, LOW);
  SET_LOW(PORTB, 0);
  delayMicroseconds(pulseHIGH/2);
  SET_LOW(PORTB, 1);
  delayMicroseconds((pulseLOW/2) - (pulseHIGH/2));
  
} // makePulse()


//====================================================================
void readPotmeter()
{
  /***
  if (upCount) 
  {
    if (potValue < 0.05)
          potValue += .001;
    else  potValue += .05;
  }
  else  
  {
    if (potValue > 0.05)
          potValue -= .05;
    else  potValue -= .001;
  }

  if (potValue > 5.0)
  {
    potValue = 5.0;
    upCount = false;
  }
  else if (potValue < 0.0)
  {
    potValue = 0.0;
    upCount = true;
  }
 
  return potValue;
  
  ***/
  
  //return map(analogRead(A0), 0, 1023, 0.0, 5.0);
  potValue = analogRead(pinPot);
  
} // readPotmeter()


//====================================================================
void setup() {
  Serial.begin(57600);
  while(!Serial) { /* wait a bit */ }
  
  pinMode(pinA,  OUTPUT);
  pinMode(pinB,  OUTPUT);
  pinMode(pinPot, INPUT);

  readTimer = millis();
  frequency = -1;

} // setup()


//====================================================================
void loop() {
  if (millis() > readTimer)
  {
    readTimer = millis() + 500;
    readPotmeter();
    // 0 = 0 Hz, 1023 = 25.000Hz
    frequency = potValue * (25000.0 / 1023.0);
    if (_DEBUG) { Serial.print("frequency:\t"); Serial.print(frequency); }
    pulseDuration = (uint32_t)(1000000 / frequency);  // microSeconds
    if (_DEBUG) { Serial.print("\tDuration:\t"); Serial.print(pulseDuration); }
    pulseHIGH = pulseDuration / 4;  // 33%
    if (_DEBUG) { Serial.print("\tHIGH:\t"); Serial.print(pulseHIGH); }
    pulseLOW  = pulseDuration - pulseHIGH;
    if (_DEBUG) { Serial.print("\tLOW:\t"); Serial.println(pulseLOW); }
  }
  makePulse();
  
} // loop()



/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
****************************************************************************
*/
