/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CResManager.h
*  PURPOSE:     Header file for display resolution manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*  Original code by Ashley Matheson
*  http://www.gamedev.net/reference/articles/article1009.asp
*
*  Class:   ResManager
*  Date:    Aug 15, 1999
*  Desc:    A wrapper class for enumeration of display modes, <nl>
*           resolution changing, and restoration of video modes.
* 
*  Method Overview ----------------------------------------------
*  +ResManager()    
*       -> Default Constructior.  Enumerates the Video Modes
*   +ResManager(int width, int Height, int depth)
*       -> Paramaterized constructior.  Calls the default construction and sets the desired video mode.
*   +~ResManager()
*       -> Default Destructior.
*   +void ChangeRes(int width, int height, int depth)
*       -> Sets the desired video mode.
*   +void RestoreResolution();
*       -> Restores the resolution to the initial Video mode.
*  Property Overview -------------------------------------------
*   -DEVMODE m_Current
*       -> The current Video mode
*   -list<DEVMODE> m_stdDeviceModes
*       -> A list containing the available video modes.
*   History:
*   08/15/99    devint  AWM Creation
*
*****************************************************************************/

#include <stdio.h>
#include <list>

#ifndef _CRES_MANAGER
#define _CRES_MANAGER

#include <windows.h>

class CResManager
{
public:
    CResManager();
    CResManager(int width, int height, int depth);
    ~CResManager();

    void Init();

    void ChangeRes(int width, int height, int depth);
    void RestoreResolution();
    void RestoreCurrent();
private:
    bool m_bModeHasBeenSet;
    DEVMODE m_Current;
    std::list<DEVMODE> m_stdDeviceModes;

};

#endif
