/**----------------------------------------------------------------------------

 * 
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "inline_hook_dll.h"
#include "dbg_msg.h"

#include "TinyDetour.h"
#include "Trampoline.h"

DWORD_PTR get_function_body_address(void* FuncName);

#define TD_HOOK_LAST_ENTRY	{FALSE, NULL, NULL, NULL, 0}

TD_HOOK_INFO g_hooks_to_load[] = 
{
	{	
		FALSE, 
		(BYTE*)get_function_body_address(MessageBoxA), 
		(BYTE*)get_function_body_address(MessageBoxA_Detour), 
		(BYTE*)get_function_body_address(MessageBoxA_Trampoline), 
		0
	},	
	TD_HOOK_LAST_ENTRY
};

/*

typedef 
int 
(__stdcall *fnMessageBoxA)(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	);

int 
__stdcall 
hooked_message_boxa(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	);

FARPROC g_original = (FARPROC) NULL;
FARPROC g_hooked     = (FARPROC) hooked_message_boxa;
*/

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
INLINE_HOOK_API bool hook_messagebox_caption()
{
	for(int i = 0;;i++)
	{
		// is last one?
		// 
		if ((NULL == g_hooks_to_load[i].Target) &&
			(NULL == g_hooks_to_load[i].Detour) &&
			(NULL == g_hooks_to_load[i].Trampoline))
		{
			break;
		}

		if (0 != InjectDetour(&g_hooks_to_load[i]))
		{
			_ASSERTE(!"InjectDetour");
			return false;
		}
	}
	return true;
}

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
INLINE_HOOK_API void unhook_messagebox_caption()
{
	for(int i = 0;;i++)
	{
		// is last one?
		// 
		if ((NULL == g_hooks_to_load[i].Target) &&
			(NULL == g_hooks_to_load[i].Detour) &&
			(NULL == g_hooks_to_load[i].Trampoline))
		{
			break;
		}

		if (0 != EjectDetour(&g_hooks_to_load[i]))
		{
			_ASSERTE(!"EnjectDetour");
			return;
		}
	}

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
DWORD_PTR get_function_body_address(void* FuncName)
{
	_ASSERTE(NULL != FuncName);
	if (NULL == FuncName){return 0;}

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
	{
		dwRVA = (DWORD_PTR)FuncName;
	}
	return (dwRVA);
}