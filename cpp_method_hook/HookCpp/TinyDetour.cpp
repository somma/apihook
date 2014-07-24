/*-----------------------------------------------------------------------------
 * TinyDetour.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 2007.01.22 created
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TinyDetour.h"
#include "libdasm.h"
#include <malloc.h>
#include <crtdbg.h>

// internal functions
//
int CopyCodeBytes(BYTE* Src, BYTE* Dest, DWORD Size);
int GetInstructionLength(BYTE* Addr);



/**	---------------------------------------------------------------------------
	\brief	

		[target function]
			A1                       A2 (A1 + RealPatchSize)
			|-----jmp A3 -- x90x90 --|^-------------
			     (   RealPatchSize   )
			jmp A3
			===> 
			A4 (  OpenProcess_Detour() 함수 주소  )

			A4 = A2 + A3 이므로 (jmp 명령이니까)
			A3 = A4 - A2 

		[trampoline]
			T1                       T2      T3     T4
			|-----MAX_PATCH_SIZE-----|jmp|-----4----|			 
			jmp T3
			===> A2

			A2 = T4 + T3 이므로
			T3 = A2 - T4

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int InjectDetour(TD_HOOK_INFO*	pHookInfo)
{
	_ASSERTE(NULL != pHookInfo);
	_ASSERTE(NULL != pHookInfo->Target);
	_ASSERTE(NULL != pHookInfo->Detour);
	_ASSERTE(NULL != pHookInfo->Trampoline);

	if ((NULL == pHookInfo) || 
		(NULL == pHookInfo->Target) ||
		(NULL == pHookInfo->Detour) ||
		(NULL == pHookInfo->Trampoline) )
	{
		return -1;
	}


	//-------------------------------------------------------------------------
	// # STEP 1
	//		build trampoline
	//-------------------------------------------------------------------------
	// [TargetFunc's original opcode] [jmp] [TagetFunc + 5]
	//
	// trampoline 에 TargetFunc 의 코드를 복사해 둔다.
	//		- TargetFunc 에 JMP_REL_32_SIZE 만큼의 명령어 공간을 계산
	// 
	int Len = 0;
	DWORD	dwOpcodeLen = 0;
	do 
	{
		Len = GetInstructionLength(pHookInfo->Target + dwOpcodeLen);
		if (-1 == Len)
		{
			_ASSERTE(!"GetInstructionLength");
			return -1;
		}
		dwOpcodeLen += Len;
	} while (JMP_REL_32_SIZE > dwOpcodeLen);

	pHookInfo->OriginalOpcodeLen = dwOpcodeLen;

	#ifdef _DEBUG
	_tprintf(TEXT("target instruction length: %ld\n"), pHookInfo->OriginalOpcodeLen);
	#endif	
	
	if (MAX_PATCH_SIZE < pHookInfo->OriginalOpcodeLen)
	{
		_ASSERTE(!"too big patch size");
		return -1;
	}


	if (0 != CopyCodeBytes(pHookInfo->Target, 
							pHookInfo->Trampoline, 
							pHookInfo->OriginalOpcodeLen))
	{
		_ASSERTE(!"CopycodeBytes");
		return -1;
	}

	pHookInfo->Trampoline[MAX_PATCH_SIZE] = INST_JMP;
	DWORD_PTR tp_jmp = (pHookInfo->Target + pHookInfo->OriginalOpcodeLen) - 
						(pHookInfo->Trampoline + MAX_PATCH_SIZE + JMP_REL_32_SIZE);
	*(DWORD_PTR*)(&pHookInfo->Trampoline[MAX_PATCH_SIZE + INST_JMP_SIZE]) = tp_jmp;


	//-------------------------------------------------------------------------
	// # STEP 2
	//		build jmp [detour] stub
	//		reroute TargetFunc to DetourFunc
	//-------------------------------------------------------------------------
	// bto (buffer to overwite) = jmp [DetourFunc]
	//
	BYTE* bto = (BYTE*) malloc(pHookInfo->OriginalOpcodeLen);

	// RealPatchSize 에서 jmp rel_32 명령을 제외한 나머지 부분은 nop 처리
	//
	memset(bto, 0x90, pHookInfo->OriginalOpcodeLen);

	bto[0] = INST_JMP;
	*(reinterpret_cast<DWORD_PTR*>(bto + 1)) = 
		pHookInfo->Detour - (pHookInfo->Target + JMP_REL_32_SIZE);
	
	if (0 != CopyCodeBytes(bto, 
						pHookInfo->Target, 
						pHookInfo->OriginalOpcodeLen))
	{
		_ASSERTE(!"CopycodeBytes");
		if (NULL != bto){ free(bto); bto = NULL; }
		return -1;
	}	

	if (NULL != bto){ free(bto); bto = NULL; }
	pHookInfo->Enabled = TRUE;

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int EjectDetour(TD_HOOK_INFO*	pHookInfo)
{
	_ASSERTE(NULL != pHookInfo);
	_ASSERTE(NULL != pHookInfo->Target);
	_ASSERTE(NULL != pHookInfo->Detour);
	_ASSERTE(NULL != pHookInfo->Trampoline);

	if ((NULL == pHookInfo) || 
		(NULL == pHookInfo->Target) ||
		(NULL == pHookInfo->Detour) ||
		(NULL == pHookInfo->Trampoline) )
	{
		return -1;
	}



	if (0 != CopyCodeBytes(pHookInfo->Trampoline, 
						pHookInfo->Target, 
						pHookInfo->OriginalOpcodeLen))
	{
		_ASSERTE(!"CopyCodeBytes");
		return -1;
	}
	pHookInfo->Enabled = FALSE;
	
	
	return 0;
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CopyCodeBytes(BYTE* Src, BYTE* Dest, DWORD Size)
{
	_ASSERTE(NULL != Src);
	_ASSERTE(NULL != Dest);
	if ( (NULL == Src) || (NULL == Dest) ) return -1;

	EnableWriteOnCodePage src_ew(Src, Size);
	EnableWriteOnCodePage dst_ew(Dest, Size);
	if (FALSE == dst_ew.SetPermission(PAGE_EXECUTE_READWRITE))
	{
		_ASSERTE(!"permission denied");
		return -1;
	}
	if (FALSE == src_ew.IsValid())
	{
		_ASSERTE(!"permission denied");
		return -1;
	}

	// copy memory
	//
	memmove(reinterpret_cast<void*>(Dest), 
			reinterpret_cast<void*>(Src), 
			Size);

	return 0;
	// destructor restore original permision on codepage
}

/**	---------------------------------------------------------------------------
	\brief	calculate instruction length of Addr 

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int GetInstructionLength(BYTE* Addr)
{

#ifdef _USE_LIBDASM_LIB		
	
	INSTRUCTION instr = {0};	
	int Len = get_instruction(&instr, 
						Addr,
						MODE_32);
	// check illegal opcode
	if (0 == Len)
	{
		_ASSERTE(!"get_instruction");
		return -1;
	}

	#ifdef _DEBUG
	char string[256] = {0};
	get_instruction_string(&instr, FORMAT_INTEL, 0, string, sizeof(string));
	_tprintf(TEXT("%s\n"), string);
	#endif
	
	return Len;		

#else

	DISASSEMBLER   Disassembler;
	INSTRUCTION *  Instruction = NULL;	
	if (TRUE != InitDisassembler(&Disassembler, ARCH_X86))
	{
		_ASSERTE(!"InitDisassembler");
		return -1;
	}

	ULONG Flags = DISASM_DISASSEMBLE | DISASM_DECODE | DISASM_STOPONERROR | 
					DISASM_STOPONANOMALY | DISASM_STOPONRETURN;
	Instruction = GetInstruction(&Disassembler, 
							(ULONG)Addr, 
							(PBYTE)Addr,
							Flags);
	if (!Instruction) 
	{
		_ASSERTE(!"GetInstruction");
		CloseDisassembler(&Disassembler);

		return -1;
	}

	#ifdef _DEBUG 
	DumpInstruction(Instruction, TRUE, TRUE);
	#endif
	

	int Len = Instruction->Length;
	CloseDisassembler(&Disassembler);
	return Len;

#endif//_USE_LIBDASM_LIB
}