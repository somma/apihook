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
#include "eat_hook_dll.h"
#include "dbg_msg.h"

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

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
EAT_HOOK_API bool hook_messagebox_caption()
{
	#define function_name_to_hook		"MessageBoxA"
	// get original messageboxa function address
    //
    HMODULE u = GetModuleHandleA("user32.dll");
    if (NULL == u) 
	{
		u = LoadLibraryA("user32.dll");
		if (NULL == u) 
		{
			log_err "can not load \"user32.dll\". gle=0x%08x", GetLastError() log_end
			return false;
		}
	}
    g_original = (FARPROC) GetProcAddress(u, function_name_to_hook);
    log_info "%s = 0x%08x", function_name_to_hook, g_original log_end

	// find EAT and replace it
    // 
    char* base = (char*)u;
    if (NULL == base) return FALSE;

    PIMAGE_DOS_HEADER idh = (PIMAGE_DOS_HEADER)base;
    if (IMAGE_DOS_SIGNATURE != idh->e_magic)
    {
		log_err "invalid idh->e_magic" log_end
        return false;
    }

    PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS) (base + idh->e_lfanew);
    if (IMAGE_NT_SIGNATURE != inh->Signature)
    {
		log_err "invalid inh->Signature" log_end
        return false;
    }
	
    PIMAGE_DATA_DIRECTORY idd = &inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    PIMAGE_EXPORT_DIRECTORY ied = (PIMAGE_EXPORT_DIRECTORY)(base + idd->VirtualAddress);

	DWORD function_index = 0;
	LPDWORD function_address_array = (LPDWORD)(base + ied->AddressOfFunctions);
	for(; function_index < ied->NumberOfFunctions; ++function_index)
	{
		if ((DWORD)g_original == (DWORD)base + (DWORD)function_address_array[function_index])
		{
			break;
		}
	}

	bool ret = false;
    if (function_index < ied->NumberOfFunctions)
	{
		DWORD old_protect;
		PDWORD old = (PDWORD)&function_address_array[function_index];
		VirtualProtect((LPVOID)old, sizeof(DWORD), PAGE_READWRITE, &old_protect);
		log_info "before = 0x%08x", *old + base log_end
		*old = (DWORD)((DWORD)g_hooked - (DWORD)base);
		log_info "after = 0x%08x", *old + base  log_end
		VirtualProtect((LPVOID)old, sizeof(DWORD), old_protect, &old_protect);

		log_info 
			"eat hooked. from 0x%08x --> to 0x%08x ", 
			g_original, g_hooked
		log_end
		ret = true;
	}
	else
	{
		log_info
			"no '%s' founded from EAT", function_name_to_hook
		log_end
	}

    return ret;
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
EAT_HOOK_API void unhook_messagebox_caption()
{
	if (NULL == g_original) return;

    // find iat and replace it
    // 
    char* base = (char*) GetModuleHandle(NULL);
    if (NULL == base) return;

    PIMAGE_DOS_HEADER  idh = (PIMAGE_DOS_HEADER)base;
    PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS) (base + idh->e_lfanew);
    PIMAGE_DATA_DIRECTORY idd = &inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	PIMAGE_EXPORT_DIRECTORY ied = (PIMAGE_EXPORT_DIRECTORY)(base + idd->VirtualAddress);

	DWORD function_index = 0;
	LPDWORD function_address_array = (LPDWORD)ied->AddressOfFunctions;
	LPDWORD function_name_array = (LPDWORD)ied->AddressOfNames;
	for(; function_index < ied->NumberOfFunctions; ++function_index)
	{
		if (g_hooked == (FARPROC)function_address_array[function_index])
		{
			function_address_array[function_index] = (DWORD)g_original;
			break;
		}
	}
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
int 
__stdcall 
hooked_message_boxa(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	)
{
	if (NULL != g_original)
	{
		fnMessageBoxA fn = (fnMessageBoxA) g_original;
		return fn( hWnd, lpText, "___this_is_hooked_caption___", uType);
	}

	return false;
}