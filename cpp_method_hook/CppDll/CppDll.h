/*-----------------------------------------------------------------------------
 * CppDLL
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 2007.01.22 created
**---------------------------------------------------------------------------*/
#ifdef CPPDLL_EXPORTS
#define CPPDLL_API __declspec(dllexport)
#else
#define CPPDLL_API __declspec(dllimport)
#endif


class CPPDLL_API CCppDll 
{
public:
	CCppDll(void);

	DWORD CppMethod(DWORD param1, DWORD param2);

private:
	DWORD m_dummy;
	DWORD m_Value;

};

