/**----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 
**---------------------------------------------------------------------------*/
#ifndef _inline_hook_dll_
#define _inline_hook_dll_

#ifdef INLINE_HOOK_DLL_EXPORTS
	#ifdef __cplusplus
		#define INLINE_HOOK_API			extern "C" __declspec(dllexport) 		
	#else
		#define INLINE_HOOK_API			__declspec(dllexport)	
	#endif//__cplusplus
#else
	#ifdef __cplusplus
		#define INLINE_HOOK_API			extern "C" __declspec(dllimport)		
	#else
		#define INLINE_HOOK_API			__declspec(dllimport)
	#endif//__cplusplus		
#endif//INLINE_HOOK_DLL_EXPORTS

INLINE_HOOK_API bool hook_messagebox_caption();
INLINE_HOOK_API void unhook_messagebox_caption();

#endif//_inline_hook_dll_