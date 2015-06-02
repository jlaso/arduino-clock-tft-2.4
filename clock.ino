// Code provided by Smoke And Wires
// http://www.smokeandwires.co.nz
// This code has been taken from the Adafruit TFT Library and modified
//  by us for use with our TFT Shields / Modules
// For original code / licensing please refer to
// https://github.com/adafruit/TFTLCD-Library

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library
#include <stdint.h>
#include "TouchScreen.h"

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
// #define LCD_CS A3 // Chip Select goes to Analog 3
// #define LCD_CD A2 // Command/Data goes to Analog 2
// #define LCD_WR A1 // LCD Write goes to Analog 1
// #define LCD_RD A0 // LCD Read goes to Analog 0

// #define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define FG_COLOR WHITE
#define BG_COLOR BLACK
#define MINUTE_COL GREEN
#define HOUR_COL MAGENTA

// position of time in text
#define H_HOUR  10
#define H_MIN  60
#define H_SEC  110
#define SIZE_TIME 3
#define X_COL_TIME 10

#define BOXSIZE 40
#define MENU_BUTTON_X 200
#define MENU_BUTTON_Y 0 
#define MENU_WIDTH 40 
#define MENU_HEIGHT 35

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// different modes of working
#define MODE_CALIBRATE1 0
#define MODE_CALIBRATE2 1
#define MODE_CLOCK 2
#define MODE_CONFIG 3

int mode = MODE_CALIBRATE1;

SWTFT tft;
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// SWTFT tft;

unsigned int 
  seconds = 0, secondsOld = 0,
  minutes = 30, minutesOld = 0,
  hour = 11, hourOld = 0;
boolean
  incMinutes = false, incHour = false;
  
// calibration of TouchScreen
long endX = 320,
     endY = 240,
     startX = 0, 
     startY = 0;
int   w,h;

int x, y, r, sr, mr, hr,
    sx = 0, sy = 0,
    mx = 0, my = 0,
    hx = 0, hy = 0;
    
float a;  

// For better pressure precision, we need to know the resistance
  // between X+ and X- Use any multimeter to read it
  // For the one we're using, its 300 ohms across the X plate
TouchScreen  ts = TouchScreen(XP, YP, XM, YM, 300);

void setup(void) {
  Serial.begin(9600);
  
  // init TFT
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(2);

  // obtain measure of screen
  w = tft.width();
  h = tft.height();
      
  // radius and center of the clock 
  r = (min(w,h)/2) - 1; 
  x = w/2,
  y = h - x;

  // radius for seconds, minutes and hour
  sr = r-10;
  mr = sr-10;
  hr = mr-10;
  
  paintScreen();
 
  pinMode(13, OUTPUT);
}

void paintScreen()
{ 
  switch(mode){  
    case MODE_CALIBRATE1:        paintCalibrate1();        break;
    case MODE_CALIBRATE2:        paintCalibrate2();        break;
    case MODE_CLOCK:             paintClockScreen();       break;  
    case MODE_CONFIG:            paintConfigScreen();      break;  
  }   
}

void paintCalibrate1()
{
  tft.fillScreen(BLACK);
  tft.fillTriangle(0,0,w,0,w,10,MAGENTA);
  tft.fillTriangle(0,0,0,h,10,h,YELLOW);
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);

  tft.setTextSize(2);
  tft.setCursor(10,h/2);
  tft.print("Calibrating, press on red");
}

void paintCalibrate2()
{
  tft.fillScreen(BLACK);
  tft.fillTriangle(0,0,w,0,w,10,MAGENTA);
  tft.fillTriangle(0,0,0,h,10,h,YELLOW);
  tft.fillRect(w-BOXSIZE, h-BOXSIZE, BOXSIZE, BOXSIZE, RED);

  tft.println("Calibrating, press on red");
}

void paintClockScreen()
{
  tft.fillScreen(BG_COLOR);
  
  tft.fillTriangle(0,0,w,0,w,10,MAGENTA);
  tft.fillTriangle(0,0,0,h,10,h,YELLOW);
  
  tft.drawCircle(x, y, r, FG_COLOR);
  
  tft.setTextSize(SIZE_TIME);
  tft.setTextColor(FG_COLOR);
  tft.setCursor(H_MIN,X_COL_TIME-30);
  tft.print(":");
  tft.setCursor(H_SEC,X_COL_TIME-30);
  tft.print(":");  
  
  tft.fillRect(MENU_BUTTON_X, MENU_BUTTON_Y, MENU_WIDTH, MENU_HEIGHT, WHITE);
}

void paintConfigScreen()
{
  tft.fillScreen(BG_COLOR);

  tft.setTextSize(SIZE_TIME);
  tft.setTextColor(FG_COLOR);
  tft.setCursor(X_COL_TIME,H_MIN-30);
  tft.print("Config");
  
//  tft.fillRect(MENU_BUTTON_X, MENU_BUTTON_Y, MENU_WIDTH, MENU_HEIGHT, WHITE);
}


