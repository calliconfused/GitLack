#include        <SPI.h>
#include        <Wire.h>
#include        <TouchScreen.h>

#include        <Adafruit_GFX.h>                // Core graphics library
#include        <Adafruit_TFTLCD.h>             // Hardware-specific library
#include        "OpenSansCondensed16pt.h"
#include        "DSEG7REGULAR40.h"
#include        "bitmaps.h"

#include        <Adafruit_BME280.h>

// now a long statement with some fixed values
// here the display and touchscreen values

#define         YP                A3            // must be an analog pin, use "An" notation!
#define         XM                A2            // must be an analog pin, use "An" notation!
#define         YM                9             // can be a digital pin
#define         XP                8             // can be a digital pin

#define         LCD_CS            A3            // Chip Select goes to Analog 3
#define         LCD_CD            A2            // Command/Data goes to Analog 2
#define         LCD_WR            A1            // LCD Write goes to Analog 1
#define         LCD_RD            A0            // LCD Read goes to Analog 0
#define         LCD_RESET         1             // Can alternately just connect to Arduino's reset pin -> it's neccessary if you need the SDA and SCL pins

#define         TS_MINX           120           // touchscreen alternative minimum value in X direction
#define         TS_MAXX           900           // touchscreen alternative minimum value in X direction
#define         TS_MINY           70            // touchscreen alternative minimum value in X direction
#define         TS_MAXY           920           // touchscreen alternative minimum value in X direction

#define         MINPRESSURE       10
#define         MAXPRESSURE       1000

uint16_t        LCD_ID =          0x9341;       // ILI9341 LCD driver, please read your documentation! I don't use all other drivers otherwise the space on the Uno will overrun

float           TEMP_TARGET_MIN = 18.0;         // lowerst adjustable temperature
float           TEMP_TARGET_MAX = 40.0;         // highest adjustable temperature

float           TEMP_PRESET_PLA = 25.0;         // preset for PLA filament
float           TEMP_PRESET_ABS = 35.0;         // preset for ABS filament

int             TFT_ROTATION  =   3;            // orientation of display

int             TS_POSX;
int             TS_POSY;

// some color definition

#define         COLOR_BLACK       0x0000    
#define         COLOR_LIGHTGREY   0xC618
#define         COLOR_DARKGREY    0x7BEF
#define         COLOR_RED         0xF800
#define         COLOR_YELLOW      0xFFE0
#define         COLOR_ORANGE      0xFD20
#define         COLOR_GREENYELLOW 0xAFE5
#define         COLOR_ARSENIC     0x2965

// now the GY-BME280 values

uint16_t        BME_ID =          0x76;         // BME ID

// and some fixed values as general setup

int             SELECTED_FAN;                     // 0 = OFF, 1 = automatic
int             SELECTED_LED;                     // 0 = OFF, 1 = 50%, 2 = 100%
int             SELECTED_FAN_LAST;                // save the last selected value for the fan

float           TEMPERATURE_CURRENT;              // save the current temperature
float           TEMPERATURE_LAST;                 // save the last temperature

float           TEMPERATURE_TARGET;               // save the target temperature
float           TEMPERATURE_TARGET_LAST;          // save the last target ... ? ... temperature

// instead of the delay command which won't work in this sketch I prefer to use the millis command together with an if statement

unsigned long   TIMER_TS_DELAY;                   
long            TIMER_TS_REST =   1000;           // count in milli seconds
unsigned long   TIMER_TE_DELAY;
long            TIMER_TE_REST =   1500;           // count in milli seconds
unsigned long   TIMER_GL_DELAY;
long            TIMER_GL_REST =   5000;           // count in milli seconds

unsigned long   TIMER_MENU_DELAY;
long            TIMER_MENU_REST = 3600000;        // count in milli seconds - a general refresh of the screen


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // mesaure the resistance between XP and XM and change the value
TSPoint p;

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_BME280 bme;

