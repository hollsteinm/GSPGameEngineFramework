//(c) copyright Martin Hollstein

#include "DXManager.h"
#include <assert.h>

CDXManager* CDXManager::_instance = 0;
bool CDXManager::instantiated = false;

CDXManager::CDXManager(void)
{
}

CDXManager::~CDXManager(void)
{
}

CDXManager* CDXManager::instance(){
	if(_instance == 0)
		_instance = new CDXManager();

	return _instance;
}

unsigned int CDXManager::getWidth(){
	assert(instantiated);
	return mWindowWidth;
}

unsigned int CDXManager::getHeight(){
	assert(instantiated);
	return mWindowHeight;
}

const ID3D10Device* CDXManager::getDevice() const{
	assert(instantiated);
	return mD3DDevice;
}

ID3D10Device* CDXManager::getDevice(){
	assert(instantiated);
	return mD3DDevice;
}

const HWND* CDXManager::getHWND() const{
	assert(instantiated);
	return mhWnd;
}


bool CDXManager::Init(HWND hWnd, unsigned int* width, unsigned int* height, bool windowed)
{
	mhWnd = &hWnd;

	//Set up swap chain and initialize device
	DXGI_SWAP_CHAIN_DESC sc;
	ZeroMemory(&sc, sizeof(sc));

	mWindowWidth = *width;
	mWindowHeight = *height;

	//set up the swap chain
	sc.BufferCount = 1;
	sc.BufferDesc.Width = mWindowWidth;
	sc.BufferDesc.Height = mWindowHeight;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc.BufferDesc.RefreshRate.Numerator = 60;
	sc.BufferDesc.RefreshRate.Denominator = 1;
	sc.SampleDesc.Quality = 0;
	sc.SampleDesc.Count = 1;
	sc.OutputWindow = hWnd;
	sc.Windowed = windowed;
	sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sc.Flags = 0;

	if(FAILED(D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		D3D10_SDK_VERSION,
		&sc,
		&mSwapChain,
		&mD3DDevice)))
	{
		OnError(L"Failed to Initialize");
		return false;
	}

	//Set up Render target.
	ID3D10Texture2D* mBackBuffer;

	if(FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&mBackBuffer)))
	{
		OnError(L"Could not get buffer");
		return false;
	}

	if(FAILED(mD3DDevice->CreateRenderTargetView(mBackBuffer, NULL, &mRenderTargetView)))
	{
		OnError(L"Could not create render target view");
		return false;
	}

	mBackBuffer->Release();

	//depth buffer texture description
	D3D10_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));

	td.Width = mWindowWidth;
	td.Height = mWindowHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D10_USAGE_DEFAULT;
	td.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	if(FAILED ( mD3DDevice->CreateTexture2D( &td, NULL, &mDepthStencil ) ) )
	{
		OnError(L"Could not create depth buffer texture");
		return false;
	}

	//create the depth stencil view description
	D3D10_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));

	// Set up the description of the stencil state.
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D10_COMPARISON_LESS;

	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	dsd.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	dsd.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	dsd.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	dsd.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	if(FAILED ( mD3DDevice->CreateDepthStencilState( &dsd, &mDepthStencilState) ) )
	{
		OnError(L"Could not create depth stencil state");
		return false;
	}

	mD3DDevice->OMSetDepthStencilState(mDepthStencilState, 1);

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	if(FAILED(mD3DDevice->CreateDepthStencilView(mDepthStencil, &dsvd, &mDepthStencilView)))
	{
		OnError(L"Could not create depth stencil view");
		return false;
	}

	mD3DDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	D3D10_RASTERIZER_DESC rd;

	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D10_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D10_FILL_SOLID;
	rd.FrontCounterClockwise = false;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;

	if(FAILED(mD3DDevice->CreateRasterizerState(&rd, &mRasterizerState)))
	{
		OnError(L"Could not create rasterizer state");
		return false;
	}

	mD3DDevice->RSSetState(mRasterizerState);

	//set the main viewport dimensions
	mViewPort.Width = mWindowWidth;
	mViewPort.Height = mWindowHeight;
	mViewPort.MinDepth = 0.1f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;

	mD3DDevice->RSSetViewports(1, &mViewPort);
	
	instantiated = true;
	return true;
}

void CDXManager::OnError(LPWSTR error)
{
	MessageBox(*mhWnd, error, L"CDXManager", MB_ICONERROR | MB_OK);
}

void CDXManager::BeginScene()
{
	assert(instantiated);
	static const float backBufferColor[4] = {0.5f, 0.5f, 0.5f, 0.5f};

	mD3DDevice->ClearRenderTargetView(mRenderTargetView, backBufferColor);
	mD3DDevice->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

void CDXManager::EndScene()
{
	assert(instantiated);
	mSwapChain->Present(1, 0);
}

void CDXManager::Shutdown()
{
	assert(instantiated);
	if(mRenderTargetView)
		mRenderTargetView->Release();

	if(mSwapChain)
		mSwapChain->Release();

	if(mD3DDevice)
		mD3DDevice->Release();

	if(mDepthStencil)
		mDepthStencil->Release();

	if(mDepthStencilView)
		mDepthStencilView->Release();

	if(_instance != 0)
		delete _instance;
}