void readTouchScreen(int milliseconds)
{
  unsigned long start = millis();

  do{
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map( p.x, startX, endX, 0, w );
       p.y = map( p.y, startY, endY, 0, h );   
   
       Serial.print("p.x="); Serial.println(p.x);
       Serial.print("p.y="); Serial.println(p.y); 
       
       tft.fillCircle(p.x, p.y, 10, RED);

       switch(mode){
       
         case MODE_CLOCK:
           if((p.x>=MENU_BUTTON_X) and (p.x<=(MENU_BUTTON_X+MENU_WIDTH)) 
              and (p.y>=MENU_BUTTON_Y) and (p.y<=(MENU_BUTTON_Y+MENU_HEIGHT))){
                mode = MODE_CONFIG;
                paintScreen();
                return;
              }
           break;
         
       }  
    }    
  }while(millis()-start < milliseconds);
}


void printDec2(int x, int y, int textSize, int color, int value)
{
   tft.setCursor(x, y);
   tft.setTextColor(color);
   tft.setTextSize(textSize);
   if(value<10){
     tft.print('0');
   }
   tft.print(value);
}


void drawSegment(int x, int y, int r1, int r2, float a, int col)
{
  a = (a / 57.2958) - 1.57; 
  float a1 = a-1.57,
      a2 = a+1.57,
      x1 = x + (cos(a1) * r1),
      y1 = y + (sin(a1) * r1),
      x2 = x + (cos(a2) * r1),
      y2 = y + (sin(a2) * r1),
      x3 = x + (cos(a) * r2),
      y3 = y + (sin(a) * r2);
      
  tft.fillTriangle(x1,y1,x2,y2,x3,y3,col);
}

void eraseTime()
{
  if(sx+sy>0){
     // erase previous line
    tft.drawLine(x, y, sx, sy, BG_COLOR);
    //drawSegment(x, y, 6, sr, 6*secondsOld, BG_COLOR);

    if(incHour){
      drawSegment(x, y, 8, hr, 30*hourOld, BG_COLOR);
      printDec2(H_HOUR,X_COL_TIME,SIZE_TIME,BG_COLOR,hourOld);
      incHour=false;
    }
    
    if(incMinutes){
      //tft.drawLine(x, y, mx, my, BG_COLOR);
      drawSegment(x, y, 6, mr, 6*minutesOld, BG_COLOR);
      printDec2(H_MIN,X_COL_TIME,SIZE_TIME,BG_COLOR,minutesOld);
      incMinutes=false;
    }
    
    printDec2(H_SEC,X_COL_TIME,SIZE_TIME,BG_COLOR,secondsOld);
  }
}


void drawTime()
{
  // center circle
  //tft.fillCircle(x, y, 8, WHITE);
  
  drawSegment(x, y, 8, hr, 30*hour, HOUR_COL);
  drawSegment(x, y, 6, mr, 6*minutes, MINUTE_COL);  
  
  a = (6 * seconds / 57.2958) - 1.570; 
  sx = x + (cos(a) * sr);
  sy = y + (sin(a) * sr);
  tft.drawLine(x, y, sx, sy, FG_COLOR);

  printDec2(H_HOUR,X_COL_TIME,SIZE_TIME,HOUR_COL,hour);
  printDec2(H_MIN,X_COL_TIME,SIZE_TIME,MINUTE_COL,minutes);
  printDec2(H_SEC,X_COL_TIME,SIZE_TIME,FG_COLOR,seconds);
}


void incTime()
{
  secondsOld = seconds;
  minutesOld = minutes;
  hourOld = hour;
  
  if(seconds>=59){
    seconds = 0;
    incMinutes=true;
    if(minutes>=59){
      minutes = 0;
      incHour=true;
      if(hour>=12){
        hour = 1;
      }else{
        hour++;
      }
    }else{
      minutes++;
    }
  }else{
    seconds++;
  }
}

void loop(void) {
  unsigned long start = millis();
     
  switch(mode){
    case MODE_CALIBRATE1:
        getCalibrateTouch();
        break;
    case MODE_CALIBRATE2:
        getCalibrateTouch();
        break;
    case MODE_CLOCK:
        eraseTime();
        drawTime();  
        incTime();
        readTouchScreen(950 - millis() + start);
        delay(1000 - millis() + start);  
        break;
    case MODE_CONFIG:
        readTouchScreen(0);
        break;
  }
  
}


void getCalibrateTouch()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if (mode==MODE_CALIBRATE1){
      startX = p.x;
      startY = p.y;
      Serial.print("startX="); Serial.println(startX);
      Serial.print("startY="); Serial.println(startY);      
    }else{
      endX = p.x; 
      endY = p.y;
      Serial.print("p.x="); Serial.println(p.x);
      Serial.print("p.y="); Serial.println(p.y);  
//      Serial.print("factorX="); Serial.println(factorX);
//      Serial.print("factorY="); Serial.println(factorY);      
    }
    mode++;
    paintScreen();
  }
}

