/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CBuildingRemoval.h
 *  PURPOSE:     Game world interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *

 *****************************************************************************/

#pragma once

#include <list>

class CEntitySAInterface;
class CVector;
class CVector4D;

struct SBuildingRemoval
{
    SBuildingRemoval()
    {
        m_pBinaryRemoveList = new std::list<CEntitySAInterface*>;
        m_pDataRemoveList = new std::list<CEntitySAInterface*>;
        m_usModel = 0;
        m_vecPos = CVector(0, 0, 0);
        m_fRadius = 0.0f;
        m_cInterior = -1;
    }

    ~SBuildingRemoval()
    {
        delete m_pBinaryRemoveList;
        delete m_pDataRemoveList;
    }

    void AddBinaryBuilding(CEntitySAInterface* pInterface)
    {
        // Add to list of binary buildings for this removal
        m_pBinaryRemoveList->push_back(pInterface);
    }
    void AddDataBuilding(CEntitySAInterface* pInterface)
    {
        // Add to list of data buildings for this removal
        m_pDataRemoveList->push_back(pInterface);
    }

    unsigned short                  m_usModel;
    CVector                         m_vecPos;
    float                           m_fRadius;
    char                            m_cInterior;
    std::list<CEntitySAInterface*>* m_pBinaryRemoveList;
    std::list<CEntitySAInterface*>* m_pDataRemoveList;
};

struct sDataBuildingRemovalItem
{
    sDataBuildingRemovalItem(CEntitySAInterface* pInterface, bool bData)
    {
        m_pInterface = pInterface;
        m_iCount = 0;
    }
    void                AddCount() { m_iCount++; }
    void                RemoveCount() { m_iCount--; }
    CEntitySAInterface* m_pInterface;
    int                 m_iCount;
};

struct sBuildingRemovalItem
{
    sBuildingRemovalItem(CEntitySAInterface* pInterface, bool bData)
    {
        m_pInterface = pInterface;
        m_iCount = 0;
    }
    void                AddCount() { m_iCount++; }
    void                RemoveCount() { m_iCount--; }
    CEntitySAInterface* m_pInterface;
    int                 m_iCount;
};

struct SIPLInst
{
    CVector   m_pPosition;
    CVector4D m_pRotation;
    int32_t   m_nModelIndex;
    union
    {
        struct
        {
            uint32_t m_nAreaCode : 8;
            uint32_t m_bRedundantStream : 1;
            uint32_t m_bDontStream : 1;
            uint32_t m_bUnderwater : 1;
            uint32_t m_bTunnel : 1;
            uint32_t m_bTunnelTransition : 1;
            uint32_t m_nReserved : 19;
        };
        uint32_t m_nInstanceType;
    };
    int32_t m_nLodInstanceIndex;
};
static_assert(sizeof(SIPLInst) == 0x28, "Invalid sizeof(SIPLInst)");

class CBuildingRemoval
{
public:
    virtual SBuildingRemoval* GetBuildingRemoval(CEntitySAInterface* pInterface) = 0;

    virtual void RemoveBuilding(uint16_t usModelToRemove, float fDistance, float fX, float fY, float fZ, char cInterior, size_t* pOutAmount = NULL) = 0;
    virtual bool IsRemovedModelInRadius(SIPLInst* pInst) = 0;
    virtual bool IsModelRemoved(uint16_t usModelID) = 0;
    virtual void ClearRemovedBuildingLists(size_t* pOutAmount = NULL) = 0;
    virtual bool RestoreBuilding(uint16_t usModelToRestore, float fDistance, float fX, float fY, float fZ, char cInterior, size_t* pOutAmount = NULL) = 0;
    virtual void AddDataBuilding(CEntitySAInterface* pInterface) = 0;
    virtual void AddBinaryBuilding(CEntitySAInterface* pInterface) = 0;
    virtual void RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface) = 0;
    virtual bool IsObjectRemoved(CEntitySAInterface* pInterface) = 0;
    virtual bool IsDataModelRemoved(uint16_t usModelID) = 0;
    virtual bool IsEntityRemoved(CEntitySAInterface* pInterface) = 0;

};
