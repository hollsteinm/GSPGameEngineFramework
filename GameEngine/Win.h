//(c) copyright Martin Hollstein

#include <Windows.h>

#ifndef CWIN_H
#define CWIN_H

class CWin
{
public:
	//Constructor
	CWin(void);

	//Destructor
	~CWin(void);

	//setters
	void setWindowed(bool);

	//getters
	int getWindowWidth();
	int getWindowHeight();
	bool isWindowed();
	HWND getHWND();
	
	//const getters
	int const getWindowHeight() const;
	int const getWindowWidth() const;
	bool const isWindowed() const;
	HWND const getHWND() const;
	
	//Name:InitWindow()
	//Purpose: initializes main windows window
	//Return: bool, true on succes, false on failure
	bool InitWindow(HWND, HINSTANCE, const int&, const int&, LPWSTR);

	//Name: ToggleFullscreen
	//Purpose: Toggles the application to run in windowed or fullscreen mode
	//return: none
	void ToggleFullscreen();

	//Name: Shutdown()
	//Purpose: cleans up all memory and 
	//return: none
	void Shutdown();

private:
	//Name: Copy Constructors
	//Purpose: class is not intended to have more than one instance
	//Return: none
	CWin(CWin&);

	//private members
	int mWindowWidth;
	int mWindowHeight;
	HWND mhWnd;
	HINSTANCE mInstance;
	LPWSTR mName;
	int mWindowX;
	int mWindowY;
	bool bWindowed;
};

#endif