void setup(void) {
  
  Serial.begin(9600);

  Serial.println("INITALIZING");
  
  Wire.begin();
  
  TEMPERATURE_CURRENT = 0;
  TEMPERATURE_LAST = 0;  

  SELECTED_FAN = 0;
  SELECTED_FAN_LAST = 0;
  SELECTED_LED = 2;

  TEMPERATURE_TARGET = 20;
  TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;

  TS_POSX = 0;
  TS_POSY = 0;
  
  TIMER_TS_DELAY = millis();
  TIMER_TE_DELAY = millis();
  TIMER_GL_DELAY = millis();
  TIMER_MENU_DELAY = millis() + TIMER_MENU_REST;
  
  bme.begin(BME_ID);
  
  tft.reset();
  tft.begin(LCD_ID);
  
  tft.setRotation(TFT_ROTATION);
  
  pinMode(13, OUTPUT);
  
  vDrawMenu();                // draw grid and all the other nice stuff
  vSendUpdateToSlave();       // send an update to the slave device

}

void loop() {

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW); 

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  // I don't finished this if statement with option 1, 2 and 4, need support!
  
  if ( TFT_ROTATION == 1) {         // needs to be adjust
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT); 
  } else if ( TFT_ROTATION == 2) {  // needs to be adjust
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT); 
  } else if ( TFT_ROTATION == 3) {  // THAT WORKS!
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } else if ( TFT_ROTATION == 4) {  // needs to be adjust
    //pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    //pinMode(YM, OUTPUT); 
  }


  if ( p.z > MINPRESSURE && p.z < MAXPRESSURE && TIMER_TS_DELAY <= millis() ) {

    // orienation ... need if statement in combination with TFT_ROTATION
    if ( TFT_ROTATION == 1 ) {          // needs to be adjust
      TS_POSX = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      TS_POSY = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    } else if ( TFT_ROTATION == 2 ) {   // THAT WORKS!
      TS_POSX = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      TS_POSY = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    } else if ( TFT_ROTATION == 3 ) {   // THAT WORKS!
      TS_POSX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
      TS_POSY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    } else if ( TFT_ROTATION == 4 ) {   // needs to be adjust
      TS_POSX = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      TS_POSY = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    }

    TIMER_TS_DELAY = millis() + TIMER_TS_REST;

    // now the long if statement of the pressed position
    // target temperature up
    if  (( TS_POSX >= 142 ) && ( TS_POSY >=  62 ) && ( TS_POSX <= 178 ) && ( TS_POSY <=  98 )) {

      if ( SELECTED_FAN == 0 ) {
        TEMPERATURE_TARGET = TEMPERATURE_TARGET_LAST;
        SELECTED_FAN = 1;
      } else 
      if ( SELECTED_FAN == 1 ) { 
        TEMPERATURE_TARGET ++;
        if ( TEMPERATURE_TARGET > TEMP_TARGET_MAX ) {
          TEMPERATURE_TARGET = TEMP_TARGET_MAX;
          TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;
        }
        SELECTED_FAN = 1;
      }
      
      vShowTemperatureTarget();
      vSendUpdateToSlave();
      
    } else
  
    // target temperature down
    if  (( TS_POSX >= 142 ) && ( TS_POSY >= 106 ) && ( TS_POSX <= 178 ) && ( TS_POSY <= 142 )) {
      
      if ( SELECTED_FAN == 0 ) {
        TEMPERATURE_TARGET = TEMPERATURE_TARGET_LAST;
        SELECTED_FAN = 1;
      } else 
      if ( SELECTED_FAN == 1 ) { 
        TEMPERATURE_TARGET --;
        if ( TEMPERATURE_TARGET < TEMP_TARGET_MIN ) {
          TEMPERATURE_TARGET = TEMP_TARGET_MIN;
          TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;
        }
        SELECTED_FAN = 1;
      }      
      vShowTemperatureTarget();
      vSendUpdateToSlave();
      
    } else
  
    // fan off
    if  (( TS_POSX >= 142 ) && ( TS_POSY >= 150 ) && ( TS_POSX <= 178 ) && ( TS_POSY <= 186 )) {

      SELECTED_FAN = 0;
      
      vShowTemperatureTarget();
      vSendUpdateToSlave();
      
    } else
  
    // preset PLA
    if  (( TS_POSX >=  58 ) && ( TS_POSY >= 200 ) && ( TS_POSX <= 100 ) && ( TS_POSY <= 236 )) {

      SELECTED_FAN = 1;
      TEMPERATURE_TARGET = TEMP_PRESET_PLA;
      TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;
      
      vShowTemperatureTarget();
      vSendUpdateToSlave();
      
    } else 
  
    // preset ABS
    if  (( TS_POSX >= 108 ) && ( TS_POSY >= 200 ) && ( TS_POSX <= 150 ) && ( TS_POSY <= 236 )) {

      SELECTED_FAN = 1;
      TEMPERATURE_TARGET = TEMP_PRESET_ABS;
      TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;
      
      vShowTemperatureTarget();
      vSendUpdateToSlave();
      
    } else
  
    // switch LED
    if  (( TS_POSX >= 208 ) && ( TS_POSY >= 200 ) && ( TS_POSX <= 316 ) && ( TS_POSY <= 236 )) {
      
      SELECTED_LED ++;
  
      if ( SELECTED_LED >= 3 ) {
        SELECTED_LED = 0;
      }

      vSendUpdateToSlave();
      
    }

    // reset touchscreen positions
    TS_POSX = 0;  TS_POSY = 0;

  }

  if ( TIMER_TE_DELAY <= millis() ) {
    vShowTemperatureCurrent();

    if ( SELECTED_FAN == 1 ) {
      vSendUpdateToSlave();
    }
    
    TIMER_TE_DELAY = millis() + TIMER_TE_REST;
  }

  if ( TIMER_GL_DELAY <= millis() ) {
    vSendUpdateToSlave;
    TIMER_GL_DELAY = millis() + TIMER_GL_REST;
  }

  if ( TIMER_MENU_DELAY <= millis() ) {
    vDrawMenu();
    TIMER_MENU_DELAY = millis() + TIMER_MENU_REST;
  }

}

