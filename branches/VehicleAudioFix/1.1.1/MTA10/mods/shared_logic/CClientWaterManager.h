/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWaterManager.h
*  PURPOSE:     Water entity manager class header
*  DEVELOPERS:  arc_
*
*****************************************************************************/

class CClientWaterManager;

#ifndef __CCLIENTWATERMANAGER_H
#define __CCLIENTWATERMANAGER_H

#include "CClientManager.h"
#include "CClientWater.h"

class CClientWaterManager
{
    friend class CClientManager;
    friend class CClientWater;

public:
    inline unsigned int                             Count                       ( void )            { return static_cast < unsigned int > ( m_List.size () ); };
    static CClientWater*                            Get                         ( ElementID ID );

    void                                            DeleteAll                   ( void );
    bool                                            Exists                      ( CClientWater* pWater );

    bool                                            GetWaterLevel               ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown );
    bool                                            SetWaterLevel               ( CVector* pvecPosition, float fLevel, void* pChangeSource = NULL );
    bool                                            SetWaterLevel               ( CClientWater* pWater, float fLevel, void* pChangeSource = NULL );
    float                                           GetWaveLevel                ();
    void                                            SetWaveLevel                ( float fWaveLevel );

    inline std::list < CClientWater* > ::const_iterator  IterBegin                   ( void )            { return m_List.begin (); };
    inline std::list < CClientWater* > ::const_iterator  IterEnd                     ( void )            { return m_List.end (); };

private:
                                                    CClientWaterManager         ( CClientManager* pManager );
                                                    ~CClientWaterManager        ( void );

    void                                            AddToList                   ( CClientWater* pWater );
    void                                            RemoveFromList              ( CClientWater* pWater );

    CClientManager*                                 m_pManager;

    std::list < CClientWater* >                     m_List;
    bool                                            m_bDontRemoveFromList;
};

#endif
