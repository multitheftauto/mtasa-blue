/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_ClothesCache.cpp.cpp
*  PORPOISE:    
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define RWFUNC_IMPLEMENT
#include "..\game_sa\gamesa_renderware.h"
#include "..\game_sa\gamesa_renderware.hpp"

#define CLOTHES_REF_TEST    1       // Debug clothes geometry refs

////////////////////////////////////////////////
//
// class CPedClothesDesc
//
// Clothes info for checking changes
//
////////////////////////////////////////////////
class CPedClothesDesc
{
public:
    union
    {
        DWORD things1[10];      // models
        struct {
            DWORD torso;
            DWORD head;
            DWORD hands;
            DWORD legs;
            DWORD feet;
            DWORD necklace;
            DWORD watch;
            DWORD glasses;
            DWORD hat;
        };
    };

    union
    {
        DWORD things2[18];      // +0x28 textures?
        struct {
            DWORD Torso;
            DWORD Hair;
            DWORD Legs;
            DWORD Shoes;
            DWORD Left_upper_arm;
            DWORD Left_lower_arm;
            DWORD Right_upper_arm;
            DWORD Right_lower_arm;
            DWORD Back_top;
            DWORD Left_chest;
            DWORD Right_chest;
            DWORD Stomach;
            DWORD Lower_back;
            DWORD Extra1;
            DWORD Extra2;
            DWORD Extra3;
            DWORD Extra4;
            DWORD Suit;
        };
    };

    float fFatAmount;           // +0x70
    float fMuscleAmount;        // +0x74

    bool operator==( const CPedClothesDesc& other ) const
    {
        if ( memcmp ( &things1, &other.things1, sizeof ( things1 ) ) == 0 )
        if ( memcmp ( &things2, &other.things2, sizeof ( things2 ) ) == 0 )
        {
            // Make fat 0,100,200,300...1000
            int iFatHere = Round ( fFatAmount / 100.f );
            int iFatThere = Round ( other.fFatAmount / 100.f );

            // Make muscle 0,250,500,750,1000
            int iMuscleHere = Round ( fMuscleAmount / 250.f );
            int iMuscleThere = Round ( other.fMuscleAmount / 250.f );

            if ( iFatHere == iFatThere && iMuscleHere == iMuscleThere )
                return true;
        }
        return false;
    }
};


////////////////////////////////////////////////
//
// class CClumpStore
//
// Save clothes clumps for later
//
////////////////////////////////////////////////
class CClumpStore
{
public:

    struct SSavedClumpInfo
    {
        RpClump* pClump;
        CPedClothesDesc clothedDesc;
        bool bUnused;
        int iCacheRevision;
        CTickCount timeUnused;
    };

    std::vector < SSavedClumpInfo > savedClumpList;
    uint                m_uiMaxSize;
    uint                m_uiMinCacheTime;
    SClothesCacheStats  m_Stats;
    int                 m_iCacheRevision;

    ///////////////////////////////////////
    //
    // Constructor
    //
    ///////////////////////////////////////
    CClumpStore ( void )
    {
        memset ( &m_Stats, 0, sizeof ( m_Stats ) );
        m_uiMaxSize = 4;
        m_uiMinCacheTime = 1000;
        m_iCacheRevision = 1;
    }


    //////////////////////////////////////////////////////////////
    //
    // Check if any clumps have become unused outside of the cache
    //
    //////////////////////////////////////////////////////////////
    uint GetNumCached ( void )
    {
        uint uiNumCached = 0;
        for ( std::vector < SSavedClumpInfo >::iterator iter = savedClumpList.begin () ; iter != savedClumpList.end () ; ++iter )
        {
            SSavedClumpInfo& info = *iter;
            RpGeometry* pGeometry = ( ( RpAtomic* ) ( ( info.pClump->atomics.root.next ) - 0x8 ) )->geometry;
#ifdef CLOTHES_REF_TEST
            if ( pGeometry->refs < 21 )
            {
                AddReportLog( 7521, SString ( "Clothes geometry refs below expected value: %d", pGeometry->refs ) );
                pGeometry->refs = 21;
            }
            if ( pGeometry->refs == 21 )
#else
            if ( pGeometry->refs == 1 )
#endif
            {
                uiNumCached++;
                if ( !info.bUnused )
                {
                    info.timeUnused = CTickCount::Now ();
                    info.bUnused = true;
                }
            }
            else
                info.bUnused = false;
        }

        m_Stats.uiNumTotal = savedClumpList.size ();
        m_Stats.uiNumUnused = uiNumCached;
        return uiNumCached;
    }


