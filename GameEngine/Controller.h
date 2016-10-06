//(c) copyright Martin Hollstein

#include "InputManager.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
	Controller(void);
	~Controller(void);

	float mouseDX();
	float mouseDY();
	float mouseDZ();

	bool mouseButtonDown(int button);
	bool keyDown(unsigned char key);

	bool mouseButtonPress(int button);
	bool mouseButtonRelease(int button);

	bool keyRelease(unsigned char key);
	bool keyPress(unsigned char key);
};

#endif