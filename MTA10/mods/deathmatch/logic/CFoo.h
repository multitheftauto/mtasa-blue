/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CFoo.h
*  PURPOSE:     Debugging class (not used in Release mode)
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/* This class is purely for debugging things. This will avoid usage of CClientGame. */

#ifndef __CFOO_H
#define __CFOO_H

class CFoo
{
public:
    #ifdef MTA_DEBUG
        void                Init                ( class CClientGame* pClientGame ) { g_pClientGame = pClientGame; };

        void                DoPulse             ( void );
        void                Test                ( const char* szString );

        class CClientGame*  g_pClientGame;
    #else
        inline void         Init               ( CClientGame* pClientGame ) {};
        inline void         DoPulse            ( void ) {};
        inline void         Test               ( const char* szString ) {};
    #endif
};

#endif
