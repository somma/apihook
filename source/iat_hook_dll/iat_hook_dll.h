/**----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 
**---------------------------------------------------------------------------*/
#ifndef _iat_hook_dll_
#define _iat_hook_dll_

#ifdef IAT_HOOK_DLL_EXPORTS
	#ifdef __cplusplus
		#define IAT_HOOK_API			extern "C" __declspec(dllexport) 		
	#else
		#define IAT_HOOK_API			__declspec(dllexport)	
	#endif//__cplusplus
#else
	#ifdef __cplusplus
		#define IAT_HOOK_API			extern "C" __declspec(dllimport)		
	#else
		#define IAT_HOOK_API			__declspec(dllimport)
	#endif//__cplusplus		
#endif//IAT_HOOK_DLL_EXPORTS

IAT_HOOK_API bool hook_messagebox_caption();
IAT_HOOK_API void unhook_messagebox_caption();

#endif//_iat_hook_dll_