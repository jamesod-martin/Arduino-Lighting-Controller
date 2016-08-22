/*
 * LightingBoard.h
 *
 *  Created on: Aug 18, 2016
 *      Author: jamesod
 */

#include "Arduino.h"
#include <Wire.h>


#ifndef LIGHTINGBOARD_H_
#define LIGHTINGBOARD_H_

#define LED1 13
#define LED2 12
#define LED3 11
#define LED4 10
#define LED5 9
#define LED6 8
#define LED7 7
#define LED8 6
#define LED9 5
#define LED10 4
#define LED11 3
#define LED12 2
#define LED13 44
#define LED14 45

#define OE 34

#define PLC1 36
#define PLC2 38
#define PLC3 40
#define PLC4 42

#define PWMB_ADDRESS 0x40

class Board{
public:
	Board();
	~Board();
	bool init();
	byte settings;
	int debug = 0;
	int inverted = 0;
	int numPulses = 1;
	int maxPosition = 1;
	int maxBrightness = 250;
	int minBrightness = 50;
	int LEDValues[28] = {0};
	int pulseWidth = 7;
	int LEDAddresses[28] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9, LED10, LED11, LED12, LED13, LED14,
							0x06, 0x0A, 0x0E, 0x12, 0x16, 0x1A, 0x1E, 0x22, 0x26, 0x2A, 0x2E, 0x32, 0x36, 0x3A};
	bool readInputs();
	bool processNewSettings();
	int LEDSpeed = 500;		// ms delay before updating LEDs
	bool newInput = 0;
	bool setLEDs();
	bool setI2CLED(int id, int value);
	bool getNewLEDValues();
private:

};


#endif /* LIGHTINGBOARD_H_ */
