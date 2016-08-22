/*
 * LightingBoard.c++
 *
 *  Created on: Aug 18, 2016
 *      Author: jamesod
 */

#include "LightingBoard.h"


Board::Board()
{

}
//Destructor
Board::~Board()
{
}

bool Board::init(){
	Wire.begin();        //enables i2c comms
	//initialize pinModes:
	pinMode(PLC1, INPUT);
	pinMode(PLC2, INPUT);
	pinMode(PLC3, INPUT);
	pinMode(PLC4, INPUT);
	pinMode(OE, OUTPUT);
	setLEDs();
	digitalWrite(OE, LOW); //enables the I2C PWM outputs
	return 1;
}

//gets new input from PLC and if there is a settings change it calls processNewSettings
bool  Board::readInputs(){
	byte plcInput;
	int lastState;
	if(digitalRead(PLC4)){
		plcInput = digitalRead(PLC1);
		plcInput << 1;
		plcInput = digitalRead(PLC2);
		plcInput << 1;
		plcInput = digitalRead(PLC3);
		if (lastState == 0){
			newInput = 1;
		}
		lastState = 1;
	}else{
		newInput = 0;
		lastState = 0;
	}
	if (newInput == 1){
		settings = plcInput;
		processNewSettings();
		newInput = 0;
	}
	return 1;
}

//makes changes to the lighting variables based on the settings byte
bool Board::processNewSettings(){
	switch(settings){
		// 0: Increase Max Brightness
		case 0:
		{
			maxBrightness  += 10;
			if (maxBrightness > 255){
				maxBrightness = 255;
			}
			break;
		}
		// 1: Decrease Max Brightness
		case 1:
		{
			maxBrightness  -= 10;
			if (maxBrightness < 0){
				maxBrightness = 0;
			}
			break;
		}
		// 2: Increase Min Brightness
			case 2:
		{
			minBrightness  += 10;
			if (minBrightness > 255){
				minBrightness = 255;
			}
			break;
		}
		// 3: Decrease Min Brightness
			case 3:
		{
			minBrightness  -= 10;
			if (minBrightness < 0){
				minBrightness = 0;
			}
			break;
		}
		// 4: speed up
		case 4:
		{
			LEDSpeed -= 25;
			if (LEDSpeed < 100){
				LEDSpeed = 100;
			}
			break;
		}
		// 5: slow down
		case 5:
		{
			LEDSpeed += 25;
			if (LEDSpeed > 2000){
				LEDSpeed = 2000;
			}
			break;
		}
		// 6: increase pulse width
		case 6:
		{
			pulseWidth ++;
			if(pulseWidth > 29){  //29 is the max, turns all on to max brightness
				pulseWidth = 29;
			}
			break;
		}
		//7:
		case 7:
		{
			pulseWidth --;
			if(pulseWidth < 1){
				pulseWidth = 1;
			}
			break;
		}
	}
	if (maxBrightness < minBrightness){
		minBrightness = maxBrightness;
	}
	return 1;
}
//sets ALL LEDs to the value held in LEDValues
bool Board::setLEDs(){
	for (int i = 0; i < 14; i++){
		analogWrite(LEDAddresses[i], LEDValues[i]);
	}
	for (int i = 14; i < 27; i++){
		setI2CLED(i, LEDValues[i]);
	}
	newInput = 0;
	return 1;
}

//sets ONE I2C LED to the given value of brightness
bool Board::setI2CLED(int id, int value){
	int tempLowValue;
	int LEDOnLow = 0, LEDOnHigh = 0, LEDOffLow, LEDOffHigh;
	if (value >= 255){
		value = 255;
	}else if (value <= 0){
		value = 0;
	}
	value = value * 16;
	tempLowValue = 4095 - value;
	LEDOnLow = value & 0xFF;
	LEDOffLow = tempLowValue & 0xFF;
	LEDOnHigh = value >> 8;
	LEDOffHigh = tempLowValue >> 8;
	Wire.beginTransmission(PWMB_ADDRESS);
	Wire.write(byte(LEDAddresses[id]));
	Wire.write(byte(LEDOnLow));
	Wire.write(byte(LEDAddresses[id]+1));
	Wire.write(byte(LEDOnHigh));
	Wire.write(byte(LEDAddresses[id]+2));
	Wire.write(byte(LEDOffLow));
	Wire.write(byte(LEDAddresses[id]+3));
	Wire.write(byte(LEDOffHigh));
	Wire.endTransmission();
	return 1;
}

//determines the new values for LED brightness from the adjustable variables
bool Board::getNewLEDValues(){
	int tempValue;
	int maxPosition2;
	maxPosition++;
	if (maxPosition > 28){		//rolls over maxposition at the 28th LED
		maxPosition = 1;
	}
	int distance; //distance from maxPosition
	int brightnessInterval = (maxBrightness - minBrightness) / pulseWidth;
	for (int i = 0; i < 28; i++){

		// code for multiple pulses
		switch(numPulses){
		case 1:
			distance = abs(i - maxPosition);
			if (distance > 14){
				distance = 14 - (distance - 14);
			}
			break;
		case 2:
			maxPosition2 = maxPosition + 14;
			if (maxPosition2 > 27){
				maxPosition2 = abs(28-maxPosition2);
			}
			if(abs(i - maxPosition) < abs(i - maxPosition2)){
				distance = abs(i - maxPosition);
			}else{
				distance = abs(i - maxPosition2);
			}
			if(distance > 7){
				distance = 14 - distance;
			}
			break;
		case 3:
		case 4:
			maxPosition2 = maxPosition + 7;
			if (maxPosition2 > 27){
				maxPosition2 = abs(28-maxPosition2);
			}
			int maxPosition3 = maxPosition + 14;
			if (maxPosition3 > 27){
				maxPosition3 = abs(28-maxPosition3);
			}
			int maxPosition4 = maxPosition + 21;
			if (maxPosition4 > 27){
				maxPosition4 = abs(28-maxPosition4);
			}
			distance = abs(i - maxPosition);
			int dist2 = abs(i - maxPosition2);
			int dist3 = abs(i - maxPosition3);
			int dist4 = abs(i - maxPosition4);

			int minDistance = min(distance, min(dist2, min(dist3, dist4)));
			if (debug){
				Serial.print(distance); Serial.print(':');
				Serial.print(dist2); Serial.print(':');
				Serial.print(dist3); Serial.print(':');
				Serial.print(dist4); Serial.print(':');
				Serial.println(minDistance);
			}
			distance = minDistance;
			break;
		}

		LEDValues[i] = maxBrightness - (distance * brightnessInterval);
		if (LEDValues[i] < minBrightness){
			LEDValues[i] = minBrightness;
		}
		if (LEDValues[i] > maxBrightness){
			LEDValues[i] = maxBrightness;
		}
		if(debug){
			Serial.print(distance); Serial.print(':'); Serial.print(LEDValues[i]); Serial.print(':'); Serial.println(brightnessInterval);
		}
		if (inverted == 1){
			LEDValues[i] *=-1;
		}
	}
	return 1;
}
