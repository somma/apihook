/**----------------------------------------------------------------------------
 * injector.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 26:8:2011   15:31 created
**---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "injector.h"


bool set_privilege(_In_z_ const wchar_t* privilege, _In_ bool enable);
HANDLE advanced_open_process(_In_ DWORD pid);


/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
bool inject_thread(_In_ DWORD pid, _In_z_ const char* dll_path)
{
	// 타겟 프로세스 오픈
	HANDLE process_handle = advanced_open_process(pid);
	if (NULL==process_handle)
	{
		printf("[ERR ] advanced_open_process(pid=%u) failed\n", pid);
		return false;
	}

	// 대상 프로세스 메모리에 dll 경로명 만큼의 버퍼 할당 (파라미터 영역)
	unsigned int buffer_size = strlen(dll_path) + sizeof(char);
	char* buffer = (char*) VirtualAllocEx(process_handle, NULL, buffer_size, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == buffer)
	{
		printf("[ERR ] can not allocate buffer... \n");

		CloseHandle(process_handle);
		return false;
	}

	// dll 경로명 (문자열) 복사
	DWORD cbWritten=0;
	if (TRUE != WriteProcessMemory(process_handle, buffer, dll_path, strlen(dll_path), &cbWritten))
	{
		printf("[ERR ] WriteProcessMemory() failed. gle=0x%08x \n", GetLastError());

		VirtualFreeEx(process_handle, buffer, buffer_size, MEM_DECOMMIT);
		CloseHandle(process_handle);
		return false;
	}

	// LoadLibraryA 주소 구하기
	HMODULE kernel32_handle = GetModuleHandleW(L"kernel32.dll");
	LPTHREAD_START_ROUTINE load_library_ptr = (LPTHREAD_START_ROUTINE) GetProcAddress(kernel32_handle, "LoadLibraryA");

	// create remote-thread 
	HANDLE remote_thread = CreateRemoteThread(process_handle, NULL, 0, load_library_ptr, buffer, 0, NULL);
	WaitForSingleObject(remote_thread, INFINITE);


	VirtualFreeEx(process_handle, buffer, buffer_size, MEM_DECOMMIT);
	CloseHandle(process_handle);
	return true;
}


//
// internal functions
//

/**
* @brief	adjust privilege
* @param	
* @see		
* @remarks	http://support.microsoft.com/kb/131065/EN-US/
* @code		
* @endcode	
* @return	
*/
bool set_privilege(_In_z_ const wchar_t* privilege, _In_ bool enable)
{
	HANDLE token = INVALID_HANDLE_VALUE;
	if (TRUE != OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &token) )
	{
		if (ERROR_NO_TOKEN == GetLastError() )
		{
			if ( ImpersonateSelf(SecurityImpersonation)	!= TRUE ) { return FALSE; }

			if (TRUE != OpenThreadToken(GetCurrentThread(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,FALSE,&token) )
			{
				return FALSE;
			}
		}
		else
		{			
			return FALSE;
		}
	}

	TOKEN_PRIVILEGES tp = { 0 };		
	LUID luid = {0};
	DWORD cb = sizeof(TOKEN_PRIVILEGES);
	
	bool ret = false;
	do 
	{
		if(!LookupPrivilegeValueW( NULL, privilege, &luid )) { break; }

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		if(enable) 
		{ 
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		} 
		else 
		{
			tp.Privileges[0].Attributes = 0;
		}
	
		AdjustTokenPrivileges( token, FALSE, &tp, cb, NULL, NULL );
		if (GetLastError() != ERROR_SUCCESS) { break; }

		ret = true;
	} while (false);
	
	CloseHandle(token);
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
HANDLE advanced_open_process(_In_ DWORD pid)
{
	HANDLE ret = NULL;
	if (true != set_privilege(L"SeDebugPrivilege", true)) return ret;

	do 
	{
		ret = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (NULL == ret) break;

		if (true != set_privilege(L"SeDebugPrivilege", false))
		{
			CloseHandle(ret); ret = NULL;
			break;
		}
	} while (false);
	
	return ret;
}