/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.h
*  PURPOSE:     Sound entity class header
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*
*****************************************************************************/

class CClientSound;

#ifndef __CCLIENTSOUND_H
#define __CCLIENTSOUND_H

#include "CClientSoundManager.h"
#include "CClientEntity.h"

#define MAX_SOUND_DISTANCE 100

using namespace irrklang;

class CClientSound : public CClientEntity
{
    friend CClientSoundManager;

public:

                            CClientSound            ( CClientManager* pManager, ElementID ID );
                            ~CClientSound           ( void );

    eClientEntityType       GetType                 ( void ) const                      { return CCLIENTSOUND; }

    bool                    Play                    ( const char* szPath, bool bLoop );
    bool                    Play3D                  ( const char* szPath, CVector vecPosition, bool bLoop );
    void                    Stop                    ( void );

    void                    SetPaused               ( bool bPaused );
    bool                    IsPaused                ( void );

    bool                    IsFinished              ( void );

    void                    SetPlayPosition         ( unsigned int uiPosition );
    unsigned int            GetPlayPosition         ( void );

    unsigned int            GetLength               ( void );

    void                    SetVolume               ( float fVolume );
    float                   GetVolume               ( void );

    void                    SetMinDistance          ( float fDistance );
    float                   GetMinDistance          ( void );

    void                    SetMaxDistance          ( float fDistance );
    float                   GetMaxDistance          ( void );

    void                    Unlink                  ( void ) {};
    void                    GetPosition             ( CVector& vecPosition ) const;
    void                    SetPosition             ( const CVector& vecPosition );

    void                    SetDimension            ( unsigned short usDimension );
    void                    RelateDimension         ( unsigned short usDimension );


protected:

    ISound*                 GetSound                ( void )                            { return m_pSound; };
    void                    Process3D               ( CVector vecPosition, CVector vecLookAt );
    void                    Set3D                   ( bool b3D )                        { m_b3D = b3D; };

private:

    float                   m_fVolume;
    float                   m_fDistance;
    float                   m_fMinDistance;

    CVector                 m_vecPosition;

    CClientSoundManager*    m_pSoundManager;
    ISound*                 m_pSound;

    bool                    m_b3D;
};

#endif
