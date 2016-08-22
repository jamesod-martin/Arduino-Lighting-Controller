#include <LightingBoard.h>
#include <Wire.h> //I2C library
/*
Lighting controller 
20160818

Controls the lighting of 28LED strips:
14 LEDs are controlled through the PWM pins, 14 are controlled through I2C

*/

Board board;
unsigned long currentTime;
unsigned long lastUpdateTime = 0;
void setup() {
  Serial.begin(9600);
  board.init();
  board.pulseWidth = 3;    //width of each individual pulse
  board.maxBrightness = 250; //maximum brightness of individual strip. set to 0 for all off
  board.minBrightness = 0;  // lowest brightness if min = max all LEDs are on to the max brightness
  board.inverted = 0;    // set to 1 for inverted outputs (we are using inverted outputs.
  board.LEDSpeed = 100;  //ms between LED updates. lower is faster. if debugging is on this runs MUCH slower
  board.numPulses = 4;  //1, 2, or 4 pulses
  board.debug = 0;      //1 = serial data output through USB is on, 0 is off
}

void loop() {
  currentTime = millis();
  board.readInputs();
  if ((currentTime - lastUpdateTime) > board.LEDSpeed){
    board.getNewLEDValues();
    board.setLEDs();
    lastUpdateTime = currentTime;
    //Serial.println(board.settings);
    if(board.debug){
      for (int i = 0; i < 28; i += 7){
        Serial.print(board.LEDValues[i]); Serial.print(":");
        Serial.print(board.LEDValues[i + 1]); Serial.print(":");
        Serial.print(board.LEDValues[i + 2]); Serial.print(":"); 
        Serial.print(board.LEDValues[i + 3]); Serial.print(":"); 
        Serial.print(board.LEDValues[i + 4]); Serial.print(":"); 
        Serial.print(board.LEDValues[i + 5]); Serial.print(":");  
        Serial.print(board.LEDValues[i + 6]); Serial.println("");   
      }
    }
  }
}
