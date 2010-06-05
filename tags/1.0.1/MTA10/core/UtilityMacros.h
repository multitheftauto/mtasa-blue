/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/UtilityMacros.h
*  PURPOSE:     Miscellaneous utility macros
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __UTILITYMACROS_H
#define __UTILITYMACROS_H

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

#define SAFE_DELETE_CAST(p,c) { if (p) { delete static_cast<c>(p); (p)=NULL; } }

#define GET_LENGTH_BOUND(x,y) ( x < y ? x : y )

#endif