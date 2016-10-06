//(c) copyright Martin Hollstein

#include "Controller.h"

Controller::Controller(void)
{
}

Controller::~Controller(void)
{
}

float Controller::mouseDX()
{
	return InputManager::getInstance()->mouseDX();
}

float Controller::mouseDY()
{
	return InputManager::getInstance()->mouseDY();
}

float Controller::mouseDZ()
{
	return InputManager::getInstance()->mouseDZ();
}

bool Controller::mouseButtonDown(int button)
{
	return InputManager::getInstance()->mouseButtonDown(button);
}

bool Controller::keyDown(unsigned char key)
{
	return InputManager::getInstance()->keyDown(key);
}

bool Controller::mouseButtonPress(int button)
{
	return InputManager::getInstance()->mouseButtonPress(button);
}

bool Controller::mouseButtonRelease(int button)
{
	return InputManager::getInstance()->mouseButtonRelease(button);
}

bool Controller::keyRelease(unsigned char key)
{
	return InputManager::getInstance()->keyRelease(key);
}

bool Controller::keyPress(unsigned char key)
{
	return InputManager::getInstance()->keyPress(key);
}