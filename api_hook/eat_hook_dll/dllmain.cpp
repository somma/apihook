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

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{		
	case DLL_PROCESS_ATTACH:	if (true != hook_messagebox_caption()){ break;}
	case DLL_THREAD_ATTACH:		break;
	case DLL_THREAD_DETACH:		break;
	case DLL_PROCESS_DETACH:	/*unhook_messagebox_caption();*/ break;
	}
	return TRUE;
}

