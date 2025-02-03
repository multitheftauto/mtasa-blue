/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientCheckpoint.cpp
 *  PURPOSE:     Checkpoint marker entity class
 *
 *****************************************************************************/

#include <StdInc.h>
#include <game/CCheckpoint.h>
#include <game/CCheckpoints.h>

CClientCheckpoint::CClientCheckpoint(CClientMarker* pThis)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientCheckpoint"), &m_pThis, NULL);

    // Init
    m_pThis = pThis;
    m_pCheckpoint = NULL;
    m_bStreamedIn = false;
    m_bVisible = true;
    m_uiIcon = CClientCheckpoint::ICON_NONE;
    m_Color = SColorRGBA(255, 0, 0, 128);
    m_fSize = 4.0f;
    m_dwType = CHECKPOINT_EMPTYTUBE;
    m_vecDirection.fX = 1.0f;
    m_bHasTarget = false;
    m_ignoreAlphaLimits = false;
    m_TargetArrowColor = SColorRGBA(255, 64, 64, 255);
    m_TargetArrowSize = m_fSize * 0.625f;
}

CClientCheckpoint::~CClientCheckpoint()
{
    // Eventually destroy the checkpoint
    Destroy();
    CClientEntityRefManager::RemoveEntityRefs(0, &m_pThis, NULL);
}

unsigned long CClientCheckpoint::GetCheckpointType() const
{
    switch (m_dwType)
    {
        case CHECKPOINT_TUBE:
        case CHECKPOINT_EMPTYTUBE:
        case CHECKPOINT_ENDTUBE:
            return CClientCheckpoint::TYPE_NORMAL;

        case CHECKPOINT_TORUS:
            return CClientCheckpoint::TYPE_RING;

        default:
            return CClientCheckpoint::TYPE_INVALID;
    }
}

void CClientCheckpoint::SetCheckpointType(unsigned long ulType)
{
    // Set the type
    unsigned long ulNewType;
    switch (ulType)
    {
        case CClientCheckpoint::TYPE_NORMAL:
        {
            // Set the correct type depending on the icon we want inside
            if (m_uiIcon == CClientCheckpoint::ICON_ARROW)
                ulNewType = CHECKPOINT_TUBE;
            else if (m_uiIcon == CClientCheckpoint::ICON_FINISH)
                ulNewType = CHECKPOINT_ENDTUBE;
            else
                ulNewType = CHECKPOINT_EMPTYTUBE;

            break;
        }

        case CClientCheckpoint::TYPE_RING:
        {
            ulNewType = CHECKPOINT_TORUS;
            break;
        }

        default:
        {
            // Set the correct type depending on the icon we want inside
            if (m_uiIcon == CClientCheckpoint::ICON_ARROW)
                ulNewType = CHECKPOINT_TUBE;
            else if (m_uiIcon == CClientCheckpoint::ICON_FINISH)
                ulNewType = CHECKPOINT_ENDTUBE;
            else
                ulNewType = CHECKPOINT_EMPTYTUBE;

            break;
        }
    }

    // Different from our current type
    if (m_dwType != ulNewType)
    {
        // Set it
        m_dwType = ulNewType;

        // Recreate it
        ReCreate();
    }
}

bool CClientCheckpoint::IsHit(const CVector& vecPosition) const
{
    // Grab the type and do a 2D or 3D distance check depending on what type it is
    unsigned long ulType = GetCheckpointType();
    if (ulType == CClientCheckpoint::TYPE_NORMAL)
    {
        return IsPointNearPoint2D(m_Matrix.vPos, vecPosition, m_fSize + 4);
    }
    else
    {
        return IsPointNearPoint3D(m_Matrix.vPos, vecPosition, m_fSize + 4);
    }
}

void CClientCheckpoint::SetPosition(const CVector& vecPosition)
{
    // Different from our current position?
    if (m_Matrix.vPos != vecPosition)
    {
        // Recalculate the stored target so that its accurate even if we move it
        if (m_vecTargetPosition.fX != 0 && m_vecTargetPosition.fY != 0 && m_vecTargetPosition.fZ != 0)
        {
            m_vecDirection = m_vecTargetPosition - vecPosition;
            m_vecDirection.Normalize();
        }

        // Store the new position, recreate and tell the streamer about the new position
        m_Matrix.vPos = vecPosition;
        ReCreateWithSameIdentifier();
    }
}

void CClientCheckpoint::SetDirection(const CVector& vecDirection)
{
    // Different target position than already?
    if (m_vecDirection != vecDirection)
    {
        // Set the direction and reset the target position
        m_vecTargetPosition = CVector(0.0f, 0.0f, 0.0f);
        m_vecDirection = vecDirection;

        // Recreate
        if (GetCheckpointType() == CClientCheckpoint::TYPE_RING || m_uiIcon == CClientCheckpoint::ICON_ARROW)
        {
            ReCreate();
        }
    }
}

void CClientCheckpoint::GetMatrix(CMatrix& mat)
{
    mat = m_Matrix;
}

void CClientCheckpoint::SetMatrix(CMatrix& mat)
{
    SetPosition(mat.vPos);
    m_Matrix = mat;
}

void CClientCheckpoint::SetNextPosition(const CVector& vecPosition)
{
    // Different target position than already?
    if (m_vecTargetPosition != vecPosition)
    {
        // Set the new target position and direction
        m_vecTargetPosition = vecPosition;
        m_vecDirection = m_vecTargetPosition - m_Matrix.vPos;
        m_vecDirection.Normalize();

        // Recreate
        if (GetCheckpointType() == CClientCheckpoint::TYPE_RING || m_uiIcon == CClientCheckpoint::ICON_ARROW)
        {
            ReCreate();
        }
    }
}

