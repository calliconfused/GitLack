#include        <SPI.h>
#include        <Wire.h>
#include        <TouchScreen.h>

#include        <Adafruit_GFX.h>    // Core graphics library
#include        <Adafruit_TFTLCD.h> // Hardware-specific library
#include        <Fonts/OpenSansCondensed16pt.h>
#include        <Fonts/OpenSansCondensed24pt.h>
#include        <Fonts/DSEG7ClassicMini40pt.h>

#include        <Adafruit_BME280.h>

#define         YP                A3  // must be an analog pin, use "An" notation!
#define         XM                A2  // must be an analog pin, use "An" notation!
#define         YM                9   // can be a digital pin
#define         XP                8   // can be a digital pin

#define         LCD_CS            A3 // Chip Select goes to Analog 3
#define         LCD_CD            A2 // Command/Data goes to Analog 2
#define         LCD_WR            A1 // LCD Write goes to Analog 1
#define         LCD_RD            A0 // LCD Read goes to Analog 0
#define         LCD_RESET         1 // Can alternately just connect to Arduino's reset pin

#define         COLOR_BLACK       0x0000
#define         COLOR_NAVY        0x000F
#define         COLOR_DARKGREEN   0x03E0
#define         COLOR_DARKCYAN    0x03EF
#define         COLOR_MAROON      0x7800
#define         COLOR_PURPLE      0x780F
#define         COLOR_OLIVE       0x7BE0
#define         COLOR_LIGHTGREY   0xC618
#define         COLOR_DARKGREY    0x7BEF
#define         COLOR_BLUE        0x001F
#define         COLOR_GREEN       0x07E0
#define         COLOR_CYAN        0x07FF
#define         COLOR_RED         0xF800
#define         COLOR_MAGENTA     0xF81F
#define         COLOR_YELLOW      0xFFE0
#define         COLOR_WHITE       0xFFFF
#define         COLOR_ORANGE      0xFD20
#define         COLOR_GREENYELLOW 0xAFE5
#define         COLOR_PINK        0xF81F

#define         TS_MINX           120
#define         TS_MAXX           900
#define         TS_MINY           70
#define         TS_MAXY           920

#define         MINPRESSURE       10
#define         MAXPRESSURE       1000

#define         NODE_MAX_NUMBERS  5

uint16_t        LCD_ID =          0x9341; //ILI9341 LCD driver
uint16_t        BME_ID =          0x76;

float           TEMPERATURE_CURRENT;
float           TEMPERATURE_LAST;
unsigned long   TIMER_TS_DELAY;
long            TIMER_TS_REST =   1000;
unsigned long   TIMER_TE_DELAY;
long            TIMER_TE_REST =   1500;

int             TS_POSX;
int             TS_POSY;

int             SELECTED_FAN;   // 0 = OFF, 1 = 33%, 2 = 66%, 3 = FULL, 4 = AUTO
int             SELECTED_LED;   // 0 = OFF, 1 = 25%, 2 = 50%, 3 = 75%,  4 = FULL
int             SELECTED_FAN_LAST;
int             SELECTED_LED_LAST;

int             POS_CIRCLE_X[] =  {72, 208};
int             POS_CIRCLE_Y[] =  {302, 275, 248, 221, 194};
char            *DESC_OPTIONS_FAN[] = {"OFF", "33%", "66%", "FULL", "AUTO"};
char            *DESC_OPTIONS_LED[] = {"OFF", "25%", "50%", "75%", "FULL"};

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // zwischen XP und XM den wiederstand messen und wert anpassen
TSPoint p;

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_BME280 bme;

