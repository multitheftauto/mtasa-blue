/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectSA.cpp
 *  PURPOSE:     Object entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CObjectSA.h"
#include "CPoolsSA.h"
#include "CRopesSA.h"
#include "CWorldSA.h"
#include "CFireManagerSA.h"
#include "CAnimManagerSA.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;

static void CObject_PreRender(CObjectSAInterface* objectInterface)
{
    SClientEntity<CObjectSA>* objectEntity = pGame->GetPools()->GetObject((DWORD*)objectInterface);
    if (objectEntity && objectEntity->pEntity)
        objectEntity->pEntity->SetPreRenderRequired(true);
}

const std::uintptr_t RETURN_CCObject_PreRender = 0x59FD56;
static void __declspec(naked) HOOK_CCObject_PreRender()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    ecx
        call    CObject_PreRender
        pop     ecx
        sub     esp, 16
        push    esi
        mov     esi, ecx
        jmp     RETURN_CCObject_PreRender
    }
    // clang-format on
}

void CObjectSA::StaticSetHooks()
{
    // Patch CObject::PreRender. We don't want the scaling code to execute
    // We'll scale the object entity matrix after onClientPedsProcessed event
    // 5E       - pop asi
    // 83 C4 10 - add esp, 0x10
    // C3       - ret
    std::uint8_t bytes[5] = {0x5E, 0x83, 0xC4, 0x10, 0xC3};
    MemCpy((void*)0x59FE0E, bytes, sizeof(bytes));
    HookInstall(0x59FD50, HOOK_CCObject_PreRender);
}

// GTA uses this to pass to CFileLoader::LoadObjectInstance the info it wants to load
struct CFileObjectInstance
{
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
    float rr;            // = 1
    DWORD modelId;
    DWORD areaNumber;
    long  flags;            // = -1
};

CObjectSA::CObjectSA(CObjectSAInterface* objectInterface)
{
    SetInterface(objectInterface);
    m_ucAlpha = 255;

    // Setup some flags
    BeingDeleted = false;
    DoNotRemoveFromGame = false;

    if (m_pInterface)
    {
        ResetScale();
        CheckForGangTag();
        m_pInterface->bStreamingDontDelete = true;
    }
}

CObjectSA::CObjectSA(DWORD dwModel, bool bBreakingDisabled)
{
    DWORD CObjectCreate = FUNC_CObject_Create;
    DWORD dwObjectPtr = 0;
    // clang-format off
    __asm
    {
        push    1
        push    dwModel
        call    CObjectCreate
        add     esp, 8
        mov     dwObjectPtr, eax
    }
    // clang-format on

    if (dwObjectPtr)
    {
        SetInterface((CEntitySAInterface*)dwObjectPtr);

        CWorldSA* world = (CWorldSA*)pGame->GetWorld();
        world->Add(m_pInterface, CObject_Constructor);

        // Setup some flags
        BeingDeleted = false;
        DoNotRemoveFromGame = false;
        MemPutFast<BYTE>(dwObjectPtr + 316, 6);
        if (bBreakingDisabled)
        {
            // Set our immunities
            // Sum of all flags checked @ CPhysical__CanPhysicalBeDamaged
            CObjectSAInterface* pObjectSAInterface = GetObjectInterface();
            pObjectSAInterface->bBulletProof = true;
            pObjectSAInterface->bFireProof = true;
            pObjectSAInterface->bCollisionProof = true;
            pObjectSAInterface->bMeeleProof = true;
            pObjectSAInterface->bExplosionProof = true;
        }
        m_pInterface->bStreamingDontDelete = true;
    }
    else
    {
        // The exception handler doesn't work for some reason, so do this
        SetInterface(nullptr);
    }

    m_ucAlpha = 255;

    if (m_pInterface)
    {
        ResetScale();
        CheckForGangTag();
    }
}

CObjectSA::~CObjectSA()
{
    if (!BeingDeleted && DoNotRemoveFromGame == false)
    {
        CEntitySAInterface* pInterface = GetInterface();
        if (pInterface)
        {
            pGame->GetRopes()->RemoveEntityRope(pInterface);

            if (!pInterface->IsPlaceableVTBL())
            {
                CWorldSA* world = (CWorldSA*)pGame->GetWorld();
                world->Remove(pInterface, CObject_Destructor);

                pInterface->Destructor(true);
            }
        }

        BeingDeleted = true;
        ((CPoolsSA*)pGame->GetPools())->RemoveObject((CObject*)(CObjectSA*)this);
    }
}

void CObjectSA::Explode()
{
    DWORD dwFunc = FUNC_CObject_Explode;
    DWORD dwThis = (DWORD)GetInterface();

    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on
}

