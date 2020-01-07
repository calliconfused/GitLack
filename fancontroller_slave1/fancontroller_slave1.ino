#include  <Wire.h>

#define   LED_PIN_R               5
#define   LED_PIN_G               6
#define   LED_PIN_B               9

#define   FAN_CONTROL             11
#define   FAN_RPM                 3
#define   INTERRUPT_RPM           0
#define   UPDATE_TIMER            500

byte      SELECTED_TYPE;
byte      SELECTED_OPTION;
byte      TEMPERATURE_CURRENT;
byte      TEMPERATURE_AUTO_MIN    = 22;
byte      TEMPERATURE_AUTO_MAX    = 35;
byte      FADE;
byte      SELECTED_OPTION_NEW;

byte      COUNTER_RPM             = 0;

unsigned long LAST_TIMER          = 0;

void setup() {

  SELECTED_TYPE = 0;
  SELECTED_OPTION = 0;
  TEMPERATURE_CURRENT = 0;
  
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Serial.println("OK!");

  analogWrite(FAN_CONTROL, 0);

  pinMode(FAN_CONTROL, OUTPUT);
  pinMode(FAN_RPM, INPUT);
  digitalWrite(FAN_RPM, HIGH);
  attachInterrupt(INTERRUPT_RPM, vRPM_FAN, FALLING);
  
}

void receiveEvent(int bytes) {

  if (bytes != 3) {
    while ( Wire.available() ) {
      Wire.read();
    }
    return;
  }

  if ( Wire.available() ) {
    SELECTED_TYPE = Wire.read();

    if ( Wire.available() ) {
      SELECTED_OPTION = Wire.read();  

      if ( Wire.available() ) {
        TEMPERATURE_CURRENT = Wire.read();
        
      }
    }
  }
  
  Serial.print("Receive:");
  Serial.print("\tType: \t");
  Serial.print(SELECTED_TYPE);
  Serial.print("\tOption: \t");
  Serial.print(SELECTED_OPTION);
  Serial.print("\tCelcius: \t");
  Serial.println(TEMPERATURE_CURRENT);
}

void loop() {

  if ( SELECTED_TYPE == 0 && SELECTED_OPTION < 4 ) {
    FADE = map(SELECTED_OPTION, 0, 3, 0, 255);
    analogWrite(FAN_CONTROL, FADE);
  }

  if ( SELECTED_TYPE == 0 && SELECTED_OPTION == 4 ) {      
    FADE = map(TEMPERATURE_CURRENT, TEMPERATURE_AUTO_MIN, TEMPERATURE_AUTO_MAX, 63, 255);
    analogWrite(FAN_CONTROL, FADE);

  } 
  
  if ( SELECTED_TYPE == 1 ) {
    FADE = map(SELECTED_OPTION, 0, 4, 0, 255);
    analogWrite(LED_PIN_R, FADE);
    analogWrite(LED_PIN_G, FADE);
    analogWrite(LED_PIN_B, FADE);
  }

  if (millis() - LAST_TIMER >= UPDATE_TIMER) {
    detachInterrupt(0);
    Serial.println(COUNTER_RPM * (60 / 1));
    COUNTER_RPM = 0;
    LAST_TIMER = millis();
    attachInterrupt(0, vRPM_FAN, FALLING);
    
  }
  
}

void vRPM_FAN() {
  COUNTER_RPM++;
}

