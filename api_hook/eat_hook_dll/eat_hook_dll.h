/**----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 
**---------------------------------------------------------------------------*/
#ifndef _eat_hook_dll_
#define _eat_hook_dll_

#ifdef EAT_HOOK_DLL_EXPORTS
	#ifdef __cplusplus
		#define EAT_HOOK_API			extern "C" __declspec(dllexport) 		
	#else
		#define EAT_HOOK_API			__declspec(dllexport)	
	#endif//__cplusplus
#else
	#ifdef __cplusplus
		#define EAT_HOOK_API			extern "C" __declspec(dllimport)		
	#else
		#define EAT_HOOK_API			__declspec(dllimport)
	#endif//__cplusplus		
#endif//EAT_HOOK_DLL_EXPORTS

EAT_HOOK_API bool hook_messagebox_caption();
EAT_HOOK_API void unhook_messagebox_caption();

#endif//_eat_hook_dll_