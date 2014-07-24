/**----------------------------------------------------------------------------
 * main.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * 
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdlib.h>
#include "injector.h"

/**
* @brief	
* @param	
* @see		
* @remarks	
* @code		
* @endcode	
* @return	
*/
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		printf("usage : %s  pid  dll_path_to_inject \n", argv[0]);
		return 0;
	}

	DWORD pid = atoi(argv[1]);
	char* path = argv[2];

	if (true != inject_thread(pid, path))
	{
		printf("[ERR ] dll injection failed. \n");
	}
	else
	{
		printf("[INFO] dll injected successfully. \n");
	}	
	return 0;
}

