/*-----------------------------------------------------------------------------
 * TinyDetour.h
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 2007.01.22 created
**---------------------------------------------------------------------------*/

#ifndef _TINY_DETOUR_H_
#define _TINY_DETOUR_H_

#define INST_NOP		0x90
#define INST_CALL		0xe8
#define INST_JMP		0xe9
#define INST_JMP_SIZE	0x01

#define REL_32_SIZE				sizeof(DWORD_PTR)				// address 의 사이즈 
#define JMP_REL_32_SIZE			(INST_JMP_SIZE + REL_32_SIZE)	// jmp rel32 size
#define MAX_PATCH_SIZE			0x0C		


typedef struct _TD_HOOK_INFO
{
	BOOL	Enabled;

	BYTE*	Target;
	BYTE*	Detour;
	BYTE*	Trampoline;

	DWORD	OriginalOpcodeLen;
} TD_HOOK_INFO, *PTD_HOOK_INFO;


int InjectDetour(TD_HOOK_INFO*	pHookInfo);
int EjectDetour(TD_HOOK_INFO*	pHookInfo);




//-----------------------------------------------------------------------------
// theft from detours library 1.5 :-)
//	CDetourEnableWriteOnCodePage
//-----------------------------------------------------------------------------
class EnableWriteOnCodePage
{
public:
    EnableWriteOnCodePage(PBYTE pbCode, LONG cbCode)
    {
        m_pbCode = pbCode;
        m_cbCode = cbCode;
        m_dwOldPerm = 0;
        m_hProcess = GetCurrentProcess();

        if (m_pbCode && m_cbCode) {
            if (!FlushInstructionCache(m_hProcess, pbCode, cbCode)) {
                return;
            }
            if (!VirtualProtect(pbCode,
                                cbCode,
                                PAGE_EXECUTE_READWRITE,
                                &m_dwOldPerm)) {
                return;
            }
        }
    }

    ~EnableWriteOnCodePage()
    {
        if (m_dwOldPerm && m_pbCode && m_cbCode) {
            DWORD dwTemp = 0;
            if (!FlushInstructionCache(m_hProcess, m_pbCode, m_cbCode)) {
                return;
            }
            if (!VirtualProtect(m_pbCode, m_cbCode, m_dwOldPerm, &dwTemp)) {
                return;
            }
        }
    }

    BOOL SetPermission(DWORD dwPerms)
    {
        if (m_dwOldPerm && m_pbCode && m_cbCode) {
            m_dwOldPerm = dwPerms;
            return TRUE;
        }
        return FALSE;
    }

    BOOL IsValid(VOID)
    {
        return m_pbCode && m_cbCode && m_dwOldPerm;
    }

private:
    HANDLE  m_hProcess;
    PBYTE   m_pbCode;
    LONG    m_cbCode;
    DWORD   m_dwOldPerm;
};



#endif//_TINY_DETOUR_H_