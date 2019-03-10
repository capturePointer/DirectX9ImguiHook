#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "detours.lib")
#include "stdafx.h"
#include "detours.h"
#include <iostream>
#include <Windows.h>
#include <Unknwnbase.h>
#include <dinput.h>
#include "d3d9.h"
#include <string>

IDirect3DDevice9* d3d9_device;

typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT SDKVersion);
typedef HRESULT(WINAPI *D3D9_CreateDevice_t)(IDirect3D9 * pD3D9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
typedef HRESULT(WINAPI *EndScene_t)(IDirect3DDevice9* pDevice);

Direct3DCreate9_t Direct3DCreate9_Original;
D3D9_CreateDevice_t D3D9_CreateDevice_Original;
EndScene_t	EndScene_Original;

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers);
HRESULT WINAPI D3D9_CreateDevice_Hook(IDirect3D9* pD3D9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
HRESULT WINAPI EndScene_Hook(IDirect3DDevice9* pDevice);

