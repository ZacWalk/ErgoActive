// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HOOKS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HOOKS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HOOKS_EXPORTS
#define HOOKS_API __declspec(dllexport)
#else
#define HOOKS_API __declspec(dllimport)
#endif

#pragma once

// This class is exported from the Hooks.dll
class HOOKS_API CHooks 
{
private:

	
public:
	CHooks();
	~CHooks();

	void Create(HWND hWnd, UINT uMessage);

};

class HOOKS_API CUse
{
public:
	CUse() : m_nMouse(0), m_nKeyBoard(0)
	{
	}

	CUse(int nMouse, int nKeys) : m_nMouse(nMouse), m_nKeyBoard(nKeys)
	{
	}	

	void Reset()
	{
		m_nMouse = 0;
		m_nKeyBoard = 0;
	}

	void operator=(const CUse& rhs)
	{
		m_nMouse = rhs.m_nMouse;
		m_nKeyBoard = rhs.m_nKeyBoard;
	}

	int GetUsage() const
	{
		return m_nMouse + m_nKeyBoard;
	}

	long m_nMouse;
	long m_nKeyBoard;
};

extern HOOKS_API CUse g_use;