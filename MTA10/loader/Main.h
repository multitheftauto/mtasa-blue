/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Main.h
*  PURPOSE:     MTA loader
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __MAIN_H
#define __MAIN_H

#define MTA_GTAEXE_NAME "gta_sa.exe"

#ifdef MTA_DEBUG
    #define MTA_DLL_NAME "core_d.dll"
#else
    #define MTA_DLL_NAME "core.dll"
#endif

#endif
