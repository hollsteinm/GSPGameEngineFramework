//(c) copyright Martin Hollstein

#include <Windows.h>

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define NUM_BUTTONS	16


class InputManager
{
public:
	~InputManager(void);
	static InputManager* getInstance();

	void Init(HWND*, HINSTANCE*);

	//poll is the Update() function
	void poll();

	bool keyDown(unsigned char key);
	bool mouseButtonDown(int button);
	
	float mouseDX();
	float mouseDY();
	float mouseDZ();

	void Shutdown();

	bool mouseButtonPress(int button);
	bool mouseButtonRelease(int button);

	bool keyRelease(unsigned char key);
	bool keyPress(unsigned char key);
	
private:
	//singleton methodology
	InputManager(void);
	InputManager(InputManager& other);

	//the input and input devices
	IDirectInput8*			mDInput;

	IDirectInputDevice8*	mKeyboard;
	unsigned char			mKeyBoardState[256];

	IDirectInputDevice8*	mMouse;
	DIMOUSESTATE2			mMouseState;

	//press and release states
	bool					mKeyPress[256];
	bool					mButtonPress[NUM_BUTTONS];

	//pointers to HWND and HINSTANCE
	HWND*					mphWnd;
	HINSTANCE*				mphInstance;

	//singleton instance pointer
	static InputManager*	mInstance;
};

#endif
