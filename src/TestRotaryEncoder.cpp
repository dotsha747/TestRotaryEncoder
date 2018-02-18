/*
 This file is part of TestRotaryEncoder
 A C++ Demonstration of using Dual-Concentric Rotary Encoders
 with Push Button to control the NAV radios on X-Plane's
 G1000 panel.

 See Library to access X-Plane via the ExtPlane Plugin for details.

 Copyright (C) 2018 shahada abubakar <shahada@abubakar.net>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

#include "wiringPi.h"
#include <XPlaneExtPlaneClient/ExtPlaneClient.h>
#include <iostream>
#include <stdlib.h>
#include <functional>

using namespace std;
using namespace XPlaneExtPlaneClient;

#define pinIA 15
#define pinIB 16
#define pinOA 4
#define pinOB 5
#define pinIS 6

ExtPlaneClient * extPlaneClient;

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
			extPlaneClient->sendLine("cmd once sim/GPS/g1000n1_com_inner_up");
		} else {
			cout << "I Anti-Clockwise" << endl;
			extPlaneClient->sendLine("cmd once sim/GPS/g1000n1_com_inner_down");
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
			extPlaneClient->sendLine("cmd once sim/GPS/g1000n1_com_outer_up");
		} else {
			cout << "O Anti-Clockwise" << endl;
			extPlaneClient->sendLine("cmd once sim/GPS/g1000n1_com_outer_down");
		}
	}

	// determine if idle or not now
	encoder_O_idle = a == 1 && b == 1;

}

// PUSH BUTTON

void push_button_ISR () {
	cout << "Button Pressed" << endl;
	extPlaneClient->sendLine("cmd once sim/GPS/g1000n1_com12");
}

// XPlane ExtPlane callbacks

void onExtPlaneConnect () {
	cout << "Connected to X-Plane via ExtPlane" << endl;
}

void onExtPlaneDisconnect () {
	cout << "Disconnected from X-Plane via ExtPlane" << endl;
}


int main(int argc, char * argv[]) {

	if (argc != 2) {
		cerr << "testRotaryEncoder <XPlaneIP>" << endl;
		exit (1);
	}

	// initialize wiringPi
	wiringPiSetup ();

	// setup ExtPlaneClient
	cout << "Connecting to X-Plane ExtPlane service at " << argv[1] << " 51000" << endl;

	extPlaneClient = new ExtPlaneClient(argv[1],
			51000,
			std::bind (&onExtPlaneConnect),
			std::bind (&onExtPlaneDisconnect),
			NULL
	);

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
