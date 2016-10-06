//(c) copyright Martin Hollstein

#include <iostream>
#include <Windows.h>
#include "DXManager.h"
#include "Win.h"
#include "Timer.h"
#include <ctime>
#include "World.h"
#include "InputManager.h"
//#include <vld.h>

#define VC_LEAN_AND_MEAN

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//set seed for psuedo RGN
	srand((unsigned int)time(NULL));

	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	HWND hWnd = 0;

	//TODO: load player resolution and window-notwindowed settings.
	
	static unsigned int width = 1920;
	static unsigned int height = 1080;

	Log::BeginRunTimeStamp();

	CWin App;
	if(!App.InitWindow(hWnd, hInstance, width, height, L"Martin's Game Engine"))
	{
		Log::Error("Failed to initialize window", "App", &App);
		MessageBox(App.getHWND(), L"Failed to initialize window handle", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	App.setWindowed(false);

	//TODO: implement singleton design patter for the CDXManager but ensure it is initialized here
	CDXManager* DX10 = CDXManager::instance();
	if(!DX10->Init(App.getHWND(), &width, &height, App.isWindowed()))
	{
		Log::Error("Failed to initialize CDXManager", "DX10", &DX10);
		MessageBox(App.getHWND(), L"Failed to initialize graphics", L"Error", MB_OK | MB_ICONERROR);
		return 2;
	}

	//Input
	InputManager* IMInstancePtr = InputManager::getInstance();
	IMInstancePtr->Init(&hWnd, &hInstance);

	World testWorld;
	testWorld.Load();

	CTimer timer;

	//Main Loop
	MSG msg; 
	ZeroMemory(&msg, sizeof(msg));
	
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		timer.Start();
		IMInstancePtr->poll();
		testWorld.Run(timer.dt());
		timer.End();
	}
	
	//reverse order
	IMInstancePtr->Shutdown();

	delete IMInstancePtr;

	testWorld.OnExit();
	DX10->Shutdown();
	App.Shutdown();

	return 0;
}