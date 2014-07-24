/*-----------------------------------------------------------------------------
 * DebugMessage.h
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * - 06.06.2010 created
**---------------------------------------------------------------------------*/

#ifndef _debug_message_
#define _debug_message_

#define DBG_OPN     DebugMessage(__FUNCTION__, __LINE__,
#define DBG_END     );

#define	DBG_ERR		DebugMessage(__FUNCTION__, __LINE__, "[ERR ]", 
#define DBG_WARN	DebugMessage(__FUNCTION__, __LINE__, "[WARN]",
#define DBG_INFO	DebugMessage(__FUNCTION__, __LINE__, "[INFO]",


#define	log_err		DebugMessage(__FUNCTION__, __LINE__, "[ERR ]", 
#define log_warn	DebugMessage(__FUNCTION__, __LINE__, "[WARN]",
#define log_info	DebugMessage(__FUNCTION__, __LINE__, "[INFO]",
#define log_end		);


void 
DebugMessage(
    const char* Function,
    int Line,
    const char* LogPrefix,    
    const char* fmt, 
    ...
    );

#endif//_debug_message_