    ///////////////////////////////////////
    //
    // Save a ref to this clump
    //
    ///////////////////////////////////////
    void SaveClump ( RpClump* pClump, CPedClothesDesc* pClothesDesc )
    {
        UpdateCacheSize ();

        // Remove up to 2 unused if over limit
        if ( GetNumCached () >= m_uiMaxSize )
            if ( RemoveOldestUnused () )
                if ( GetNumCached () >= m_uiMaxSize )
                    RemoveOldestUnused ();

        RpClump * pClumpCopy = RpClumpClone ( pClump );

        SSavedClumpInfo info;
        info.pClump = pClumpCopy;
#ifdef CLOTHES_REF_TEST
        RpGeometry* pGeometry = ( ( RpAtomic* ) ( ( info.pClump->atomics.root.next ) - 0x8 ) )->geometry;
        pGeometry->refs += 20;
#endif

        info.clothedDesc = *pClothesDesc;
        info.bUnused = false;
        info.iCacheRevision = m_iCacheRevision;
        savedClumpList.push_back ( info );
        m_Stats.uiNumTotal++;
    }


    ///////////////////////////////////////
    //
    // Adjust cache numbers depending on amount of streaming memory allocated
    //
    //  64MB streaming = 4 MB for clothes
    //  96MB streaming = 8 MB for clothes
    //  128MB streaming = 16 MB for clothes
    //  256MB streaming = 32 MB for clothes
    //
    ///////////////////////////////////////
    void UpdateCacheSize ( void )
    {
        int iStreamingMemoryAvailableKB             = *(int*)0x08A5A80;

        SSamplePoint < float > cloPoints[] = { {65536, 4},  {98304, 8},    {131072, 16},   {262144, 32} };

        m_uiMaxSize = (uint)EvalSamplePosition < float > ( cloPoints, NUMELMS ( cloPoints ), (float)iStreamingMemoryAvailableKB );
    }


    ///////////////////////////////////////
    //
    // Remove clothes that have been unused the longest
    //
    ///////////////////////////////////////
    bool RemoveOldestUnused ( void )
    {
        uint uiBestAge = -1;
        std::vector < SSavedClumpInfo >::iterator uiBestIndex;

        CTickCount timeNow = CTickCount::Now ();
        for ( std::vector < SSavedClumpInfo >::iterator iter = savedClumpList.begin () ; iter != savedClumpList.end () ; ++iter )
        {
            const SSavedClumpInfo& info = *iter;
            if ( info.bUnused )
            {
                uint uiAge = ( timeNow - info.timeUnused ).ToInt ();
                if ( uiAge > m_uiMinCacheTime )
                {
                    if ( uiAge > uiBestAge || uiBestAge == -1 )
                    {
                        uiBestAge = uiAge;
                        uiBestIndex = iter;
                    }
                }
            }
        }

        if ( uiBestAge == -1 )
            return false;

        const SSavedClumpInfo& info = *uiBestIndex;

#ifdef CLOTHES_REF_TEST
        RpGeometry* pGeometry = ( ( RpAtomic* ) ( ( info.pClump->atomics.root.next ) - 0x8 ) )->geometry;
        pGeometry->refs -= 20;
#endif
        RpClumpDestroy ( info.pClump );
        assert ( info.bUnused );
        m_Stats.uiNumTotal--;
        m_Stats.uiNumUnused--;
        m_Stats.uiNumRemoved++;
        savedClumpList.erase ( uiBestIndex );
        return true;
    }


