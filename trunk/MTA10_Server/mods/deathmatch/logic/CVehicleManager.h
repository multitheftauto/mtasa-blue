/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleManager.h
*  PURPOSE:     Vehicle entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVehicleManager;

#ifndef __CVEHICLEMANAGER_H
#define __CVEHICLEMANAGER_H

#include "CVehicle.h"
#include "CVehicleColorManager.h"
#include <list>
#include "lua/CLuaMain.h"

// Undefined number of passengers (to disable custom passenger seats overriding in CVehicle)
#define VEHICLE_PASSENGERS_UNDEFINED    255

class CVehicleManager
{
    friend class CVehicle;

public:
                                        CVehicleManager             ( void );
                                        ~CVehicleManager            ( void );

    CVehicle*                           Create                      ( unsigned short usModel, CElement* pParent, CXMLNode* pNode = NULL );
    CVehicle*                           CreateFromXML               ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents );
    void                                DeleteAll                   ( void );

    inline unsigned int                 Count                       ( void )                            { return static_cast < unsigned int > ( m_List.size () ); };
    bool                                Exists                      ( CVehicle* pVehicle );

    static bool                         IsValidModel                ( unsigned int uiVehicleModel );
    static eVehicleType                 GetVehicleType              ( unsigned short usModel );
    static bool                         IsValidUpgrade              ( unsigned short usUpgrade );
    static unsigned int                 GetMaxPassengers            ( unsigned int uiVehicleModel );
    static bool                         HasTurret                   ( unsigned int uiVehicleModel );
    static bool                         HasSirens                   ( unsigned int uiVehicleModel );
    static bool                         HasTaxiLight                ( unsigned int uiVehicleModel );
    static bool                         HasLandingGears             ( unsigned int uiVehicleModel );
    static bool                         HasAdjustableProperty       ( unsigned int uiVehicleModel );
    static bool                         HasSmokeTrail               ( unsigned int uiVehicleModel );
    static bool                         IsTrailer                   ( unsigned int uiVehicleModel );
    static bool                         HasDamageModel              ( unsigned short usModel );
    static bool                         HasDamageModel              ( enum eVehicleType Type );
    static bool                         HasDoors                    ( unsigned short usModel );

    inline CVehicleColorManager*        GetColorManager             ( void )                            { return &m_ColorManager; };
    CVehicleColor                       GetRandomColor              ( unsigned short usModel );

    void                                GetVehiclesOfType           ( unsigned int uiModel, lua_State* luaVM );

    list < CVehicle* > ::const_iterator IterBegin                   ( void )                            { return m_List.begin (); };
    list < CVehicle* > ::const_iterator IterEnd                     ( void )                            { return m_List.end (); };

private:
    inline void                         AddToList                   ( CVehicle* pVehicle )              { m_List.push_back ( pVehicle ); };
    void                                RemoveFromList              ( CVehicle* pVehicle );

    CVehicleColorManager                m_ColorManager;

    list < CVehicle* >                  m_List;
    bool                                m_bDontRemoveFromList;
};

#endif
