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
#include "iat_hook_dll.h"
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
IAT_HOOK_API bool hook_messagebox_caption()
{
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
    g_original = (FARPROC) GetProcAddress(u, "MessageBoxA");
    
    
    // find IAT and replace it
    // 
    char* base = (char*) GetModuleHandle(NULL);
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

    PIMAGE_DATA_DIRECTORY idd = &inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    PIMAGE_IMPORT_DESCRIPTOR iid = (PIMAGE_IMPORT_DESCRIPTOR)(base + idd->VirtualAddress);
    for(; iid->OriginalFirstThunk; ++iid)
    {
        char* module_name = (LPSTR)(base + iid->Name);
        if(0 == _strcmpi(module_name, "user32.dll")) break;
    }

    if (NULL == iid->Name) 
	{
		log_err "no user32.dll from IAT -_-" log_end
		return false;
	}

    PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)(base + iid->FirstThunk);
    for(; thunk->u1.Function; thunk++) 
    {
        FARPROC* ppfn = (FARPROC*)&thunk->u1.Function;
        if(*ppfn == g_original)
        {
            // save & replace 
            // 
			g_original = **ppfn;
			*ppfn = g_hooked;

			log_info 
				"iat hooked. from 0x%08x --> to 0x%08x ", 
				g_original, g_hooked
			log_end
            
            return true;
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
IAT_HOOK_API void unhook_messagebox_caption()
{
	if (NULL == g_original) return;

    // find iat and replace it
    // 
    char* base = (char*) GetModuleHandle(NULL);
    if (NULL == base) return;

    PIMAGE_DOS_HEADER  idh = (PIMAGE_DOS_HEADER)base;
    PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS) (base + idh->e_lfanew);
    PIMAGE_DATA_DIRECTORY idd = &inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    PIMAGE_IMPORT_DESCRIPTOR iid = (PIMAGE_IMPORT_DESCRIPTOR)(base + idd->VirtualAddress);
    for(; iid->OriginalFirstThunk; ++iid)
    {
        char* module_name = (LPSTR)(base + iid->Name);
        if(0 == _strcmpi(module_name, "user32.dll")) break;
    }
    if (NULL == iid->Name) return;

    PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)(base + iid->FirstThunk);
    for(; thunk->u1.Function; thunk++) 
    {
        FARPROC* ppfn = (FARPROC*)&thunk->u1.Function;        
        if(*ppfn == g_hooked)
        {
            // replace
            // 
            *ppfn = g_original;
            return;
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