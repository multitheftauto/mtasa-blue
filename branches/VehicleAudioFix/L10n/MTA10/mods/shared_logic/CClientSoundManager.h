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
    ZERO_ON_NEW
                            CClientSoundManager         ( CClientManager* pClientManager );
                            ~CClientSoundManager        ( void );

    void                    DoPulse                     ( void );

    void                    SetDimension                ( unsigned short usDimension );

    CClientSound*           PlaySound2D                 ( const SString& strSound, bool bIsURL, bool bLoop );
    CClientSound*           PlaySound3D                 ( const SString& strSound, bool bIsURL, const CVector& vecPosition, bool bLoop );

    void                    AddToList                   ( CClientSound* pSound );
    void                    RemoveFromList              ( CClientSound* pSound );

    int                     GetFxEffectFromName         ( const std::string& strEffectName );

    std::map < std::string, int >  GetFxEffects         ( void )                    { return m_FxEffectNames; }

    void                    UpdateVolume                ( void );

    void                    UpdateDistanceStreaming     ( const CVector& vecListenerPosition );

    void                    OnDistanceStreamIn          ( CClientSound* pSound );
    void                    OnDistanceStreamOut         ( CClientSound* pSound );

    bool                    IsMinimizeMuted             ( void )                    { return m_bMinimizeMuted; };
    void                    SetMinimizeMuted            ( bool bMute )              { m_bMinimizeMuted = bMute; };

private:

    CClientManager*                 m_pClientManager;

    unsigned short                  m_usDimension;

    std::list < CClientSound* >     m_Sounds;
    std::set < CClientSound* >      m_DistanceStreamedInMap;

    std::map < std::string, int >   m_FxEffectNames;
    SString                         m_strUserAgent;

    bool                            m_bMinimizeMuted;
};

#endif
