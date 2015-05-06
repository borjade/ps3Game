#include "Input.h"

//Static member variables need defining!
Input::JoyPad Input::joypads[JOYPAD_SIZE];

/*
Even if we're only assuming a single joypad is to be connected,
we may as well initialise enough for a multiplayer game
*/
void Input::Initialise() {
	if(cellPadInit(JOYPAD_SIZE) != CELL_PAD_OK) {
		std::cout << "cellPadInit failed!" << std::endl;	
	}
}

/*
Closes libpad
*/
void Input::Destroy() {
	cellPadEnd();
}
/*
Gets the pointer position of the requisite joypad. Handy for debug
code / menus etc. 
*/
void Input::GetPointerPosition(float &x, float &y, JoyPadNum num) {
	x = joypads[num].pointerPosX;
	y = joypads[num].pointerPosY;
}
/*
Gets the relative movement of the analogue sticks. As the code currently
stands, both analogue sticks will effect this value, but it should be
trivial for you to refactor the class to separate this out into two sets
of member variables / get functions. 
*/
void Input::GetJoypadMovement(float &x, float &y, JoyPadNum num) {
	x = joypads[num].pointerRelativeX;
	y = joypads[num].pointerRelativeY;
}

/*
Sets a function that will be called whenever the provided button is set. 
As the code currently is, this function will be called every frame that the
button is down for - if you want this to be a 'trigger' action (only called the
first frame the button is down for), you'll maybe want to take a look at how I
handle key and mouse presses over multiple frames in the graphics framework
(look for holdstates and holdbuttons, and how they are used).
*/
void Input::SetPadFunction(PadButtons button, InputFunction function, JoyPadNum num){
	joypads[num].functions[button] = function;
}

/*
Returns true if the passed button is pressed down, false otherwise.
*/
bool Input::ButtonDown(PadButtons button, JoyPadNum num) {
	return joypads[num].buttons[button];
}

/*
Updates the Input class internal states. Call this once per frame,
in the main game loop. 
*/
void Input::UpdateJoypad() {
	CellPadData data;
	data.len = 0;

	for(int p = 0; p < JOYPAD_SIZE; ++p) {
		cellPadGetData(p,&data);

		int halfInput = INPUT_SIZE / 2;

		if(data.len > 0) {			//Pad status has changed...
			//if(data.button[2]) {
				for(int i = 0; i < halfInput; ++i) {
					if(data.button[2] & (1 << i)) {
						joypads[p].buttons[i] = true;
						if(joypads[p].functions[i]) {
							joypads[p].functions[i]();
						}
					}
					else{
						joypads[p].buttons[i] = false;
					}
				}
			//}
			//if(data.button[3]) {
				for(int i = 0; i < halfInput; ++i) {
					if(data.button[3] & (1 << i)) {
						joypads[p].buttons[halfInput+i] = true;
						if(joypads[p].functions[halfInput+i]) {					
							joypads[p].functions[halfInput+i]();
						}
					}
					else{
						joypads[p].buttons[halfInput+i] = false;
					}
				}
			//}

			//joypads[p].pointerRelativeX = 0.0f;
			//joypads[p].pointerRelativeY = 0.0f;

			//Check the left analogue stick, X axis, then Y.
			//if(data.button[6]){ 
				joypads[p].pointerRelativeX = -127.5f+data.button[6];

				joypads[p].pointerRelativeX *= SENSITIVITY;

				if(fabs(joypads[p].pointerRelativeX) < DEADZONE) {
					joypads[p].pointerRelativeX = 0;
				}
			//}
			//if(data.button[7]){ 
				joypads[p].pointerRelativeY = -127.5f+data.button[7];

				joypads[p].pointerRelativeY *= SENSITIVITY;

				if(fabs(joypads[p].pointerRelativeY) < DEADZONE) {
					joypads[p].pointerRelativeY = 0;
				}
			//}

			//New! Checks the right analogue stick, X axis, then Y. 
			//Why is this if statement slightly different than the left analogue
			//statement? Because I sometimes got invalid data inside the struct,
			//that meant I got massively incorrect values, and a wildly spinning
			//camera as a result. The API spec doesn't seem to indicate why this
			//should be, so just put it down to Sony being special.
			//if(data.button[4] > 0){ 
				joypads[p].pointerRelativeX = -127.5f+data.button[4];

				joypads[p].pointerRelativeX *= SENSITIVITY;

				if(fabs(joypads[p].pointerRelativeX) < DEADZONE) {
					joypads[p].pointerRelativeX = 0;
				}
			//}
			//if(data.button[5] > 0 && data.button[5] < 256){ 
				joypads[p].pointerRelativeY = -127.5f+data.button[5];

				joypads[p].pointerRelativeY *= SENSITIVITY;

				if(fabs(joypads[p].pointerRelativeY) < DEADZONE) {
					joypads[p].pointerRelativeY = 0;
				}
			//}
		}

		joypads[p].pointerPosX  += (joypads[p].pointerRelativeX );
		joypads[p].pointerPosY  += (joypads[p].pointerRelativeY );

		if(joypads[p].pointerPosX < 0) {
			joypads[p].pointerPosX = 0;
		}
		else if(joypads[p].pointerPosX > SCREENX) {
			joypads[p].pointerPosX = SCREENX;
		}

		if(joypads[p].pointerPosY < 0) {
			joypads[p].pointerPosY = 0;
		}
		else if(joypads[p].pointerPosY > SCREENY) {
			joypads[p].pointerPosY = SCREENY;
		}

		cellPadClearBuf(p);
	}
};