void vShowTemperatureCurrent() {
    
  TEMPERATURE_CURRENT = bme.readTemperature();
  TEMPERATURE_CURRENT = TEMPERATURE_CURRENT * 10;
  TEMPERATURE_CURRENT = (int) TEMPERATURE_CURRENT;
  TEMPERATURE_CURRENT = TEMPERATURE_CURRENT / 10;

  if ( TEMPERATURE_CURRENT != TEMPERATURE_LAST ) {

    tft.setFont(&DSEG7REGULAR40);
    tft.setTextColor(COLOR_ARSENIC);
    tft.setCursor(6, 130);  tft.print("88.8");
    
    tft.setCursor(6, 130);

    if ( TEMPERATURE_CURRENT <= 30 ) {
      tft.setTextColor(COLOR_GREENYELLOW);  
    } else
    if ( TEMPERATURE_CURRENT > 30 && TEMPERATURE_CURRENT <= 45 ) {
      tft.setTextColor(COLOR_YELLOW);  
    } else    
    if ( TEMPERATURE_CURRENT > 45 ) {
      tft.setTextColor(COLOR_ORANGE);  
    }     
    tft.setFont(&DSEG7REGULAR40);
    tft.print(TEMPERATURE_CURRENT, 1);
 
  }

  TEMPERATURE_LAST = TEMPERATURE_CURRENT;
    
}

void vShowTemperatureTarget() {

  if ( SELECTED_FAN == 0 ) {
    
    tft.setFont(&DSEG7REGULAR40);
    tft.setTextColor(COLOR_ARSENIC);
    tft.setCursor(196, 130);            tft.print("88.8");    
  
    tft.setTextColor(COLOR_RED);
    tft.setCursor(196, 130);          tft.print(";:");
    tft.setTextColor(COLOR_ARSENIC);  tft.print(".");
    tft.setTextColor(COLOR_RED);      tft.print(":");
    
  } else
  if ( SELECTED_FAN == 1 ) {
    
    tft.setFont(&DSEG7REGULAR40);
    tft.setTextColor(COLOR_ARSENIC);
    tft.setCursor(196, 130);            tft.print("88.8");    
    
    tft.setTextColor(COLOR_RED);
    tft.setCursor(196, 130);          tft.print(TEMPERATURE_TARGET, 1);
  } 
  
  TEMPERATURE_TARGET_LAST = TEMPERATURE_TARGET;
  
}

