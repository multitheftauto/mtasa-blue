/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHandlingManager.h
*  PURPOSE:     Header file for vehicle handling manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CHandlingManager;

#ifndef __CHANDLINGMANAGER_H
#define __CHANDLINGMANAGER_H

#include "CHandlingEntry.h"

class CHandlingManager
{
public:
                                CHandlingManager                ( void );
                                ~CHandlingManager               ( void );

    CHandlingEntry*             CreateHandlingData              ( void );

    bool                        ApplyHandlingData               ( eVehicleTypes eModel, CHandlingEntry* pEntry );

    const CHandlingEntry*       GetModelHandlingData            ( eVehicleTypes eModel );
    const CHandlingEntry*       GetOriginalHandlingData         ( eVehicleTypes eModel );

    eHandlingTypes              GetHandlingID                   ( eVehicleTypes eModel );

    // Helper functions
    eHandlingProperty           GetPropertyEnumFromName         ( std::string strName);
    bool                        HasModelHandlingChanged         ( eVehicleTypes eModel);
    void                        SetModelHandlingHasChanged      ( eVehicleTypes eModel, bool bChanged );

    std::map < std::string, eHandlingProperty > m_HandlingNames;
private:
    void                        InitializeDefaultHandlings      ( void );

    // Original handling data unaffected by handling.cfg changes
    static SFixedArray < tHandlingData, HT_MAX >        m_OriginalHandlingData;

    // Array with the original handling entries
    static SFixedArray < CHandlingEntry*, HT_MAX >      m_pOriginalEntries;

    // Array with the model handling entries
    static SFixedArray < CHandlingEntry*, HT_MAX >      m_pModelEntries;

    SFixedArray < bool, HT_MAX >                        m_bModelHandlingChanged;
};

#endif
