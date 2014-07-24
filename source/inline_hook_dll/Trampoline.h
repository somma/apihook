/*-----------------------------------------------------------------------------
 * Trampoline.h
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#ifndef _TRAMPOLINE_H_
#define _TRAMPOLINE_H_

// trampoline function
//
//__declspec(naked) 
int 
__stdcall 
MessageBoxA_Trampoline(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	);

// Detour fucntion
// 
int 
__stdcall 
MessageBoxA_Detour(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	);


//typedef 
//NTSTATUS 
//(__stdcall *ZWQUERYSYSTEMINFORMATION)(
//	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
//	OUT PVOID SystemInformation,
//	IN ULONG SystemInformationLength,
//	OUT PULONG ReturnLength OPTIONAL
//	);

#endif//_TRAMPOLINE_H_

