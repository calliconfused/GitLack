#include  <Wire.h>

#define   LED_PIN_R               5
#define   LED_PIN_G               6
#define   LED_PIN_B               9

#define   FAN_CONTROL             11
#define   FAN_RPM                 3
#define   INTERRUPT_RPM           0
#define   UPDATE_TIMER            500

byte      TEMP_DIFF_LOW           = 5;
byte      TEMP_DIFF_HIGH          = 10;
byte      LOWEST_FAN_SPEED        = 63;
byte      HIGHEST_FAN_SPEED       = 255;

byte      SELECTED_LED;
byte      SELECTED_LED_LAST;
byte      SELECTED_FAN;
byte      SELECTED_FAN_LAST;
byte      TEMPERATURE_CURRENT;
byte      TEMPERATURE_TARGET;
byte      FADE;
byte      TEMPERATURE_AUTO_MIN;
byte      TEMPERATURE_AUTO_MAX;
byte      TEMPERATURE_MAX         = 45;

byte      COUNTER_RPM             = 0;

unsigned long LAST_TIMER          = 0;

void setup() {

  SELECTED_LED = 0;
  SELECTED_FAN = 0;
  TEMPERATURE_CURRENT = 0;
  TEMPERATURE_TARGET = 0;
  
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Serial.println("OK!");

  analogWrite(FAN_CONTROL, 0);
  
  /*
  pinMode(FAN_CONTROL, OUTPUT);
  pinMode(FAN_RPM, INPUT);
  digitalWrite(FAN_RPM, HIGH);
  attachInterrupt(INTERRUPT_RPM, vRPM_FAN, FALLING);
  */
  
}

void receiveEvent(int bytes) {

  if (bytes != 4) {
    while ( Wire.available() ) {
      Wire.read();
    }
    return;
  }

  if ( Wire.available() ) {
    SELECTED_LED = Wire.read();

    if ( Wire.available() ) {
      SELECTED_FAN = Wire.read();  

      if ( Wire.available() ) {
        TEMPERATURE_CURRENT = Wire.read();
        
        if ( Wire.available() ) {
          TEMPERATURE_TARGET = Wire.read();
        }
      }
    }
  }

}

void loop() {

  if ( SELECTED_FAN == 0 ) {
    analogWrite(FAN_CONTROL, 0);
  }

  if ( SELECTED_FAN == 1 ) {

    TEMPERATURE_AUTO_MIN = TEMPERATURE_TARGET;
    TEMPERATURE_AUTO_MAX = TEMPERATURE_TARGET + TEMP_DIFF_HIGH;
    
    if ( TEMPERATURE_CURRENT < TEMPERATURE_TARGET ) {
      analogWrite(FAN_CONTROL, 0);
    } else
    if ( TEMPERATURE_CURRENT >= TEMPERATURE_TARGET && TEMPERATURE_CURRENT <= TEMPERATURE_AUTO_MAX ) {
      FADE = map(TEMPERATURE_CURRENT, TEMPERATURE_AUTO_MIN, TEMPERATURE_AUTO_MAX, LOWEST_FAN_SPEED, HIGHEST_FAN_SPEED);
      analogWrite(FAN_CONTROL, FADE);
    } else
    if ( TEMPERATURE_CURRENT > TEMPERATURE_AUTO_MAX ) {
      analogWrite(FAN_CONTROL, HIGHEST_FAN_SPEED);
    }

  } 
  
  if ( SELECTED_LED_LAST != SELECTED_LED ) {
    
    if ( SELECTED_LED == 0 ) {
      FADE = 0;
    } else
    if ( SELECTED_LED == 1 ) {
      FADE = 88;
    } else
    if ( SELECTED_LED == 2 ) {
      FADE = 255;
    }     
    analogWrite(LED_PIN_R, FADE);
    analogWrite(LED_PIN_G, FADE);
    analogWrite(LED_PIN_B, FADE);

    SELECTED_LED_LAST = SELECTED_LED;
  }

  if (millis() - LAST_TIMER >= UPDATE_TIMER) {
    //detachInterrupt(0);
    //Serial.println(COUNTER_RPM * (60 / 1));
    //COUNTER_RPM = 0;
    //LAST_TIMER = millis();
    //attachInterrupt(0, vRPM_FAN, FALLING);
  }
  
}

void vRPM_FAN() {
  COUNTER_RPM++;
}
