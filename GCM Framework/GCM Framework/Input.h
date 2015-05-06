/******************************************************************************
Class:Input
Implements:
Author:Rich Davison
Description: Wrapper for the PS3 libpad library. Provides static access to all
of the attached controllers. Most of the functionality of this class is covered
in the Joypad tutorial text, but this year, has been updated to also have the
ButtonDown function, which should be familiar to anyone who used the Mouse 
class in the graphics framework. All of the functions have an additional, 
optional variable, to set which pad to query the state of, defaulting to the
first joypad.

Also, the PadButtons enumerator is actually correct this year.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <cell/pad.h>

/*
In the PC Framework, the Mouse function SetAbsolutePositionBounds was used
by the Window class to update the bounds. If you get round to modifying the
screen resolution of your PS3 games, replicating the Mouse class function,
and promoting SCREENX and SCREENY to static member variables of Input would
be a good idea.
*/
#define SCREENX 720
#define SCREENY 480
#define DEADZONE 2.0f
/*
Sensitivity could be define as a member variable of the Joypad...
*/
#define SENSITIVITY 0.045f //0.08f

//Really simple type definition, to define the type of function that the
//joypad can store function pointers to.
typedef void(*InputFunction)();

/*
New! An enum to easily differentiate the joypads. JOYPAD_SIZE is
our handy little enum to make arrays large enough, while the extra
enum JOYPAD_NONE can be used by classes to mark the lack of a joypad,
see the CameraNode class for an example of this.
*/
enum JoyPadNum {
	JOYPAD_A,
	JOYPAD_B,
	JOYPAD_C,
	JOYPAD_D,
	JOYPAD_SIZE,
	JOYPAD_NONE
};

/*
Another enumerator, again capped with a 'size' marker. These are ordered
in the same way as the Sony libpad API, to make it nice and easy to iterate
over their structures and update the Input class. See the tutorial text!
*/
enum PadButtons {
	INPUT_SELECT = 0,
	INPUT_L3	 = 1,
	INPUT_R3	 = 2,
	INPUT_START  = 3,
	INPUT_UP	 = 4,
	INPUT_RIGHT	 = 5,
	INPUT_DOWN	 = 6,
	INPUT_LEFT	 = 7,
	INPUT_L2	 = 8,
	INPUT_R2	 = 9,
	INPUT_L1	 = 10,
	INPUT_R1	 = 11,
	INPUT_TRIANGLE  = 12,
	INPUT_CIRCLE	= 13,
	INPUT_CROSS		= 14,
	INPUT_SQUARE	= 15,
	INPUT_LANALOGUE = 16,
	INPUT_SIZE
};


class Input	{
public:
	static void Initialise();	//Initialise libpad, so we can access joypads
	static void Destroy();		//Destroy libpad, we're done with it!
	static void UpdateJoypad();	//Update the joypad states, call once a frame.

	/*
	I can't remember exactly why I decided not to make these just return a Vector2
	of some sort, possibly because it'd mean importing yet another Sony class...

	In any case, we can access the state of all of the joypads via these four static
	functions. Note that by default, these functions will work on the first joypad,
	via a default member variable. 
	*/
	static void GetPointerPosition(float &x, float &y, JoyPadNum num = JOYPAD_A);
	static void GetJoypadMovement( float &x, float &y, JoyPadNum num = JOYPAD_A); 
	static void SetPadFunction(PadButtons button, InputFunction function, JoyPadNum num = JOYPAD_A);

	static bool	ButtonDown(PadButtons button, JoyPadNum num = JOYPAD_A );
protected:
	/*
	This year, to support multiple joypads, most of the member variables from
	last year's Input class have been moved into this struct, so we can support
	multiple joypads easily. See the joypad tutorial text for how each of these
	work, if you can't work it out from their names. We don't need any other 
	class to be able to access this struct, everything is accessed through the
	static functions of the Input class, so we hide it in the class declaration
	itself, protected, so it can't be instantiated, or otherwise messed around
	with.
	*/
	struct JoyPad {
		InputFunction functions[INPUT_SIZE];

		bool buttons[INPUT_SIZE];

		float pointerPosX;
		float pointerPosY;

		float pointerRelativeX;
		float pointerRelativeY;

		JoyPad() {
			pointerPosX = 0.0f;
			pointerPosY = 0.0f;

			pointerRelativeX = 0.0f;
			pointerRelativeY = 0.0f;

			for(int i = 0; i < INPUT_SIZE; ++i) {
				buttons[i] = NULL;
			}

			for(int i = 0; i < INPUT_SIZE; ++i){
				functions[i] = NULL;
			}
		}
	};

	//Some joypads. 
	static JoyPad joypads[JOYPAD_SIZE];
};
