/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColStoreSA.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CColStore.h>

class CColStoreSA final : public CColStore
{
public:
    void        Initialise() override;
    void        Shutdown() override;
    void        BoundingBoxesPostProcess() override;
    int         AddColSlot(const char* name) override;
    bool        IsValidSlot(CollisionSlot slot) override;
    void        AddCollisionNeededAtPosition(const CVector& position) override;
    void        EnsureCollisionIsInMemory(const CVector& position) override;
    bool        HasCollisionLoaded(const CVector& position, int areaCode) override;
    void        RequestCollision(const CVector& position, int areaCode) override;
    void        SetCollisionRequired(const CVector& position, int areaCode) override;
    void        RemoveAllCollision() override;
    void        AddRef(CollisionSlot slot) override;
    void        RemoveRef(CollisionSlot slot) override;
    void        RemoveCol(CollisionSlot slot) override;
    void        RemoveColSlot(CollisionSlot slot) override;
    void        LoadAllBoundingBoxes() override;
    BoundingBox GetBoundingBox(CollisionSlot slot) override;
    void        IncludeModelIndex(CollisionSlot slot, std::uint16_t model) override;
    int         GetFirstModel(CollisionSlot slot) override;
    int         GetLastModel(CollisionSlot slot) override;
};
