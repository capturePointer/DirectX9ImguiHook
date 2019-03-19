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

typedef int(*thook)();
thook oHook;

using namespace std;
bool DX9HookSuccess = false;

void Hook_D3D9_Direct3DCreate9();
void Hook_D3D9_CreateDevice();
void Hook_D3D9_EndScene();

void GetTargetedEnemy();
bool doesContain(DWORD);
void initEntitylist();

int ix;
int counter = 0;
DWORD Addy;
DWORD addyarray[256] = { };
bool duplicate = false;

int hHook() {
	_asm {
		fsub DWORD ptr [eax+0x60]
		fstp DWORD ptr [ebp-0x08]
		mov Addy, eax
	}	

	if (ix < 256) {
			addyarray[ix] = Addy;
			ix++;
	}

	return oHook();
}

//bool doesContain(DWORD checkaddy) {
//	//bool contains = false;
//	//Foo array[10];
//	//	Foo *foo = std::find(std::begin(array), std::end(array), someObject);
//	//// When the element is not found, std::find returns the end of the range
//	//if (foo != std::end(array)) {
//	//	cerr << "Found at position " << std::distance(array, foo) << endl;
//	//}
//	//else {
//	//	cerr << "Not found" << endl;
//	//}
//	//for (int x = 0; x <= ix; x++) {
//	//	if (addyarray[x] == checkaddy) {
//	//		contains = true;
//	//	}
//	//}
//	return contains;
//}

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


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole(); // enables the console
		freopen("CONOUT$", "w", stdout);
		initEntitylist();
		oHook = (thook)(0x00F51555);
		DetourHookFunction(&(PVOID&)oHook, hHook);
		Hook_D3D9_Direct3DCreate9();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void initEntitylist() {
	for (int x = 0; x < 256; x++) {
		addyarray[x] = 0x0;
	}
}


		//DWORD fbase = (DWORD)GetModuleHandleA("Fiesta.exe");
		//DWORD AddyToHook = fbase + 0x251BB9;

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


DWORD_PTR Base;
DWORD_PTR Offset2;
DWORD_PTR Offset3;
DWORD_PTR Offset4;
DWORD_PTR Offset5;
DWORD_PTR FinalAddress;

bool checkbox;
const char* text;
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

	if (ImGui::Checkbox("GetTargetedEnemy:", &checkbox)) {
		system("CLS");
		for (int x = 0; x < 255; x++)
		{
			float xpos = *(float*)((DWORD)addyarray[x] +0x58);
	/*		printf("eax[%d] = %X \n", x, addyarray[x]);*/
			printf("xaddy[%d] = %f \n", x, xpos);
		}
	}

	if (checkbox) {
		float x = *(float*)((DWORD)Addy + 0x58);
		float y = *(float*)((DWORD)Addy + 0x5C);
		float z = *(float*)((DWORD)Addy + 0x60);
		//
		ImGui::Text("Position: (%f,%f,%f) : Count: %d : eax: %X", x,y,z,ix,Addy);
	}
	else {

		ImGui::Text("Heya");
	}



	ImGui::EndFrame();
	ImGui::Render();


	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	HRESULT hres = EndScene_Original(pDevice);
	return hres;
}



void GetTargetedEnemy() {
	Base = (DWORD)GetModuleHandleA("Fiesta.exe");

	Base = *(DWORD_PTR*)(Base + 0x006AB4A8);
	Offset2 = *(DWORD_PTR*)(Base + 0x224);
	Offset3 = *(DWORD_PTR*)(Offset2 + 0x138);
	Offset4 = *(DWORD_PTR*)(Offset3 + 0x20);
	Offset5 = *(DWORD_PTR*)(Offset4 + 0x12C);
	FinalAddress = *(DWORD_PTR*)(Offset5 + 0x12C);
	float HP = *(float*)((DWORD)FinalAddress);
	text = ("HP: ");
}
