/*-----------------------------------------------------------------------------
 * CppDLL
 *-----------------------------------------------------------------------------
 * c++ method 후킹 테스트를 위한 샘플 DLL
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 2007.01.22 created
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "CppDll.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


CCppDll::CCppDll()
{
	return;
}


DWORD CCppDll::CppMethod(DWORD param1,DWORD param2)
{
	m_Value = param1 + param2;
	return m_Value;
}