void CObjectSA::Break()
{
    DWORD dwFunc = 0x5A0D90;
    DWORD dwThis = (DWORD)GetInterface();

    float fHitVelocity = 1000.0f;            // has no direct influence, but should be high enough to trigger the break (effect)

    // clang-format off
    __asm
    {
        push    32h // most cases: between 30 and 37
        push    0 // colliding entity. To ignore it, we can set it to 0
        push    0B73710h // vecCollisionImpactVelocity
        push    0 // vecCollisionLastPos
        push    fHitVelocity
        mov     ecx, dwThis
        call    dwFunc
    }
    // clang-format on

    if (IsGlass())
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fZ = 0.0f;
        dwFunc = FUNC_CGlass_WindowRespondsToCollision;

        // clang-format off
        __asm
        {
            push 0
            push fZ
            push fY
            push fX
            push 0
            push 0
            push 0
            push fHitVelocity
            push dwThis
            call dwFunc
            add esp, 24h
        }
        // clang-format on
    }
}

void CObjectSA::SetHealth(float fHealth)
{
    static_cast<CObjectSAInterface*>(GetInterface())->fHealth = fHealth;
}

float CObjectSA::GetHealth()
{
    return static_cast<CObjectSAInterface*>(GetInterface())->fHealth;
}

void CObjectSA::SetModelIndex(unsigned long ulModel)
{
    // Delete any existing RwObject first
    GetInterface()->DeleteRwObject();

    // Jax: I'm not sure if using the virtual method is right (as ped and vehicle dont), but it works
    GetInterface()->SetModelIndex(ulModel);

    CheckForGangTag();
}

void CObjectSA::CheckForGangTag()
{
    switch (GetModelIndex())
    {
        case 1524:
        case 1525:
        case 1526:
        case 1527:
        case 1528:
        case 1529:
        case 1530:
        case 1531:
            m_bIsAGangTag = true;
            break;
        default:
            m_bIsAGangTag = false;
            break;
    }
}

bool CObjectSA::IsGlass()
{
    DWORD dwFunc = 0x46A760;
    DWORD dwThis = (DWORD)GetInterface();
    bool  bResult;

    // clang-format off
    __asm
    {
        push dwThis
        call dwFunc
        mov bResult, al
        add esp, 4
    }
    // clang-format on
    return bResult;
}

void CObjectSA::SetScale(float fX, float fY, float fZ)
{
    m_vecScale = CVector(fX, fY, fZ);
    GetObjectInterface()->bUpdateScale = true;
    GetObjectInterface()->fScale = std::max(fX, std::max(fY, fZ));
}

CVector* CObjectSA::GetScale()
{
    return &m_vecScale;
}

void CObjectSA::ResetScale()
{
    SetScale(1.0f, 1.0f, 1.0f);
}

bool CObjectSA::SetOnFire(bool onFire)
{
    CObjectSAInterface* objectInterface = GetObjectInterface();
    if (onFire == !!objectInterface->pFire)
        return false;

    auto* fireManager = static_cast<CFireManagerSA*>(pGame->GetFireManager());

    if (onFire)
    {
        CFire* fire = fireManager->StartFire(this, nullptr, static_cast<float>(DEFAULT_FIRE_PARTICLE_SIZE));
        if (!fire)
            return false;

        fire->SetTarget(this);
        fire->SetStrength(1.0f);
        fire->Ignite();
        fire->SetNumGenerationsAllowed(0);

        objectInterface->pFire = fire->GetInterface();
    }
    else
    {
        CFire* fire = fireManager->GetFire(objectInterface->pFire);
        if (!fire)
            return false;

        fire->Extinguish();
    }

    return true;
}

void CObjectSA::SetAnimation(CAnimBlendHierarchySAInterface* animation, eAnimationFlags flags)
{
    if (!m_pInterface || !m_pInterface->m_pRwObject)
        return;

    RpClump* clump = GetRpClump();

    if (!RpAnimBlendClumpIsInitialized(clump) && animation)
        RpAnimBlendClumpInit(clump);

    if (animation)
        pGame->GetAnimManager()->BlendAnimation(clump, animation, flags, 1.0f);
    else
    {
        for (auto assoc = pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation(clump); assoc; assoc = pGame->GetAnimManager()->RpAnimBlendGetNextAssociation(assoc))
        {
            // Disable playing flag
            short flags = assoc->GetFlags();
            flags &= ~ANIMATION_IS_PLAYING;

            assoc->SetFlags(flags);
        }
    }
}

bool CObjectSA::SetAnimationSpeed(float speed)
{
    if (!m_pInterface || !m_pInterface->m_pRwObject)
        return false;

    auto assoc = pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation(GetRpClump());
    if (!assoc)
        return false;

    assoc->SetCurrentSpeed(speed);
    return true;
}
