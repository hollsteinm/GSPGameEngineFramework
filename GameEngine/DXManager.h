//(c) copyright Martin Hollstein

#include <Windows.h>
#include "DX10Util.h"

#include <iostream>
#include <string.h>
#include <vector>


#ifndef CDXMANAGER_H
#define CDXMANAGER_H

class CDXManager
{
public:
	static CDXManager* instance();

	//destructor
	~CDXManager(void);

	//Name: Init()
	//Purpose: creates device, initializes settings, determines capabilities,
	//sets up swap chain
	//Return: bool, true on succes, false on failure
	bool Init(HWND, unsigned int*, unsigned int*, bool);

	//Name: RenderScene()
	//Purpose: renders scene of objects
	//Return: none
	void BeginScene();
	void EndScene();

	//Name: Shutdown()
	//Purpose: cleans up memory and releases devices
	//Return: none
	void Shutdown();

	unsigned int getWidth();
	unsigned int getHeight();

	const ID3D10Device* getDevice() const;
	ID3D10Device* getDevice();
	const HWND* getHWND() const;

private:
	//constructor
	CDXManager(void);
	static CDXManager* _instance;
	static bool instantiated;

	//Name: OnError()
	//Purpose: handles errors involving DX10
	//Return: none
	void OnError(LPWSTR);
	
	//device setup interfaces and rendering for back buffer
	IDXGISwapChain*				mSwapChain;
	ID3D10RenderTargetView*		mRenderTargetView;
	ID3D10Texture2D*			mBackBuffer;

	//stencil members
	ID3D10Texture2D*			mDepthStencil;
	ID3D10DepthStencilView*		mDepthStencilView;
	ID3D10DepthStencilState*	mDepthStencilState;

	//reasterizer
	ID3D10RasterizerState* mRasterizerState;

	//main viewport
	D3D10_VIEWPORT				mViewPort;

	ID3D10Device*	mD3DDevice;
	unsigned int mWindowWidth;
	unsigned int mWindowHeight;
	HWND* mhWnd;
};

#endif

