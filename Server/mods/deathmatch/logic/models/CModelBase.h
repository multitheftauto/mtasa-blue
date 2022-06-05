/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelBase.h
 *  PURPOSE:     Base model info class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CModelBase;
#pragma once

#include <string_view>

enum class eModelInfoType : unsigned char
{
    INVALID = 0,
    ATOMIC = 1,
    UNKNOWN = 2,
    TIME = 3,
    WEAPON = 4,
    CLUMP = 5,
    VEHICLE = 6,
    PED = 7,
    LOD_ATOMIC = 8,
};

class CModelBase
{
public:
    CModelBase(uint32_t uiModelID): m_uiModelID(uiModelID), m_uiParentID(-1)
    {};

    ~CModelBase(){};

    virtual CModelBase* Clone(uint32_t uiModelID) = 0;

    uint32_t         GetModelID() { return m_uiModelID; };
    uint32_t         GetParentModel() { return m_uiParentID; };
    void             SetParentModel(uint32_t uiParentModel) { m_uiParentID = uiParentModel; };
    virtual bool     SetName(std::string_view strName) { return false; };
    virtual bool     IsCustom() { return true; };
    virtual void     Unload() = 0;

    eModelInfoType GetType() { return m_type; };

protected:
    eModelInfoType m_type = eModelInfoType::INVALID;
    // Model id
    uint32_t m_uiModelID;

    // Model prototype
    uint32_t m_uiParentID;
};
