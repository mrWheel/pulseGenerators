/*
* Pulsgever
* 
*   +--+     +--+
* A |  |     |  |
* --+  +-----+  +-----
* 
*     +--+     +--+
*   B |  |     |  |
* ----+  +-----+  +----
* 
* Connect lineair potmeter 10K between 3V3, GND and GPIO34
* 
*   3v3 -------+
*              |
*             +-+
*             | |
*             | |<--- GPIO34
*             | |
*             +-+
*              |
*   GND -------+ 
*/
 
#define _DEBUG  1 // 1 = print output, 0 = no prints (production)
#define pinKnop 13
#define pinA    26
#define pinB    18
#define pinPot  34
#define pinSDA  21
#define pinSCL  22

#include <WiFi.h>
#include <Wire.h>  
#include <U8g2lib.h>


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C
//U8G2_SSD1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C
uint32_t  tmpFrequency, frequency;
int32_t   potValue = 0;
uint32_t  readTimer;
bool      upCount = true;
uint32_t  period;
uint32_t  pulseDuration;
int32_t   pulseHigh;
int32_t   halfPulseHigh;

//====================================================================
void initOLED()
{
  //--- initialize the library
  u8g2.begin();

  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_helvB08_tr); // choose a suitable font
  u8g2.drawStr(25, 10, "Willem");
  u8g2.drawStr(2, 30, "ENCODER SIMULATOR");
  u8g2.drawStr(2, 40, "FW: v1.2");
  u8g2.drawStr(2, 50, "D: 03072020");
  u8g2.drawStr(2, 60, "DEBUG Set 0-1");
  
  u8g2.sendBuffer();          // transfer internal memory to the display

} // initOLED()


//====================================================================
void debugOLED()
{
  u8g2.clearBuffer();         // clear the internal memory 
  u8g2.setCursor(64, 32);
  u8g2.setFont(u8g2_font_open_iconic_thing_6x_t);
  u8g2.print(char(77));       // pot
  u8g2.sendBuffer();          // transfer internal memory to the display

} // debugOLED()


//====================================================================
void updateOLED()
{
  //--- display frequency on OLED display
  //--- make use of:
  //---  > 'frequency'
  //---  > 'potValue'
  //---  > 'pulseDuration'
  //--- You really don't need anything else
  u8g2.clearBuffer();          // clear the internal memory;
  u8g2.setCursor(50, 15);
  u8g2.setFont(u8g2_font_helvB10_tr); // choose a suitable font
  u8g2.print(frequency);
  u8g2.drawStr(110, 15, "Hz");   // put string of display at position X, Y
  u8g2.setCursor(10, 17);
  u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
  u8g2.print(char(70)); // pulse
  u8g2.setFont(u8g2_font_helvB10_tr); // choose a suitable font
  u8g2.setCursor(50, 37);
  u8g2.print(pulseDuration);
  u8g2.drawStr(95, 37, "TIME");   // put string of display at position X, Y
  u8g2.setCursor(10, 40);
  u8g2.setFont(u8g2_font_open_iconic_app_2x_t);
  u8g2.print(char(72)); // pot
  u8g2.setFont(u8g2_font_helvB10_tr); // choose a suitable font
  u8g2.setCursor(50, 60);
  u8g2.print(potValue);
  u8g2.drawStr(95, 60, "MAP");   // put string of display at position X, Y
  u8g2.setCursor(10, 63);
  u8g2.setFont(u8g2_font_open_iconic_arrow_2x_t);
  u8g2.print(char(87)); // pot
  u8g2.sendBuffer();          // transfer internal memory to the display

} // updateDisplay()


//====================================================================
void makePulses()
{
  digitalWrite(pinA, HIGH);
  delayMicroseconds(halfPulseHigh);
  digitalWrite(pinB, HIGH);
  delayMicroseconds(halfPulseHigh);
  digitalWrite(pinA, LOW);
  delayMicroseconds(halfPulseHigh);
  digitalWrite(pinB, LOW);
  while(period > micros()) { /* just wait */ }

  //--- period is used to fill the right pulse-duration
  period = micros() + pulseDuration; // do NOT remove this line on this place!!!

} // makePulses()


//====================================================================
// reads value of [pinPot]
// converts potValue to frequency
// > 0 = 10 Hz, 2047 = 25.000Hz
// returns:
//     - potValue (0-2047) B11111111111 => 2047Dec 
//     - frequency (Hz)
void readPotmeter()
{
  potValue = (potValue + potValue + analogRead(pinPot)) / 3;
  if (_DEBUG) { Serial.print("potValue:\t"); Serial.print(potValue); }

  //--- calculate tmpFreq
  tmpFrequency = (potValue + 1) * (25000 / 2048);

  //--- calculate duration of the pulse
  pulseDuration = round((100000 / tmpFrequency) * 10);  // microSeconds
  if (_DEBUG) { Serial.print(" \tDuration:\t"); Serial.print(pulseDuration); }
  
  //--- due to rounding errors (we work only with integers)
  //--- we now calculate the real frequency according to the (rounded) duration
  frequency = 1000000 / pulseDuration;
  if (_DEBUG) { Serial.print(" \tfrequency:\t"); Serial.print(frequency); }

  //--- calculate the time the 'A' pulse has to be HIGH
  pulseHigh     = pulseDuration / 3;  // 33%
  if (_DEBUG) { Serial.print(" \tHIGH:\t"); Serial.println(pulseHigh); }

  //--- the 'B' pulse will follow halfway the pulseHigh
  halfPulseHigh = pulseHigh / 2;
  
} // readPotmeter()


//====================================================================
void setup() {
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }
  //--- initialize OLED library/display
  initOLED();
  Serial.println("\r\nLets start pulsing ...\r\n");

  delay(5000);

  //--- it seems WiFi will influence ADC convertors
  WiFi.mode(WIFI_OFF);
  //--- just to be sure: also stop the BT transmitter
  btStop();

  //--- set GPIO pins to the correct state
  pinMode(pinA,  OUTPUT);
  pinMode(pinB,  OUTPUT);
  pinMode(pinPot, INPUT);
  
  //--- ADC is 12 bit's but that's rather nervous
  //--- so set it to 11 bit
  analogReadResolution(11);
  //--- do a first read
  readPotmeter();
  Serial.println("\r");

  readTimer = millis();
  frequency = -1;

} // setup()


//====================================================================
void loop() {
  //--- read ADC and calculate all vars every 1 second
  if (millis() > readTimer)
  {
    readTimer = millis() + 100;
    readPotmeter();
    updateOLED();   
  }

  //--- at last: make the 'A' and 'B' pulses
  makePulses();
  
} // loop()
