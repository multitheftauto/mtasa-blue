/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHandlingManager.h
*  PURPOSE:     Vehicle handling manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CHANDLINGMANAGER_H
#define __CHANDLINGMANAGER_H

#include "CHandling.h"
#include <list>

class CHandlingManager
{
    friend class CHandling;

public:
                                CHandlingManager                ( void );
                                ~CHandlingManager               ( void );

    CHandling*                  Create                          ( CElement* pParent, CXMLNode* pNode = NULL );
    CHandling*                  CreateFromXML                   ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                        DeleteAll                       ( void );

    inline unsigned int         Count                           ( void )                            { return static_cast < unsigned int > ( m_List.size () ); };
    bool                        Exists                          ( CHandling* pHandling );

    inline list < CHandling* > ::const_iterator IterBegin       ( void )                            { return m_List.begin (); };
    inline list < CHandling* > ::const_iterator IterEnd         ( void )                            { return m_List.end (); };

    void                        AddDefaultHandling              ( unsigned short usID, CHandling* pHandling );
    void                        GetDefaultHandlings             ( unsigned short usID, std::vector < CHandling* >& List );

    inline CHandling*           GetOriginalHandling             ( unsigned short usID )             { return m_pDefaultHandlings [usID - 400]; };

    // Use the following functions to get the value that will be used in first->last priority
    float                       GetMass                         ( const std::vector < CHandling* >& List );
    float                       GetTurnMass                     ( const std::vector < CHandling* >& List );
    float                       GetDragCoeff                    ( const std::vector < CHandling* >& List );
    const CVector&              GetCenterOfMass                 ( const std::vector < CHandling* >& List );

    unsigned int                GetPercentSubmerged             ( const std::vector < CHandling* >& List );
    float                       GetTractionMultiplier           ( const std::vector < CHandling* >& List );

    CHandling::eDriveType       GetDriveType                    ( const std::vector < CHandling* >& List );
    CHandling::eEngineType      GetEngineType                   ( const std::vector < CHandling* >& List );
    unsigned char               GetNumberOfGears                ( const std::vector < CHandling* >& List );

    float                       GetEngineAccelleration          ( const std::vector < CHandling* >& List );
    float                       GetEngineInertia                ( const std::vector < CHandling* >& List );
    float                       GetMaxVelocity                  ( const std::vector < CHandling* >& List );

    float                       GetBrakeDecelleration           ( const std::vector < CHandling* >& List );
    float                       GetBrakeBias                    ( const std::vector < CHandling* >& List );
    bool                        GetABS                          ( const std::vector < CHandling* >& List );

    float                       GetSteeringLock                 ( const std::vector < CHandling* >& List );
    float                       GetTractionLoss                 ( const std::vector < CHandling* >& List );
    float                       GetTractionBias                 ( const std::vector < CHandling* >& List );

    float                       GetSuspensionForceLevel         ( const std::vector < CHandling* >& List );
    float                       GetSuspensionDamping            ( const std::vector < CHandling* >& List );
    float                       GetSuspensionHighSpeedDamping   ( const std::vector < CHandling* >& List );
    float                       GetSuspensionUpperLimit         ( const std::vector < CHandling* >& List );
    float                       GetSuspensionLowerLimit         ( const std::vector < CHandling* >& List );
    float                       GetSuspensionFrontRearBias      ( const std::vector < CHandling* >& List );
    float                       GetSuspensionAntidiveMultiplier ( const std::vector < CHandling* >& List );

    float                       GetCollisionDamageMultiplier    ( const std::vector < CHandling* >& List );

    unsigned int                GetHandlingFlags                ( const std::vector < CHandling* >& List );
    unsigned int                GetModelFlags                   ( const std::vector < CHandling* >& List );
    float                       GetSeatOffsetDistance           ( const std::vector < CHandling* >& List );

    CHandling::eLightType       GetHeadLight                    ( const std::vector < CHandling* >& List );
    CHandling::eLightType       GetTailLight                    ( const std::vector < CHandling* >& List );
    unsigned char               GetAnimGroup                    ( const std::vector < CHandling* >& List );

private:
    void                        RestoreHandling                 ( unsigned short usID );

    inline void                 AddToList                       ( CHandling* pHandling )            { m_List.push_back ( pHandling ); };
    void                        RemoveFromList                  ( CHandling* pHandling );

    void                        InitializeOriginalHandlings     ( void );

    bool                        m_bRemoveFromList;
    list < CHandling* >         m_List;
    CHandling*                  m_pDefaultHandlings [212];
};

#endif
