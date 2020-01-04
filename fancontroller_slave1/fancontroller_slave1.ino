#include  <Wire.h>

#define   LED_PIN_R   5
#define   LED_PIN_G   6
#define   LED_PIN_B   9

#define   FAN_PIN     11

byte      SELECTED_TYPE;
byte      SELECTED_OPTION;
byte      TEMPERATURE_CURRENT;
byte      FADE;
byte      SELECTED_OPTION_NEW;

void setup() {

  SELECTED_TYPE = 0;
  SELECTED_OPTION = 0;
  TEMPERATURE_CURRENT = 0;
  
  Serial.begin(9600);
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Serial.println("OK!");

  analogWrite(FAN_PIN, 0);
  
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
    analogWrite(FAN_PIN, FADE);
  }

  if ( SELECTED_TYPE == 0 && SELECTED_OPTION == 4 ) {

    if ( TEMPERATURE_CURRENT <= 30 ) {
      SELECTED_OPTION_NEW = 1;   
    } else
    if ( TEMPERATURE_CURRENT > 30 && TEMPERATURE_CURRENT <= 45 ) { 
      SELECTED_OPTION_NEW = 2;   
    } else    
    if ( TEMPERATURE_CURRENT > 45 ) { 
      SELECTED_OPTION_NEW = 3;
    }
      
    FADE = map(SELECTED_OPTION_NEW, 0, 3, 0, 255);
    analogWrite(FAN_PIN, FADE);

  } 
  
  if ( SELECTED_TYPE == 1 ) {
    FADE = map(SELECTED_OPTION, 0, 4, 0, 255);
    analogWrite(LED_PIN_R, FADE);
    analogWrite(LED_PIN_G, FADE);
    analogWrite(LED_PIN_B, FADE);
  }
  
}
