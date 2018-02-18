//============================================================================
// Name        : TestRotaryEncoder.cpp
// Author      : Shahada Abubakar
// Version     :
// Copyright   : Copyright (c) 2014, NEXTSense Sdn Bhd
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "wiringPi.h"

#include <iostream>

using namespace std;

#define pinIA 15
#define pinIB 16
#define pinOA 4
#define pinOB 5
#define pinIS 6

// ENCODER I

bool encoder_I_idle = true;

void encoder_I_ISR () {

	int a = digitalRead (pinIA);
	int b = digitalRead (pinIB);

	// were we idle before this?
	if (encoder_I_idle) {
		// check our transition
		if (a == 0) {
			cout << "I Clockwise" << endl;
		} else {
			cout << "I Anti-Clockwise" << endl;
		}
	}

	// determine if idle or not now
	encoder_I_idle = a == 1 && b == 1;

}

// ENCODER O

bool encoder_O_idle = true;

void encoder_O_ISR () {

	int a = digitalRead (pinOA);
	int b = digitalRead (pinOB);

	// were we idle before this?
	if (encoder_O_idle) {
		// check our transition.This is reversed for
		// the outer encoder due to the gears used
		// to transfer motion.
		if (a == 1) {
			cout << "O Clockwise" << endl;
		} else {
			cout << "O Anti-Clockwise" << endl;
		}
	}

	// determine if idle or not now
	encoder_O_idle = a == 1 && b == 1;

}

// PUSH BUTTON

void push_button_ISR () {
	cout << "Button Pressed" << endl;
}


int main() {

	// initialize wiringPi
	wiringPiSetup ();

	// set pins for input
	pinMode (pinIA, INPUT);
	pinMode (pinIB, INPUT);
	pinMode (pinOA, INPUT);
	pinMode (pinOB, INPUT);
	pinMode (pinIS, INPUT);

	// set pull-up mode
	pullUpDnControl (pinIA, PUD_UP);
	pullUpDnControl (pinIB, PUD_UP);
	pullUpDnControl (pinOA, PUD_UP);
	pullUpDnControl (pinOB, PUD_UP);
	pullUpDnControl (pinIS, PUD_UP);

	// set up interrupts
	wiringPiISR (pinIA, INT_EDGE_BOTH, &encoder_I_ISR);
	wiringPiISR (pinIB, INT_EDGE_BOTH, &encoder_I_ISR);
	wiringPiISR (pinOA, INT_EDGE_BOTH, &encoder_O_ISR);
	wiringPiISR (pinOB, INT_EDGE_BOTH, &encoder_O_ISR);
	wiringPiISR (pinIS, INT_EDGE_FALLING, &push_button_ISR);

	cout << "Press Control-C to exit." << endl;
	while (1) {
		delay (1000);
	}
}
