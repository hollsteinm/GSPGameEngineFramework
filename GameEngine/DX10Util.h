//(c) copyright Martin Hollstein

#include <Windows.h>
#include <xnamath.h>
#include <D3DX10.h>
#include <D3D10.h>
#include <vector>
#include "Log.h"

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib, "d3dx10.lib")

#ifndef DX10UTIL_H
#define DX10UTIL_H

//define common colors
#define WHITE	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
#define RED		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)

#endif
