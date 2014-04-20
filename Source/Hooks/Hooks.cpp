// Hooks.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Hooks.h"

HINSTANCE g_hInstance = NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule, 
					  DWORD  ul_reason_for_call,  LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:		
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	g_hInstance = hModule;
	return TRUE;
}

#pragma data_seg (".SHARED")

HWND g_hWnd = 0;
UINT g_uMessage = 0;
HOOKS_API CUse g_use;

#pragma data_seg() 
#pragma comment(linker, "/SECTION:.SHARED,RWS")


HHOOK g_hookMouse = 0; 
HHOOK g_hookKeyboard = 0; 

HOOKS_API LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
	if (HC_ACTION == nCode && g_hWnd != 0)
	{	
		if (wParam == WM_LBUTTONUP || 
			wParam == WM_RBUTTONUP || 
			wParam == WM_LBUTTONDBLCLK ||
			wParam == WM_RBUTTONDBLCLK)
		{
			::SendMessage(g_hWnd, g_uMessage, 0, 0);
			//g_use.m_nMouse += 1;
		}
	}

	return CallNextHookEx(g_hookMouse, nCode, wParam,  lParam); 
} 



HOOKS_API LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
	if (HC_ACTION == nCode && g_hWnd != 0)
	{
		if (wParam == WM_KEYDOWN ||
			wParam == WM_SYSKEYDOWN)
		{
			::SendMessage(g_hWnd, g_uMessage, 0, 0);
			//g_use.m_nKeyBoard += 1;
		}
	}

	return CallNextHookEx(g_hookKeyboard, nCode, wParam,  lParam); 
} 

CHooks::CHooks()
{
}

CHooks::~CHooks()
{ 
	UnhookWindowsHookEx(g_hookMouse);
	UnhookWindowsHookEx(g_hookKeyboard);
}

void CHooks::Create(HWND hWnd, UINT uMessage)
{ 
	g_hWnd = hWnd;
	g_uMessage = uMessage;
	g_hookKeyboard = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, NULL);
	g_hookMouse = SetWindowsHookEx(WH_MOUSE, MouseProc, g_hInstance, NULL);
} 