void setup(void) {
  
  Serial.begin(9600);
  Wire.begin();
  
  TEMPERATURE_CURRENT = 0;
  TEMPERATURE_LAST = 0;  

  SELECTED_FAN = 0;
  SELECTED_FAN_LAST = 9;
  SELECTED_LED = 4;
  SELECTED_LED_LAST = 9;

  TS_POSX = 0;
  TS_POSY = 0;
  
  TIMER_TS_DELAY = millis();
  TIMER_TE_DELAY = millis();
  
  bme.begin(BME_ID);
  
  tft.reset();
  tft.begin(LCD_ID);
  
  Serial.println("OK - PASSED!");
  Serial.println("WELCOME to FAN & SPEED CONTROL UNIT");
  
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  
  pinMode(13, OUTPUT);
  
  tft.setTextColor(COLOR_LIGHTGREY);
  tft.setFont(&Open_Sans_Condensed_Bold_24);
  
  tft.setCursor(73, 20);
  tft.print("FAN & LED");
  
  tft.setCursor(50, 40);
  tft.print("CONTROL UNIT"); 
  
  tft.setCursor(174, 96 + 42);
  tft.print("C"); 

  tft.setCursor(31, 175);
  tft.print("FAN");

  tft.setCursor(164, 175);
  tft.print("LED");
  
  tft.setFont(&Open_Sans_Condensed_Bold_16);
  
  tft.setCursor(40, 80);
  tft.print("CURRENT TEMPERATURE");   

  // circle for degrees
  tft.drawCircle(170, 120, 4, COLOR_LIGHTGREY);
  tft.drawCircle(170, 120, 3, COLOR_LIGHTGREY);

  // round rectangulars and text based on cirlce positions

  for ( int i = 0; i <= 1; i++ ) {
    for ( int j = 0; j <= 4; j++ ) {
      tft.drawRoundRect(POS_CIRCLE_X[i] - 12, POS_CIRCLE_Y[j] - 12, 25, 25, 3, COLOR_DARKGREY);
      tft.setCursor(POS_CIRCLE_X[i] - 57, POS_CIRCLE_Y[j] + 6);

      if ( i == 0 ) {
        tft.print(DESC_OPTIONS_FAN[j]);  
      } else
      if ( i == 1 ) {
        tft.print(DESC_OPTIONS_LED[j]);  
      }
      
    }
  }
  
  Serial.println("Ready, waiting for user input!");

}

void loop() {

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW); 

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT); 

  if ( p.z > MINPRESSURE && p.z < MAXPRESSURE && TIMER_TS_DELAY <= millis() ) {

    TS_POSX = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    TS_POSY = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);   

    TIMER_TS_DELAY = millis() + TIMER_TS_REST;
    
  }

  // now the long if statement of the pressed position

  if  (( TS_POSX >= 10 ) && ( TS_POSX <= 90 ) && ( TS_POSY >= 182 ) && ( TS_POSY <= 204 )) {
    SELECTED_FAN = 4;
  }

  if  (( TS_POSX >= 10 ) && ( TS_POSX <= 90 ) && ( TS_POSY >= 209 ) && ( TS_POSY <= 233 )) {
    SELECTED_FAN = 3;
  }

  if  (( TS_POSX >= 10 ) && ( TS_POSX <= 90 ) && ( TS_POSY >= 236 ) && ( TS_POSY <= 260 )) {
    SELECTED_FAN = 2;
  }

  if  (( TS_POSX >= 10 ) && ( TS_POSX <= 90 ) && ( TS_POSY >= 263 ) && ( TS_POSY <= 287 )) {
    SELECTED_FAN = 1;
  }      
  
  if  (( TS_POSX >= 10 ) && ( TS_POSX <= 90 ) && ( TS_POSY >= 290 ) && ( TS_POSY <= 314 )) {
    SELECTED_FAN = 0;
  }
  
  if  (( TS_POSX >= 150 ) && ( TS_POSX <= 230 ) && ( TS_POSY >= 182 ) && ( TS_POSY <= 204 )) {
    SELECTED_LED = 4;
  }

  if  (( TS_POSX >= 150 ) && ( TS_POSX <= 230 ) && ( TS_POSY >= 209 ) && ( TS_POSY <= 233 )) {
    SELECTED_LED = 3;
  }

  if  (( TS_POSX >= 150 ) && ( TS_POSX <= 230 ) && ( TS_POSY >= 236 ) && ( TS_POSY <= 260 )) {
    SELECTED_LED = 2;
  }

  if  (( TS_POSX >= 150 ) && ( TS_POSX <= 230 ) && ( TS_POSY >= 263 ) && ( TS_POSY <= 287 )) {
    SELECTED_LED = 1;
  }      
  
  if  (( TS_POSX >= 150 ) && ( TS_POSX <= 230 ) && ( TS_POSY >= 290 ) && ( TS_POSY <= 314 )) {
    SELECTED_LED = 0;
  }

  if ( TIMER_TE_DELAY <= millis() ) {
    vShowTemperature();

    if ( SELECTED_FAN == 4 ) {
      // send to slave
      Wire.beginTransmission(2);
      //Wire.write('T');
      Wire.write((byte) 0);
      //Wire.write('P');
      Wire.write((byte) 4);
      //Wire.write('C');
      Wire.write((byte) TEMPERATURE_CURRENT);
      Wire.endTransmission();      
    }
    
    TIMER_TE_DELAY = millis() + TIMER_TE_REST;
  }
  
  vChangeButtons(SELECTED_FAN, SELECTED_FAN_LAST, 0);
  vChangeButtons(SELECTED_LED, SELECTED_LED_LAST, 1);

}

