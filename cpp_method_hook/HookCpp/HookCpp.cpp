/*-----------------------------------------------------------------------------
 * HookCpp.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 2007.01.22 created
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include <Windows.h>
#include "CppDll.h"
#include <crtdbg.h>
#include "TinyDetour.h"

DWORD_PTR GetFunctionRVA(void* FuncName);

/*__declspec(naked)*/ 
DWORD
__stdcall 
CppMethodPtr_Trampoline(
	DWORD param1, 
	DWORD param2
	);

DWORD
__stdcall 
CppMethodPtr_Detour(
	DWORD param1, 
	DWORD param2
	);





/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD Value = 0;

	// hook cpp+ method
	// ecx : this pointer, same as stdcall
	// 
	typedef DWORD (__stdcall *Child_ChildMethod)(DWORD param1, DWORD Param2);
	typedef DWORD (__stdcall *Child_ParentMethod)(DWORD param1, DWORD Param2);
	typedef DWORD (__stdcall *Child_ParentVirtualMethod)(DWORD param1, DWORD Param2);

	// get pointer
	// 
	Child_ChildMethod ChildMethodPtr = NULL;
	Child_ParentMethod ParentMethodPtr = NULL;
	Child_ParentVirtualMethod ParentVirtualMethodPtr = NULL;

	
	// 
	// CPP DLL link
	// 

	CCppDll CppDll;
	Value = CppDll.CppMethod(1, 2);


	// hook cpp function
	// 
	typedef DWORD (__stdcall *CppDLL_CppMethod)(DWORD param1, DWORD param2);
	HMODULE hCppDll = GetModuleHandleW(L"CppDll.dll");
	_ASSERTE(NULL != hCppDll);

	// CCppDll::CCppDll 생성자
	//
	//typedef void (__stdcall *CppDllCreateInstance)();
	//CppDllCreateInstance CppDllCreateInstancePtr = (CppDllCreateInstance)GetProcAddress(hCppDll, "??0CCppDll@@QAE@XZ");
	//_ASSERTE(NULL != CppDllCreateInstancePtr);

	// CCppDll::CppMethod
	//
	CppDLL_CppMethod CppMethodPtr = (CppDLL_CppMethod)GetProcAddress(hCppDll, "?CppMethod@CCppDll@@QAEKKK@Z");
	_ASSERTE(NULL != CppMethodPtr);

	#define TD_HOOK_LAST_ENTRY	{FALSE, NULL, NULL, NULL, 0}
	TD_HOOK_INFO g_hooks_to_load[] = 
	{
		{
			FALSE, 
				(BYTE*)GetFunctionRVA(CppMethodPtr), 
				(BYTE*)GetFunctionRVA(CppMethodPtr_Detour), 
				(BYTE*)GetFunctionRVA(CppMethodPtr_Trampoline), 
				0
		},
		TD_HOOK_LAST_ENTRY
	};	
	
	if (0 != InjectDetour(&g_hooks_to_load[0]))
	{
		_ASSERTE(!"oops");
	}

	Value = CppDll.CppMethod(1, 2);

	return 0;
}



/**	---------------------------------------------------------------------------
	\brief	

		IncrementalLink 옵션을 사용해서 링크한 경우(DEBUG default)

		push argument
		call my_thread_proc
		...
		my_thread_proc :
		jmp RVA
		형태로 코드가 생성됨
		따라서 실제 구현코드는 jmp 인스트럭션 다음 4바이트값이다.
		
		IncrementalLink 옵션을 사용하지 않은 경우(RELEASE default)
		funcName 은 실제 func 의 body 부분임

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
DWORD_PTR GetFunctionRVA(void* FuncName)
{
	_ASSERTE(NULL != FuncName);
	if (NULL == FuncName)
	{
		return 0;
	}

	DWORD_PTR dwRVA = 0;
	
	BYTE* tmp = (BYTE*) FuncName;
	if (0xe9 == tmp[0])
	{
		void *_tempFuncName = FuncName;
		char *ptempFuncName = PCHAR(_tempFuncName);	
		DWORD_PTR _jmpdwRVA = 0;
		CopyMemory(&_jmpdwRVA, ptempFuncName + 1, 4);
		dwRVA = (DWORD_PTR)ptempFuncName + _jmpdwRVA + 5;	
	}
	else
		dwRVA = (DWORD_PTR)FuncName;


	return (dwRVA);
}



/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
#pragma warning(disable: 4100)

__declspec(naked) 
DWORD
__stdcall 
CppMethodPtr_Trampoline(
	DWORD param1, 
	DWORD param2
	)
{
	__asm

	{
		// 원본 opcode 를 보관하기 위한 공간 (12 byte)
		nop
		nop
		nop
		nop		
		nop
		nop
		nop
		nop				
		nop
		nop
		nop
		nop				
		
		
		_emit	0xCC
		_emit	0xAA
		_emit	0xBB
		_emit	0xCC
		_emit	0xDD
		_emit	0xCC
		_emit	0xCC
		_emit	0xCC
		_emit	0xCC
	}
}
#pragma warning(default: 4100)


/**----------------------------------------------------------------------------
    \brief  

    \param  
    \return         
    \code
    \endcode        
-----------------------------------------------------------------------------*/
//DWORD
//__stdcall 
//CppMethodPtr_Detour(
//	DWORD param1, 
//	DWORD param2
//	)
//{
//    DWORD val = CppMethodPtr_Trampoline(param1, param2);
//	return val*2;
//}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
__declspec(naked) 
DWORD
__stdcall 
CppMethodPtr_Detour(
	DWORD param1, 
	DWORD param2
	)
{
	__asm      /* prolog */
	{
		push   ebp
		mov    ebp, esp
		sub    esp, __LOCAL_SIZE
    }

    DWORD val;

    __asm 
    {
        mov eax, dword ptr [param2]
        push eax
        mov eax, dword ptr [param1]
        push eax
        
        call CppMethodPtr_Trampoline
        
        mov val, eax
    }

	
	val = val*2;

	__asm      
	{
        mov eax, val
		mov esp, ebp
		pop	ebp	
		ret	8							// 0xC3
	}
}