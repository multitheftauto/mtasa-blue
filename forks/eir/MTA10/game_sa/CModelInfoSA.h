/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.h
*  PURPOSE:     Header file for entity model information handler class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MODELINFO
#define __CGAMESA_MODELINFO

#include <game/CModelInfo.h>
#include <game/Common.h>

class CPedModelInfoSA;
class CPedModelInfoSAInterface;

#define     ARRAY_ModelLoaded               0x8E4CD0 // ##SA##

#define     FUNC_CStreaming__HasModelLoaded 0x4044C0

#define     ARRAY_ModelInfo                 0xA9B0C8
#define     CLASS_CText                     0xC1B340
#define     FUNC_CText_Get                  0x6A0050
#define     FUNC_GetModelFlags              0x4044E0
#define     FUNC_GetBoundingBox             0x4082F0

#define     FUNC_RemoveRef                  0x4C4BB0

#define     FUNC_RemoveModel                0x4089a0
#define     FUNC_FlushRequestList           0x40E4E0

#define     FUNC_HasVehicleUpgradeLoaded    0x407820
#define     FUNC_RequestVehicleUpgrade      0x408C70

#define     FUNC_CVehicleModelInfo__GetNumRemaps        0x4C86B0
#define     FUNC_CVehicleStructure_delete   0x4C9580

#define     FUNC_SetColModel                0x4C4BC0
#define     FUNC_AddPedModel                0x4c67a0
#define     VAR_CTempColModels_ModelPed1    0x968DF0

#include "CModelInfoSA.base.h"
#include "CModelInfoSA.ide.h"

void    ModelInfo_Init( void );
void    ModelInfo_Shutdown( void );

#include "CColModelSA.h"


/**
 * \todo Someone move GetLevelFromPosition out of here or delete it entirely please
 */

class CModelInfoSA : public CModelInfo
{
protected:
    CBaseModelInfoSAInterface *     m_pInterface;
    DWORD                           m_dwModelID;
    DWORD                           m_dwReferences;
    DWORD                           m_dwPendingInterfaceRef;
    CColModel*                      m_pCustomColModel;
    CColModelSAInterface*           m_pOriginalColModelInterface;
    RpClump*                        m_pCustomClump;
    static std::map < unsigned short, int > ms_RestreamTxdIDMap;
    static std::map < DWORD, float > ms_ModelDefaultLodDistanceMap;
    static std::map < DWORD, BYTE > ms_ModelDefaultAlphaTransparencyMap;
    bool                            m_bAddedRefForCollision;
    SVehicleSupportedUpgrades       m_ModelSupportedUpgrades;
    bool                            _isInsideRequester;
    bool                            _isInsideRemover;
public:
    static std::set < uint >        ms_ReplacedColModels;

                                    CModelInfoSA            ( void );
                                    CModelInfoSA            ( DWORD dwModelID );

    CBaseModelInfoSAInterface *     GetInterface             ( void );
    CPedModelInfoSAInterface *      GetPedModelInfoInterface ( void )              { return reinterpret_cast < CPedModelInfoSAInterface * > ( GetInterface () ); }

    DWORD                           GetModel                ( void )               { return m_dwModelID; }
    uint                            GetAnimFileIndex        ( void );

    bool                            IsValidGameInfo         ( void );

    bool                            IsPlayerModel           ( void );

    BOOL                            IsBoat                  ( void );
    BOOL                            IsCar                   ( void );
    BOOL                            IsTrain                 ( void );
    BOOL                            IsHeli                  ( void );
    BOOL                            IsPlane                 ( void );
    BOOL                            IsBike                  ( void );
    BOOL                            IsFakePlane             ( void );
    BOOL                            IsMonsterTruck          ( void );
    BOOL                            IsQuadBike              ( void );
    BOOL                            IsBmx                   ( void );
    BOOL                            IsTrailer               ( void );
    BOOL                            IsVehicle               ( void );
    BOOL                            IsUpgrade               ( void );

    char *                          GetNameIfVehicle        ( void );

    VOID                            Request                 ( EModelRequestType requestType, const char* szTag );
    VOID                            Remove                  ( void );
    BYTE                            GetLevelFromPosition    ( CVector * vecPosition );
    BOOL                            IsLoaded                ( void );
    BOOL                            DoIsLoaded              ( void );
    BYTE                            GetFlags                ( void );
    CBoundingBox *                  GetBoundingBox          ( void );
    bool                            IsValid                 ( void );
    float                           GetDistanceFromCentreOfMassToBaseOfModel ( void );
    unsigned short                  GetTextureDictionaryID  ( void );
    void                            SetTextureDictionaryID  ( unsigned short usID );
    float                           GetLODDistance          ( void );
    void                            SetLODDistance          ( float fDistance );
    static void                     StaticResetLodDistances ( void );
    void                            RestreamIPL             ( void );
    static void                     StaticFlushPendingRestreamIPL ( void );
    static void                     StaticSetHooks          ( void );

    void                            SetAlphaTransparencyEnabled ( BOOL bEnabled );
    bool                            IsAlphaTransparencyEnabled ();
    void                            ResetAlphaTransparency  ( void );
    static void                     StaticResetAlphaTransparencies ( void );

    void                            ModelAddRef             ( EModelRequestType requestType, const char* szTag );
    int                             GetRefCount             ( void );
    void                            RemoveRef               ( bool bRemoveExtraGTARef = false );

    // CVehicleModelInfo specific
    short                           GetAvailableVehicleMod  ( unsigned short usSlot );
    bool                            IsUpgradeAvailable      ( eVehicleUpgradePosn posn );
    void                            SetCustomCarPlateText   ( const char * szText );
    unsigned int                    GetNumRemaps            ( void );
    void*                           GetVehicleSuspensionData( void );
    void*                           SetVehicleSuspensionData( void* pSuspensionLines );

    // ONLY use for peds
    void                            GetVoice                ( short* psVoiceType, short* psVoice );
    void                            GetVoice                ( const char** pszVoiceType, const char** szVoice );
    void                            SetVoice                ( short sVoiceType, short sVoice );
    void                            SetVoice                ( const char* szVoiceType, const char* szVoice );

    // Custom collision related functions
    void                            SetCustomModel          ( RpClump* pClump );
    void                            RestoreOriginalModel    ( void );
    void                            MakeCustomModel         ( void );

    inline void                     SetModelID              ( DWORD dwModelID ) { m_dwModelID = dwModelID; }

    inline RwObject*                GetRwObject             ( void ) { return m_pInterface ? m_pInterface->pRwObject : NULL; }

    // CModelInfoSA methods
    void                            MakePedModel            ( char * szTexture );

    SVehicleSupportedUpgrades       GetVehicleSupportedUpgrades ( void ) { return m_ModelSupportedUpgrades; }

    void                            InitialiseSupportedUpgrades ( RpClump * pClump );
    void                            ResetSupportedUpgrades      ( void );

private:
    void                            RwSetSupportedUpgrades      ( RwFrame * parent, DWORD dwModel );
};

// Exports.
bool OnMY_CFileLoader_LoadCollisionFile_Mid ( int iModelId );

#endif