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
#include "dx9hook.h"
#include "Imgui/imgui.h"
#include "Imgui/imconfig.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imstb_rectpack.h"
#include "Imgui/imstb_textedit.h"
#include "Imgui/imstb_truetype.h"

using namespace std;
bool DX9HookSuccess = false;

void Hook_D3D9_Direct3DCreate9();
void Hook_D3D9_CreateDevice();
void Hook_D3D9_EndScene();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Hook_D3D9_Direct3DCreate9();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

bool DetourHookFunction(PVOID *ppPointer, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach(ppPointer, pDetour))
		return false;
	if (DetourTransactionCommit() != NO_ERROR)
		return false;
	return true;
}

// Hook D3D9 Direct3DCreate9
void Hook_D3D9_Direct3DCreate9()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
	if (hD3D9)
	{
		PBYTE pDirect3DCreate9 = (PBYTE)GetProcAddress(hD3D9, "Direct3DCreate9");
		if (pDirect3DCreate9)
		{
			Direct3DCreate9_Original = (Direct3DCreate9_t)(pDirect3DCreate9);
			if (DetourHookFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
			{
				return;
			}
		}
	}
}
IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	IDirect3D9 *legit = Direct3DCreate9_Original(sdkVers);
	int* pVTable = (int*)(*(int*)legit);
	D3D9_CreateDevice_Original = (D3D9_CreateDevice_t)pVTable[16];
	Hook_D3D9_CreateDevice();
	return legit;
}

// Hook D3D9 CreateDevice
void Hook_D3D9_CreateDevice()
{
	DetourHookFunction(&(PVOID&)D3D9_CreateDevice_Original, D3D9_CreateDevice_Hook);
}
HRESULT WINAPI D3D9_CreateDevice_Hook(IDirect3D9* pD3D9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface)
{
	ImGui_ImplWin32_Init(pPresentationParameters->hDeviceWindow);
	HRESULT hres = D3D9_CreateDevice_Original(pD3D9, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	d3d9_device = *ppReturnedDeviceInterface;
	int* pVTable = (int*)(*(int*)d3d9_device);
	EndScene_Original = (EndScene_t)pVTable[42];
	Hook_D3D9_EndScene();
	return hres;
}

// Hook D3D9 EndScene
void Hook_D3D9_EndScene()
{
	DetourHookFunction(&(PVOID&)EndScene_Original, EndScene_Hook);
}

HRESULT WINAPI EndScene_Hook(IDirect3DDevice9* pDevice)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 1 << 15) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.MouseDown[0] = true;
	}
	else {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.MouseDown[0] = false;
	}

	if (DX9HookSuccess == false) {
		DX9HookSuccess = true;
		ImGui_ImplDX9_Init(pDevice);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Text("Demo Text");

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	HRESULT hres = EndScene_Original(pDevice);
	return hres;
}

void GetTargetedEnemy() {

}
