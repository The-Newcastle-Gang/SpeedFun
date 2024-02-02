#include "Window.h"
using namespace NCL;
struct InputStruct {
	float lrMoveInput; //-1 is left, 1 is right
	float fbMoveInput; //-1 is back, 1 is forward
	float horizontalCameraInput; //-1 is left, 1 is right
	float verticalCameraInput; //-1 is down, 1 is up
	bool jumpPressed;
	bool abilityPressed;
	bool startPressed;
	bool backPressed;
	bool actionOnePressed;//left click on PC, right trigger on PS5
	bool actionTwoPressed;//right click on PC, left trigger on PS5;

};

class InputGetter {
	virtual InputStruct GetInput() = 0; //must be implemented
};

class InputGetterPC : public InputGetter {
	InputStruct GetInput() override {
		InputStruct input;
		//get inputs from kb & m, return an input struct
		input.lrMoveInput = (int)Window::GetKeyboard()->KeyDown(KeyboardKeys::A) - (int)Window::GetKeyboard()->KeyDown(KeyboardKeys::D);
		input.fbMoveInput = (int)Window::GetKeyboard()->KeyDown(KeyboardKeys::W) - (int)Window::GetKeyboard()->KeyDown(KeyboardKeys::S);

		input.horizontalCameraInput = Window::GetMouse()->GetRelativePosition().x;
		input.verticalCameraInput = Window::GetMouse()->GetRelativePosition().y;

		input.jumpPressed = Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE);
		input.abilityPressed = Window::GetKeyboard()->KeyPressed(KeyboardKeys::E);
		input.startPressed = Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN);
		input.backPressed = Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE);

		input.actionOnePressed = Window::GetMouse()->ButtonPressed(MouseButtons::LEFT);
		input.actionTwoPressed = Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT);

		return input;
	}
};

//missing implementation
class InputGetterPS5 : public InputGetter {
	InputStruct GetInput() override {
		InputStruct input;
		//get inputs from ps5 (will need the API first)
		return input;
	}
};