void vShowTemperature() {
    
  TEMPERATURE_CURRENT = bme.readTemperature();
  TEMPERATURE_CURRENT = TEMPERATURE_CURRENT * 10;
  TEMPERATURE_CURRENT = (int) TEMPERATURE_CURRENT;
  TEMPERATURE_CURRENT = TEMPERATURE_CURRENT / 10;

  if ( TEMPERATURE_CURRENT != TEMPERATURE_LAST) {

    tft.fillRect(64, 96, 97, 44, COLOR_BLACK);

    tft.setCursor(61, 138);

    if ( TEMPERATURE_CURRENT <= 30 ) {
      tft.setTextColor(COLOR_GREENYELLOW);  
    } else
    if ( TEMPERATURE_CURRENT > 30 && TEMPERATURE_CURRENT <= 45 ) {
      tft.setTextColor(COLOR_YELLOW);  
    } else    
    if ( TEMPERATURE_CURRENT > 45 ) {
      tft.setTextColor(COLOR_ORANGE);  
    }     
    tft.setFont(&DSEG7_Classic_Mini_Regular_40);
    tft.print(TEMPERATURE_CURRENT, 1);
 
  }

  TEMPERATURE_LAST = TEMPERATURE_CURRENT;
    
}


void vChangeButtons(int SELECTED_OPTION, int SELECTED_OPTION_LAST, int SELECTED_TYPE) {

  if ( SELECTED_OPTION != SELECTED_OPTION_LAST ) {

    // reset last selected button

    if ( SELECTED_TYPE == 0 ) {
      tft.fillCircle(POS_CIRCLE_X[SELECTED_TYPE], POS_CIRCLE_Y[SELECTED_FAN_LAST], 7, COLOR_BLACK);
      SELECTED_FAN_LAST = SELECTED_OPTION;
    } else 
    
    if ( SELECTED_TYPE == 1 ) {
      tft.fillCircle(POS_CIRCLE_X[SELECTED_TYPE], POS_CIRCLE_Y[SELECTED_LED_LAST], 7, COLOR_BLACK);
      SELECTED_LED_LAST = SELECTED_OPTION;
    }    

    // fill the current selection

    Serial.print("Selected Type: "); Serial.print(SELECTED_TYPE); 
    Serial.print("\tPosition X: "); Serial.print(POS_CIRCLE_X[SELECTED_TYPE]);
    Serial.print("\tSelected Option: "); Serial.print(SELECTED_OPTION);
    Serial.print("\tPosition Y: "); Serial.println(POS_CIRCLE_Y[SELECTED_OPTION]);
    
    tft.fillCircle(POS_CIRCLE_X[SELECTED_TYPE], POS_CIRCLE_Y[SELECTED_OPTION], 7, COLOR_LIGHTGREY);

    // send to slave
    Wire.beginTransmission(2);
    //Wire.write('T');
    Wire.write((byte) SELECTED_TYPE);
    //Wire.write('P');
    Wire.write((byte) SELECTED_OPTION);
    //Wire.write('C');
    Wire.write((byte) TEMPERATURE_CURRENT);
    Wire.endTransmission();
  }
}

