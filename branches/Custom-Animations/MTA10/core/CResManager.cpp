/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CResManager.cpp
*  PURPOSE:     Resolution manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*  Original code by Ashley Matheson
*  http://www.gamedev.net/reference/articles/article1009.asp
*
*****************************************************************************/

#include "StdInc.h"

CResManager::CResManager()
{
  Init();
  m_bModeHasBeenSet = false;
}

CResManager::CResManager(int width, int height, int depth)
{
    Init();
    m_bModeHasBeenSet = false;

    ChangeRes(width, height, depth);
}

CResManager::~CResManager()
{
    // clean out the list of DeviceModes
    if (m_stdDeviceModes.size() > 0)
    {
        m_stdDeviceModes.clear ();
    }
}

void CResManager::Init()
{
    int  nModeExist;
    DEVMODE devMode;
    memset ( &devMode, 0, sizeof ( DEVMODE ) );
    devMode.dmSize = sizeof(DEVMODE);

    // Get the current list of Available DevModes and store them into 
    for (int i=0; ;i++) 
    {
        nModeExist = EnumDisplaySettings(NULL, i, &devMode);

        if (nModeExist != 1) 
        {
            // End of modes.  bail out.
            break;
        }
        else
        {
            // Add the driver to the list.
            m_stdDeviceModes.push_front(devMode);
        }
    }
}

void CResManager::ChangeRes(int width, int height, int depth)
{
    bool success = false;
    int nModeSwitch;

    // Iterate through the DeviceModes list, looking for a match
    // if one is found, switch to that resolution and bail.
    // Otherwise, leave alone.
    std::list<DEVMODE>::iterator resIter = m_stdDeviceModes.begin();
    for ( ; resIter != m_stdDeviceModes.end (); resIter++ )
    {
        if ( resIter->dmBitsPerPel == depth &&
             resIter->dmPelsWidth == width &&
             resIter->dmPelsHeight == height &&
             resIter->dmOrientation == 0 /* DMDO_DEFAULT  */ )
        {
            /*
            nModeSwitch = ChangeDisplaySettings ( &(*resIter), CDS_FULLSCREEN ); 
            if ( nModeSwitch==DISP_CHANGE_SUCCESSFUL ) 
            {
                success = true;
                break;
            }
            */

            // Whoops, it didn't work.  Possibly running Win95?  Try without switching resolutions
            int nCloseMode = 0;
            EnumDisplaySettings(NULL, nCloseMode, &(*resIter));
            resIter->dmBitsPerPel = depth; 
            resIter->dmPelsWidth  = width; 
            resIter->dmPelsHeight = height; 
            resIter->dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 

            nModeSwitch = ChangeDisplaySettings(&(*resIter), CDS_FULLSCREEN); 
            if ( nModeSwitch == DISP_CHANGE_SUCCESSFUL ) 
            {
                success = true;
                break;
            }

            // Nope, what else can we try?  Separately change the BitDepth and then resolution
            resIter->dmFields = DM_BITSPERPEL; 
            nModeSwitch = ChangeDisplaySettings ( &(*resIter), CDS_FULLSCREEN );
            if ( nModeSwitch == DISP_CHANGE_SUCCESSFUL ) 
            {
                resIter->dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; 
                nModeSwitch = ChangeDisplaySettings ( &(*resIter), CDS_FULLSCREEN ); 
                if ( nModeSwitch == DISP_CHANGE_SUCCESSFUL ) 
                {
                    success = true;
                    break;
                }

                ChangeDisplaySettings ( NULL, 0 );
                break;
            }
        }
    }

    // Did we change the resolution?
    if ( true == success )
    {
        m_Current = (*resIter);
        m_bModeHasBeenSet = true;
        //CCore::GetSingleton ().GetConsole ()->Echo ( "Success!");
    }
    else
    {
        // error
    }
}

void CResManager::RestoreResolution()
{
    ChangeDisplaySettings(NULL, CDS_RESET);
}

void CResManager::RestoreCurrent()
{
    if ( m_bModeHasBeenSet )
    {
        ChangeRes ( m_Current.dmPelsWidth, m_Current.dmPelsHeight, m_Current.dmBitsPerPel ); 
    }
}
