/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSoundManager.h
*  PURPOSE:     Sound manager class header
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

class CClientSoundManager;

#ifndef __CCLIENTSOUNDMANAGER_H
#define __CCLIENTSOUNDMANAGER_H

#include <list>
#include <bass.h>
#include "CClientSound.h"

class CClientSoundManager
{
public:

                            CClientSoundManager         ( CClientManager* pClientManager );
                            ~CClientSoundManager        ( void );

    void                    DoPulse                     ( void );

    inline unsigned short   GetDimension                ( void )                    { return m_usDimension; }
    void                    SetDimension                ( unsigned short usDimension );

    CClientSound*           PlaySound2D                 ( const SString& strSound, bool bIsURL, bool bLoop );
    CClientSound*           PlaySound3D                 ( const SString& strSound, bool bIsURL, const CVector& vecPosition, bool bLoop );

    void                    AddToList                   ( CClientSound* pSound )    { m_Sounds.push_back ( pSound ); }
    void                    RemoveFromList              ( CClientSound* pSound )    { m_Sounds.remove ( pSound ); }
    bool                    Exists                      ( CClientSound* pSound );
    CClientSound*           Get                         ( DWORD pSound );

    int                     GetFxEffectFromName         ( const std::string& strEffectName );

    std::map < std::string, int >  GetFxEffects         ( void )                    { return m_FxEffectNames; }

    void                    UpdateVolume                ( void );

private:

    CClientManager*                 m_pClientManager;

    unsigned short                  m_usDimension;

    std::list < CClientSound* >     m_Sounds;

    std::map < std::string, int >   m_FxEffectNames;
};

#endif