void CClientCheckpoint::SetVisible(bool bVisible)
{
    // If we're streamed in, create/destroy as we're getting visible/invisible
    if (IsStreamedIn())
    {
        if (!bVisible && m_bVisible)
        {
            Destroy();
        }
        else if (bVisible && !m_bVisible)
        {
            Create();
        }
    }

    // Set the new visible state
    m_bVisible = bVisible;
}

void CClientCheckpoint::SetIcon(unsigned int uiIcon)
{
    // Different from our current icon?
    if (m_uiIcon != uiIcon)
    {
        // Set the new icon
        m_uiIcon = uiIcon;

        // Normal checkpoint?
        if (GetCheckpointType() == CClientCheckpoint::TYPE_NORMAL)
        {
            // Update the type
            unsigned long ulNewType;
            switch (uiIcon)
            {
                case CClientCheckpoint::ICON_NONE:
                    ulNewType = CHECKPOINT_EMPTYTUBE;
                    break;

                case CClientCheckpoint::ICON_ARROW:
                    ulNewType = CHECKPOINT_TUBE;
                    break;

                case CClientCheckpoint::ICON_FINISH:
                    ulNewType = CHECKPOINT_ENDTUBE;
                    break;

                default:
                    ulNewType = CHECKPOINT_EMPTYTUBE;
                    break;
            }

            // Different from our previous type?
            if (ulNewType != m_dwType)
            {
                // Set the new type and recreate the checkpoint
                m_dwType = ulNewType;
                ReCreate();
            }
        }
    }
}

void CClientCheckpoint::SetColor(const SColor& color)
{
    // Different from our current color?
    if (m_Color != color)
    {
        // Set it
        m_Color = color;

        // Default alpha
        if (!m_ignoreAlphaLimits && GetCheckpointType() == CClientCheckpoint::TYPE_NORMAL)
            m_Color.A = 128;

        // Recreate
        ReCreate();
    }
}

void CClientCheckpoint::SetSize(float fSize)
{
    // Different from our current size
    if (m_fSize != fSize)
    {
        // Set the new size and recreate
        m_fSize = fSize;
        m_TargetArrowSize = fSize * 0.625f;

        ReCreate();
    }
}

unsigned char CClientCheckpoint::StringToIcon(const char* szString)
{
    if (stricmp(szString, "none") == 0)
    {
        return ICON_NONE;
    }
    else if (stricmp(szString, "arrow") == 0)
    {
        return ICON_ARROW;
    }
    else if (stricmp(szString, "finish") == 0)
    {
        return ICON_FINISH;
    }
    else
    {
        return ICON_INVALID;
    }
}

bool CClientCheckpoint::IconToString(unsigned char ucIcon, SString& strOutString)
{
    switch (ucIcon)
    {
        case ICON_NONE:
            strOutString = "none";
            return true;

        case ICON_ARROW:
            strOutString = "arrow";
            return true;

        case ICON_FINISH:
            strOutString = "finish";
            return true;

        default:
            strOutString = "invalid";
            return false;
    }
}

void CClientCheckpoint::StreamIn()
{
    // We're now streamed in
    m_bStreamedIn = true;

    // Create us if we're visible
    if (m_bVisible)
    {
        Create();
    }
}

void CClientCheckpoint::StreamOut()
{
    // Destroy our checkpoint
    Destroy();

    // No longer streamed in
    m_bStreamedIn = false;
}

void CClientCheckpoint::Create(unsigned long ulIdentifier)
{
    // If the item already exists, don't create it
    if (!m_pCheckpoint)
    {
        // Generate an identifier (TODO: Move identifier stuff to game layer)
        static unsigned long s_ulIdentifier = 128;
        if (ulIdentifier == 0)
        {
            s_ulIdentifier++;
            m_dwIdentifier = s_ulIdentifier;
        }
        else
        {
            m_dwIdentifier = ulIdentifier;
        }

        // Create it
        m_pCheckpoint =
            g_pGame->GetCheckpoints()->CreateCheckpoint(m_dwIdentifier, static_cast<WORD>(m_dwType), &m_Matrix.vPos, &m_vecDirection, m_fSize, 0.2f, m_Color);
        if (m_pCheckpoint)
        {
            // Set properties
            m_pCheckpoint->SetRotateRate(0);
            ApplyCheckpointTargetArrowProperties();
        }
    }
}

void CClientCheckpoint::Destroy()
{
    // Destroy it and NULL it
    if (m_pCheckpoint)
    {
        m_pCheckpoint->Remove();
        m_pCheckpoint = NULL;
    }
}

void CClientCheckpoint::ReCreate()
{
    // Recreate if we're streamed in and visible
    if (IsStreamedIn() && m_bVisible)
    {
        Destroy();
        Create();
    }
}

void CClientCheckpoint::ReCreateWithSameIdentifier()
{
    // Recreate if we're streamed in and visible
    if (IsStreamedIn() && m_bVisible)
    {
        Destroy();
        Create(m_dwIdentifier);
    }
}

void CClientCheckpoint::SetTargetArrowProperties(const SColor& arrowColor, float size) noexcept
{
    if (m_TargetArrowColor == arrowColor && m_TargetArrowSize == size)
        return;

    m_TargetArrowColor = arrowColor;
    m_TargetArrowSize = size;

    ApplyCheckpointTargetArrowProperties();
}

void CClientCheckpoint::ApplyCheckpointTargetArrowProperties() noexcept
{
    if (m_pCheckpoint && m_uiIcon == CClientCheckpoint::ICON_ARROW)
        m_pCheckpoint->SetTargetArrowData(m_TargetArrowColor, m_TargetArrowSize);
}
