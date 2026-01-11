/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPickup.cpp
 *  PURPOSE:     Pickup entity class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <game/CPickups.h>
#include <game/CPickup.h>

extern CClientGame* g_pClientGame;

CClientPickup::CClientPickup(CClientManager* pManager, ElementID ID, unsigned short usModel, CVector vecPosition)
    : ClassInit(this), CClientStreamElement(pManager->GetPickupStreamer(), ID)
{
    // Initialize
    m_pManager = pManager;
    m_pPickupManager = pManager->GetPickupManager();
    m_usModel = usModel;
    m_bVisible = true;
    m_pPickup = NULL;
    m_pObject = NULL;
    m_pCollision = NULL;

    // Add us to pickup manager's list
    m_pPickupManager->m_List.push_back(this);

    SetTypeName("pickup");
    m_ucType = CClientPickup::WEAPON;
    m_ucWeaponType = CClientPickup::WEAPON_BRASSKNUCKLE;
    m_fAmount = 0.0f;
    m_usAmmo = 0;

    // Make sure our streamer knows where we are to start with
    UpdateStreamPosition(m_vecPosition = vecPosition);
}

CClientPickup::~CClientPickup()
{
    AttachTo(NULL);

    // Make sure our pickup is destroyed
    Destroy();

    // Remove us from the pickup manager's list
    Unlink();
}

void CClientPickup::Unlink()
{
    m_pPickupManager->RemoveFromList(this);
}

void CClientPickup::GetPosition(CVector& vecPosition) const
{
    if (m_pAttachedToEntity)
    {
        m_pAttachedToEntity->GetPosition(vecPosition);
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}

void CClientPickup::SetPosition(const CVector& vecPosition)
{
    // Different from our current position?
    if (m_vecPosition != vecPosition)
    {
        // Set the new position and recreate it
        m_vecPosition = vecPosition;
        ReCreate();

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }
}

void CClientPickup::SetModel(unsigned short usModel)
{
    // Different from our current model?
    if (m_usModel != usModel)
    {
        // Set the model and recreate the pickup
        m_usModel = usModel;
        UpdateSpatialData();
        ReCreate();
    }
}

void CClientPickup::AttachTo(CClientEntity* pEntity)
{
    CClientEntity::AttachTo(pEntity);

    if (m_pAttachedToEntity)
    {
        DoAttaching();
        UpdateStreamPosition(m_vecPosition);
    }
}

void CClientPickup::SetVisible(bool bVisible)
{
    // Update the flag
    m_bVisible = bVisible;

    // Only update visible state if we're streamed in
    if (IsStreamedIn())
    {
        if (bVisible)
        {
            // Attempt to create - revert visibility if failed
            if (!Create())
            {
                m_bVisible = false;
            }
        }
        else
            Destroy();
    }
}

void CClientPickup::StreamIn(bool bInstantly)
{
    // Invisible pickups don't need a game object - just notify streamer
    if (!m_bVisible)
    {
        NotifyCreate();
        return;
    }

    // Create the pickup and notify the streamer of the result
    if (Create())
    {
        NotifyCreate();
    }
    else
    {
        NotifyUnableToCreate();
    }
}

void CClientPickup::StreamOut()
{
    // Destroy the pickup
    Destroy();
}

bool CClientPickup::Create()
{
    if (!m_pPickup && m_bVisible)
    {
        // Grab the attributes from the MTA interface for this pickup
        unsigned char  ucAreaCode = GetInterior();
        unsigned short usDimension = GetDimension();

        // Create the pickup
        m_pPickup = g_pGame->GetPickups()->CreatePickup(&m_vecPosition, m_usModel, PickupType::PICKUP_ONCE);
        m_pObject = nullptr;
        if (m_pPickup)
        {
            // Make sure we have an object - request one if needed
            if (!m_pPickup->GetObject())
            {
                if (!m_pPickup->GiveUsAPickUpObject())
                {
                    // Failed to create object - clean up and fail
                    m_pPickup->Remove();
                    m_pPickup = nullptr;
                    return false;
                }
            }

            // Store our pickup's object
            m_pObject = m_pPickup->GetObject();
            
            // Validate the object was stored correctly
            if (!m_pObject)
            {
                m_pPickup->Remove();
                m_pPickup = nullptr;
                return false;
            }

            // Create our collision sphere
            m_pCollision = new CClientColSphere(g_pClientGame->GetManager(), ElementID(INVALID_ELEMENT_ID), m_vecPosition, 1.0f);
            m_pCollision->m_pOwningPickup = this;
            m_pCollision->SetHitCallback(this);
            m_pCollision->SetCanBeDestroyedByScript(false);

            // Increment pickup counter
            ++m_pPickupManager->m_uiPickupCount;

            // Restore the attributes
            SetInterior(ucAreaCode);
            SetDimension(usDimension);

            // Reattach to an entity + any entities attached to this
            ReattachEntities();
            return true;
        }
    }
    return false;
}

void CClientPickup::Destroy()
{
    // Delete the collision sphere
    if (m_pCollision)
    {
        delete m_pCollision;
        m_pCollision = nullptr;
    }
    // Delete the pickup
    if (m_pPickup)
    {
        // Clear object reference before Remove() to prevent dangling pointer
        m_pObject = nullptr;
        m_pPickup->Remove();
        m_pPickup = nullptr;

        // Decrement pickup counter
        --m_pPickupManager->m_uiPickupCount;
    }
}

bool CClientPickup::ReCreate()
{
    // If we had a pickup, destroy and recreate it
    if (m_pPickup)
    {
        Destroy();
        return Create();
    }
    return false;
}

void CClientPickup::Callback_OnCollision(CClientColShape& Shape, CClientEntity& Entity)
{
    if (IS_PLAYER(&Entity))
    {
        bool bMatchingDimensions = (GetDimension() == Entity.GetDimension());            // Matching dimensions?

        // Call the pickup hit event (source = pickup that was hit)
        CLuaArguments Arguments;
        Arguments.PushElement(&Entity);            // The element that hit the pickup
        Arguments.PushBoolean(bMatchingDimensions);
        CallEvent("onClientPickupHit", Arguments, true);

        // Call the player pickup hit (source = player that hit the pickup)
        CLuaArguments Arguments2;
        Arguments2.PushElement(this);            // The pickup that was hit
        Arguments2.PushBoolean(bMatchingDimensions);
        Entity.CallEvent("onClientPlayerPickupHit", Arguments2, true);
    }
}

void CClientPickup::Callback_OnLeave(CClientColShape& Shape, CClientEntity& Entity)
{
    if (IS_PLAYER(&Entity))
    {
        bool bMatchingDimensions = (GetDimension() == Entity.GetDimension());            // Matching dimensions?

        // Call the pickup leave event (source = the pickup that was left)
        CLuaArguments Arguments;
        Arguments.PushElement(&Entity);            // The element that left the pickup
        Arguments.PushBoolean(bMatchingDimensions);
        CallEvent("onClientPickupLeave", Arguments, true);

        // Call the player pickup leave event (source = the player that left the pickup)
        CLuaArguments Arguments2;
        Arguments2.PushElement(this);            // The pickup that was left (this)
        Arguments2.PushBoolean(bMatchingDimensions);
        Entity.CallEvent("onClientPlayerPickupLeave", Arguments2, true);
    }
}

void CClientPickup::NotifyUnableToCreate()
{
    // Clean up any collision sphere we may have created
    if (m_pCollision)
    {
        delete m_pCollision;
        m_pCollision = nullptr;
    }
    
    // Notify the streamer we were unable to create the pickup
    CClientStreamElement::NotifyUnableToCreate();
}
