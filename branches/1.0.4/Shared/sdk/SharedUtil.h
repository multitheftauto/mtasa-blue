/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.h
*  PURPOSE:     Solution wide utility functions
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

/*************************************************************************
	Simplification of some 'unsigned' types
*************************************************************************/
typedef	unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned char	uchar;

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

#include "SharedUtil.Defines.h"
#include "SharedUtil.AllocTracking.h"
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <string>
#include <stdarg.h>
#include <assert.h>
#include "SString.h"
#include "SharedUtil.Misc.h"
#include "SharedUtil.File.h"
#include "SharedUtil.Time.h"
