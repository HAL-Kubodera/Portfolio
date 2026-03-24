/*============================================================
	[main.h]
	Å•ëËñº : ÉÅÉCÉìä÷êî
	Å•DATE : 2026/03/18
	Å•AUTHOR : ãvï€éõ èrâÓ
============================================================*/
#ifndef _MAIN_H_
#define _MAIN_H_

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define NOMINMAX
#include <windows.h>
#include <assert.h>
#include <functional>

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


#include <DirectXMath.h>
using namespace DirectX;

#include "DirectXTex.h"

#if _DEBUG
#pragma comment(lib,"DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex_Release.lib")
#endif

#include "vector3.h"
#include "keyboard.h"


#pragma comment (lib, "winmm.lib")


#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)
#define RADIAN (XM_PI / 180)


HWND GetWindow();

void Invoke(std::function<void()> Function, int Time);

#endif // _MAIN_H_