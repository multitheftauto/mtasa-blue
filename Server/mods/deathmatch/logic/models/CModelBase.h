/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelBase.h
 *  PURPOSE:     Base model info class header
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

class CModelBase;
#pragma once

#include <string_view>

#define MODEL_MISSING_PARENT -1

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
    CModelBase(eModelInfoType eType, uint32_t uiModelID) : m_eType(eType), m_uiModelID(uiModelID), m_uiParentID(MODEL_MISSING_PARENT){};

    virtual ~CModelBase(){};

    virtual CModelBase* Clone(uint32_t uiModelID) = 0;

    const uint32_t GetModelID() { return m_uiModelID; };
    uint32_t       GetParentModel() { return m_uiParentID; };
    void           SetParentModel(uint32_t uiParentModel) { m_uiParentID = uiParentModel; };
    bool           IsCustom() { return m_uiParentID != MODEL_MISSING_PARENT; };
    virtual void   Unload() = 0;

    eModelInfoType GetType() { return m_eType; };

protected:
    eModelInfoType m_eType;
    // Model id
    const uint32_t m_uiModelID;

    // Model prototype
    uint32_t m_uiParentID;
};

#undef MODEL_MISSING_PARENT
