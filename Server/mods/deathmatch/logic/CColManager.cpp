/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CColManager.cpp
 *  PURPOSE:     Collision entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CColManager.h"
#include "CSpatialDatabase.h"
#include "Utils.h"

CColManager::CColManager()
{
    m_bCanRemoveFromList = true;
    m_bIteratingList = false;
}

CColManager::~CColManager()
{
    DeleteAll();
    TakeOutTheTrash();
}

void CColManager::DoHitDetection(const CVector& vecNowPosition, CElement* pEntity, CColShape* pJustThis, bool bChildren)
{
    if (pJustThis)
        DoHitDetectionForColShape(pJustThis);
    else
        DoHitDetectionForEntity(vecNowPosition, pEntity);
}

//
// Handle the changing state of collision between one colshape and any entity
//
void CColManager::DoHitDetectionForColShape(CColShape* pShape)
{
    // Ensure colshape is enabled and not being deleted
    if (pShape->IsBeingDeleted() || !pShape->IsEnabled())
        return;

    std::map<CElement*, int> entityList;

    // Get all entities within the sphere
    CSphere        querySphere = pShape->GetWorldBoundingSphere();
    CElementResult result;
    GetSpatialDatabase()->SphereQuery(result, querySphere);

    // Extract relevant types
    for (CElementResult::const_iterator it = result.begin(); it != result.end(); ++it)
    {
        CElement* pEntity = *it;
        switch (pEntity->GetType())
        {
            case CElement::COLSHAPE:
            case CElement::SCRIPTFILE:
            case CElement::RADAR_AREA:
            case CElement::CONSOLE:
            case CElement::TEAM:
            case CElement::BLIP:
            case CElement::DUMMY:
                break;
            default:
                if (pEntity->GetParentEntity())
                    entityList[pEntity] = 1;
        }
    }

    // Add existing colliders, so they can be disconnected if required
    for (std::list<CElement*>::const_iterator it = pShape->CollidersBegin(); it != pShape->CollidersEnd(); ++it)
    {
        entityList[*it] = 1;
    }

    // Test each entity against the colshape
    for (std::map<CElement*, int>::const_iterator it = entityList.begin(); it != entityList.end(); ++it)
    {
        CElement* pEntity = it->first;
        CVector   vecPosition = pEntity->GetPosition();

        // Collided?
        bool bHit = pShape->DoHitDetection(vecPosition);
        HandleHitDetectionResult(bHit, pShape, pEntity);
    }
}

//
// Handle the changing state of collision between one entity and any colshape
//
void CColManager::DoHitDetectionForEntity(const CVector& vecNowPosition, CElement* pEntity)
{
    std::map<CColShape*, int> shortList;

    // Get all entities within the sphere
    CElementResult queryResult;
    GetSpatialDatabase()->SphereQuery(queryResult, CSphere(vecNowPosition, 0.0f));

    // Extract colshapes
    for (CElementResult ::const_iterator it = queryResult.begin(); it != queryResult.end(); ++it)
        if ((*it)->GetType() == CElement::COLSHAPE)
            shortList[(CColShape*)*it] = 1;

    // Add existing collisions, so they can be disconnected if required
    for (std::list<CColShape*>::const_iterator it = pEntity->CollisionsBegin(); it != pEntity->CollisionsEnd(); ++it)
        shortList[*it] = 1;

    // Test each colshape against the entity
    for (std::map<CColShape*, int>::const_iterator it = shortList.begin(); it != shortList.end(); ++it)
    {
        CColShape* pShape = it->first;

        // Enabled and not being deleted?
        if (!pShape->IsBeingDeleted() && pShape->IsEnabled())
        {
            // Collided?
            bool bHit = pShape->DoHitDetection(vecNowPosition);
            HandleHitDetectionResult(bHit, pShape, pEntity);
        }
    }
}

//
// Handle the changing state of collision between one colshape and one entity
//
void CColManager::HandleHitDetectionResult(bool bHit, CColShape* pShape, CElement* pEntity)
{
    if (bHit)
    {
        // If they havn't collided yet
        if (!pEntity->CollisionExists(pShape))
        {
            // Add the collision and the collider
            pShape->AddCollider(pEntity);
            pEntity->AddCollision(pShape);

            // Can we call the event?
            if (pShape->GetAutoCallEvent() && !pEntity->IsBeingDeleted())
            {
                // Call the event
                CLuaArguments Arguments;
                Arguments.PushElement(pEntity);
                Arguments.PushBoolean((pShape->GetDimension() == pEntity->GetDimension()));
                pShape->CallEvent("onColShapeHit", Arguments);

                CLuaArguments Arguments2;
                Arguments2.PushElement(pShape);
                Arguments2.PushBoolean((pShape->GetDimension() == pEntity->GetDimension()));
                pEntity->CallEvent("onElementColShapeHit", Arguments2);
            }

            // Run whatever callback the collision item might have attached
            pShape->CallHitCallback(*pEntity);
        }
    }
    else
    {
        // If they collided before
        if (pEntity->CollisionExists(pShape))
        {
            // Remove the collision and the collider
            pShape->RemoveCollider(pEntity);
            pEntity->RemoveCollision(pShape);

            // Can we call the event?
            if (!pEntity->IsBeingDeleted())
            {
                // Call the event
                CLuaArguments Arguments;
                Arguments.PushElement(pEntity);
                Arguments.PushBoolean((pShape->GetDimension() == pEntity->GetDimension()));
                pShape->CallEvent("onColShapeLeave", Arguments);

                CLuaArguments Arguments2;
                Arguments2.PushElement(pShape);
                Arguments2.PushBoolean((pShape->GetDimension() == pEntity->GetDimension()));
                pEntity->CallEvent("onElementColShapeLeave", Arguments2);
            }

            pShape->CallLeaveCallback(*pEntity);
        }
    }
}

bool CColManager::Exists(CColShape* pShape)
{
    return ListContains(m_List, pShape);
}

void CColManager::DeleteAll()
{
    // Delete all of them
    DeletePointersAndClearList(m_List);
}

void CColManager::RemoveFromList(CColShape* pShape)
{
    if (m_bCanRemoveFromList)
    {
        // Dont wanna remove it if we're going through the list
        if (m_bIteratingList)
        {
            m_TrashCan.push_back(pShape);
        }
        else
        {
            ListRemove(m_List, pShape);
        }
    }
}

void CColManager::TakeOutTheTrash()
{
    std::vector<CColShape*>::const_iterator iter = m_TrashCan.begin();
    for (; iter != m_TrashCan.end(); iter++)
    {
        ListRemove(m_List, *iter);
    }

    m_TrashCan.clear();
}