void vSendUpdateToSlave() {

  // send to slave
  Wire.beginTransmission(2);
  // send LED value
  Wire.write((byte) SELECTED_LED);
  // send fan option
  Wire.write((byte) SELECTED_FAN);
  // send current temperature
  Wire.write((byte) TEMPERATURE_CURRENT);
  // send target temperature
  Wire.write((byte) TEMPERATURE_TARGET);
  Wire.endTransmission();
  
}

void vDrawMenu() {

  tft.fillScreen(COLOR_BLACK);
  
  tft.setTextColor(COLOR_LIGHTGREY);
  tft.setFont(&Open_Sans_Condensed_Bold_16);

  // head
  tft.setCursor(112, 20);   tft.print("GitLACK | v0.4");

  // grid
  tft.drawFastHLine(0, 31, 320, COLOR_DARKGREY);
  tft.drawFastHLine(0, 192, 320, COLOR_DARKGREY);
  
  tft.drawFastVLine(130, 31, 161, COLOR_DARKGREY);
  tft.drawFastVLine(190, 31, 161, COLOR_DARKGREY);
  tft.drawFastVLine(160, 192, 48, COLOR_DARKGREY);

  // left area of current temperature
  tft.drawCircle(113, 114, 1, COLOR_LIGHTGREY);
  tft.drawCircle(113, 114, 2, COLOR_LIGHTGREY);

  tft.setCursor(116, 128);  tft.print("C");  
  tft.setCursor(39, 50);    tft.print("current");
  tft.setCursor(22, 68);    tft.print("temperature");

  // right area of target temperature
  tft.drawCircle(303, 114, 1, COLOR_LIGHTGREY);
  tft.drawCircle(303, 114, 2, COLOR_LIGHTGREY);

  tft.setCursor(306, 128);  tft.print("C");  
  tft.setCursor(233, 50);   tft.print("target");
  tft.setCursor(216, 68);   tft.print("temperature");

  // middle area to set temperature
  tft.setCursor(150, 50);  tft.print("SET"); 
  
  tft.drawBitmap(144, 64, SYMBOL_arrowup, 32, 32, COLOR_LIGHTGREY);
  tft.drawRoundRect(142, 62, 36, 36, 4, COLOR_LIGHTGREY);
  
  tft.drawBitmap(144, 108, SYMBOL_arrowdown, 32, 32, COLOR_LIGHTGREY);
  tft.drawRoundRect(142, 106, 36, 36, 4, COLOR_LIGHTGREY);
  
  tft.drawBitmap(144, 152, SYMBOL_switchoff, 32, 32, COLOR_LIGHTGREY);
  tft.drawRoundRect(142, 150, 36, 36, 4, COLOR_LIGHTGREY);

  // lower left area to set the preset
  tft.setCursor(4, 212);  tft.print("preset"); 
  tft.setCursor(8, 232);  tft.print("temp");

  tft.setCursor(66, 224);  tft.print("PLA"); 
  tft.drawRoundRect(58, 200, 42, 36, 4, COLOR_LIGHTGREY);
  tft.setCursor(116, 224);  tft.print("ABS"); 
  tft.drawRoundRect(108, 200, 42, 36, 4, COLOR_LIGHTGREY);

  // lower right area to set the LEDs
  tft.setCursor(168, 224);  tft.print("LEDs"); 

  tft.setCursor(217, 224);  tft.print("switch mode"); 
  tft.drawRoundRect(208, 200, 108, 36, 4, COLOR_LIGHTGREY);
  
  Serial.println("OK - PASSED!");
  Serial.println("WELCOME to GitLACK | v0.4");

  TEMPERATURE_LAST = 0;
  vShowTemperatureCurrent();
  vShowTemperatureTarget();
    
}