    ///////////////////////////////////////
    //
    // Find a clump which can be used for the provided clothes desc.
    // Returns NULL if can't be done
    //
    ///////////////////////////////////////
    RpClump* FindMatchAndUse ( CPedClothesDesc* pClothesDesc )
    {
        for ( std::vector < SSavedClumpInfo >::iterator iter = savedClumpList.begin () ; iter != savedClumpList.end () ; ++iter )
        {
            SSavedClumpInfo& info = *iter;
            if ( info.iCacheRevision != m_iCacheRevision )
                continue;   // Don't match if it was generated with different custom clothes textures

            if ( info.clothedDesc == *pClothesDesc )
            {
                if ( info.bUnused )
                {
                    info.bUnused = false;
                    m_Stats.uiNumUnused--;
                }

                m_Stats.uiCacheHit++;
                RpClump * pCopy = RpClumpClone ( info.pClump );
                return pCopy;
            }
        }
        m_Stats.uiCacheMiss++;
        return NULL;
    }
};

CClumpStore ms_clumpStore;


////////////////////////////////////////////////
//
// CMultiplayerSA::FlushClothesCache
//
// Stop using old cached clumps
//
////////////////////////////////////////////////
void CMultiplayerSA::FlushClothesCache( void )
{
    ms_clumpStore.m_iCacheRevision++;
}


////////////////////////////////////////////////
//
// Hook CClothesBuilder::CreateSkinnedClump
//
//
////////////////////////////////////////////////
RpClump* _cdecl OnCClothesBuilderCreateSkinnedClumpPre ( RpClump* pRpClump, RwTexDictionary* pRwTexDictionary, CPedClothesDesc* pNewClothesDesc, CPedClothesDesc* pPrevClothesDesc, bool bAlwaysFalse )
{
    // If torso, head, hands, legs or feet are NULL, set to the default value
    DWORD defaults [] = { 0x2ff481ca, 0x6e99e4d7, 0x6e850eb7, 0x347251c1, 0xf79d4684 };

    for ( uint i = 0 ; i < NUMELMS ( defaults ) ; i++ )
        if ( !pNewClothesDesc->things1[i] )
            pNewClothesDesc->things1[i] = defaults[i];

    return ms_clumpStore.FindMatchAndUse ( pNewClothesDesc );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCClothesBuilderCreateSkinnedClumpPost ( RpClump* pRpClumpResult, RpClump* pRpClump, RwTexDictionary* pRwTexDictionary, CPedClothesDesc* pNewClothesDesc, CPedClothesDesc* pPrevClothesDesc, bool bAlwaysFalse )
{
    if ( pRpClumpResult )
        ms_clumpStore.SaveClump ( pRpClumpResult, pNewClothesDesc );
}


// Hook info
#define HOOKPOS_CClothesBuilderCreateSkinnedClump        0x5A69D0
#define HOOKSIZE_CClothesBuilderCreateSkinnedClump       6
DWORD RETURN_CClothesBuilderCreateSkinnedClump =         0x5A69D6;
void _declspec(naked) HOOK_CClothesBuilderCreateSkinnedClump()
{
    _asm
    {
        pushad
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        call    OnCClothesBuilderCreateSkinnedClumpPre
        mov     [esp+0],eax
        add     esp, 4*5
        popad

        mov     eax,[esp-32-4*5]
        cmp     eax, 0
        jnz     done        // Use our supplied clump ?

        push    [esp+0+4*5]
        push    [esp+0+4*5]
        push    [esp+0+4*5]
        push    [esp+0+4*5]
        push    [esp+0+4*5]
        call    inside
        add     esp, 4*5

        pushad
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    [esp+32+4*5]
        push    eax
        call    OnCClothesBuilderCreateSkinnedClumpPost
        add     esp, 4*5+4
        popad
done:
        retn

inside:
        // Original code
        sub     esp, 0D4h 
        jmp     RETURN_CClothesBuilderCreateSkinnedClump
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::GetClothesCacheStats
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::GetClothesCacheStats ( SClothesCacheStats& outStats )
{
    ms_clumpStore.GetNumCached ();
    outStats = ms_clumpStore.m_Stats;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_ClothesCache
//
// Setup hooks for ClothesCache
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_ClothesCache ( void )
{
    EZHookInstall ( CClothesBuilderCreateSkinnedClump );
    InitRwFunctions( pGameInterface->GetGameVersion () );
}
