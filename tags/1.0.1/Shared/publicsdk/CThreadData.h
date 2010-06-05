/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

class CThreadData;

#ifndef __CTHREADDATA_H
#define __CTHREADDATA_H

#include "CThread.h"

class CThreadData
{
public:
						CThreadData			( void );
						~CThreadData		( void );

	bool				bAbortThread;
	ThreadMutex			MutexPrimary;		// primary mutex for suspend/resume operations
	ThreadMutex			MutexLogical;		// logical mutex for proper CThreadData sync
};

#endif
