/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColStore.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

class CVector;

using CollisionSlot = std::uint8_t;

class CColStore
{
protected:
    ~CColStore() = default;

public:
    struct BoundingBox
    {
        float left;
        float bottom;
        float right;
        float top;
    };

    // Initialises the collision store, its pool and quad-tree
    virtual void Initialise() = 0;

    // Shuts down the collision store and releases all acquired memory
    virtual void Shutdown() = 0;

    // Post-processing for bounding boxes after loading a level
    virtual void BoundingBoxesPostProcess() = 0;

    // Creates a new collision pool slot and returns the slot number
    virtual int AddColSlot(const char* name) = 0;

    // Returns true, if the collision pool slot is active (read: non-empty pool index)
    virtual bool IsValidSlot(CollisionSlot slot) = 0;

    // Sets a position to mark the collisions slots near the position as required in the next call to `LoadCollision`
    virtual void AddCollisionNeededAtPosition(const CVector& position) = 0;

    // For each active collision slot in range: request and load the collision model and mark the slot as required
    virtual void EnsureCollisionIsInMemory(const CVector& position) = 0;

    // Returns true, if each active collision slot in range is loaded
    virtual bool HasCollisionLoaded(const CVector& position, int areaCode) = 0;

    // For each active collision slot in the required range: request the collision model
    virtual void RequestCollision(const CVector& position, int areaCode) = 0;

    // For each active collision slot in the required range: mark it as required (the caller is expected to clear it later)
    virtual void SetCollisionRequired(const CVector& position, int areaCode) = 0;

    // For each active collision slot: unload the collision model
    virtual void RemoveAllCollision() = 0;

    // Increases the reference counter for the collision pool slot
    virtual void AddRef(CollisionSlot slot) = 0;

    // Decreases the reference counter for the collision pool slot
    virtual void RemoveRef(CollisionSlot slot) = 0;

    // Removes the collision volumes for each model in the collision pool slot
    virtual void RemoveCol(CollisionSlot slot) = 0;

    // Delete the collision slot from the collision pool
    virtual void RemoveColSlot(CollisionSlot slot) = 0;

    // Loads the bounding boxes for each active collision pool slot
    virtual void LoadAllBoundingBoxes() = 0;

    // Returns the bounding box of the collision pool slot
    virtual BoundingBox GetBoundingBox(CollisionSlot slot) = 0;

    // Adds the model id to the collision pool slot model range
    virtual void IncludeModelIndex(CollisionSlot slot, std::uint16_t model) = 0;

    // Returns the first model id in the collision pool slot model range
    virtual int GetFirstModel(CollisionSlot slot) = 0;

    // Returns the last model id in the collision pool slot model range
    virtual int GetLastModel(CollisionSlot slot) = 0;
};
