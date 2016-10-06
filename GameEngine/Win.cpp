//(c) copyright Martin Hollstein

#include "Win.h"


CWin::CWin(void)
{
}

CWin::~CWin(void)
{
}

LRESULT CALLBACK
	WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	default:
		{
			return DefWindowProc(hwnd, message, wparam, lparam);
		}
	}
}

void CWin::ToggleFullscreen()
{
	DEVMODE dmScreenSettings;

	if(!bWindowed)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)mWindowWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)mWindowHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		mWindowX = mWindowY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		mWindowWidth  = mWindowWidth;
		mWindowHeight = mWindowHeight;

		// Place the window in the middle of the screen.
		mWindowX = (GetSystemMetrics(SM_CXSCREEN) - mWindowWidth)  / 2;
		mWindowY = (GetSystemMetrics(SM_CYSCREEN) - mWindowHeight) / 2;
	}
}

bool CWin::InitWindow(HWND hwnd, HINSTANCE instance, const int& screenHeight, const int& screenWidth, LPWSTR windowTitle)
{
	WNDCLASSEX wc;
	mInstance = instance;
	mhWnd = hwnd;
	mName = windowTitle;

	mWindowWidth = screenWidth;
	mWindowHeight = screenHeight;
	
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	mWindowWidth = GetSystemMetrics(SM_CXSCREEN);
	mWindowHeight = GetSystemMetrics(SM_CYSCREEN);

	ToggleFullscreen();

	mhWnd = CreateWindowEx(WS_EX_APPWINDOW, mName, mName, 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		mWindowX, mWindowY, mWindowWidth, mWindowHeight, NULL, NULL, mInstance, NULL);

	ShowWindow(mhWnd, SW_SHOW);
	SetForegroundWindow(mhWnd);
	SetFocus(mhWnd);

	ShowCursor(false);

	return true;
}

void CWin::Shutdown()
{
	UnregisterClass(mName, mInstance);
}

CWin::CWin(CWin&)
{
}

void CWin::setWindowed(bool isWindowed)
{
	bWindowed = isWindowed;
}

int CWin::getWindowWidth()
{
	return mWindowWidth;
}

int CWin::getWindowHeight()
{
	return mWindowHeight;
}

bool CWin::isWindowed()
{
	return bWindowed;
}

HWND CWin::getHWND()
{
	return mhWnd;
}

int const CWin::getWindowHeight() const
{
	return mWindowHeight;
}

int const CWin::getWindowWidth() const
{
	return mWindowWidth;
}

bool const CWin::isWindowed() const
{
	return bWindowed;
}

HWND const CWin::getHWND() const
{
	return mhWnd;
}
