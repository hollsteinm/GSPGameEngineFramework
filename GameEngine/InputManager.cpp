//(c) copyright Martin Hollstein

#include "InputManager.h"
#include "Log.h"

InputManager* InputManager::mInstance = 0;

InputManager* InputManager::getInstance()
{
	if(!mInstance)
		mInstance = new InputManager;

	return mInstance;
}

InputManager::InputManager(void)
{
	ZeroMemory(mKeyBoardState, sizeof(mKeyBoardState));
	ZeroMemory(&mMouseState, sizeof(mMouseState));

	mphWnd = 0;
	mphInstance = 0;

	for(int i = 0; i < 256; ++i)
	{
		mKeyPress[i] = false;
	}

	for(int i = 0; i < NUM_BUTTONS; ++i)
	{
		mButtonPress[i] = false;
	}

	Log::Inform("Input Manager Created", "InputManager", this);
}

InputManager::~InputManager(void)
{
}

void InputManager::Init(HWND* hWnd, HINSTANCE* hInstance)
{
	mphWnd = hWnd;
	mphInstance = hInstance;

	//zero out the memory
	ZeroMemory(mKeyBoardState, sizeof(mKeyBoardState));
	ZeroMemory(&mMouseState, sizeof(mMouseState));

	//create the device
	DirectInput8Create(*mphInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDInput, NULL);

	//set the objects and create devices
	mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);
	mDInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);

	//set the data format
	mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	mMouse->SetDataFormat(&c_dfDIMouse2);

	//set the cooperative level
	mKeyboard->SetCooperativeLevel(*mphWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	mMouse->SetCooperativeLevel(*mphWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//acquire
	mKeyboard->Acquire();
	mMouse->Acquire();

	Log::Inform("Initialized", "InputManager", this);
}

void InputManager::poll()
{
	//poll keyboard
	HRESULT hr = mKeyboard->GetDeviceState(sizeof(mKeyBoardState), (void**)&mKeyBoardState);

	if(FAILED(hr))
	{
		Log::Error("Could not Get Device Keyboard", "InputManager", this);

		//keyboard lost, zero out the structure
		ZeroMemory(mKeyBoardState, sizeof(mKeyBoardState));

		//attempt to acquire
		hr = mKeyboard->Acquire();
	}

	//poll mouse
	hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState);

	if(FAILED(hr))
	{
		Log::Error("Could not Get Device Mouse", "InputManager", this);

		//lost the mouse? zero out memory
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		//try to acquire mouse for next poll
		hr = mMouse->Acquire();
	}
}

float InputManager::mouseDX()
{
	return (float)mMouseState.lX;
}

float InputManager::mouseDY()
{
	return (float)mMouseState.lY;
}

float InputManager::mouseDZ()
{
	return (float)mMouseState.lZ;
}

bool InputManager::mouseButtonDown(int button)
{
	//return boolean of button pressed int he DIMOUSESTATE2 struct
	return (mMouseState.rgbButtons[button] & 0x80) != 0;
}

bool InputManager::keyDown(unsigned char key)
{
	//return if the passed key value in dinput.h is down
	return(mKeyBoardState[key] & 0x80) != 0;
}

void InputManager::Shutdown()
{
	//release COM devices
	if(mDInput)
		mDInput->Release();

	//unaquire mouse and keyboard...
	mKeyboard->Unacquire();
	mMouse->Unacquire();

	//...then release and nullify
	if(mKeyboard)
		mKeyboard->Release();

	if(mMouse)
		mMouse->Release();
}

bool InputManager::keyPress(unsigned char key)
{
	if(!mKeyPress[key])
	{
		if(keyDown(key))
		{
			mKeyPress[key] = true;
			return keyDown(key);
		}
	}
	else
	{
		if(!keyDown(key))
		{
			mKeyPress[key] = false;
			return false;
		}
		else
		{
			return false;
		}
	}
}

bool InputManager::keyRelease(unsigned char key)
{
	if(mKeyPress[key])
	{
		if(!keyDown(key))
		{
			mKeyPress[key] = false;
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool InputManager::mouseButtonPress(int button)
{
	if(!mButtonPress[button])
	{
		if(mouseButtonDown(button))
		{
			mButtonPress[button] = true;
			return mouseButtonDown(button);
		}
	}
	else
	{
		if(!mouseButtonDown(button))
		{
			mButtonPress[button] = false;
			return false;
		}
		else
		{
			return false;
		}
	}
}

bool InputManager::mouseButtonRelease(int button)
{
	if(mButtonPress[button])
	{
		if(!mouseButtonDown(button))
		{
			mButtonPress[button] = false;
			return true;
		}
	}
	else
	{
		return false;
	}
}