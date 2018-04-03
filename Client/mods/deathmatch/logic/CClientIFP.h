#pragma once

#ifndef __CCLIENTIFP_H
#define __CCLIENTIFP_H

#include "CClientEntity.h"
#include "CFileReader.h"
#include "CIFPAnimations.h"
#include <algorithm>

class CClientIFP: public CClientEntity, CFileReader
{
public:
    typedef CIFPAnimations::SAnimation SAnimation;

    struct SBase 
    {
        char     FourCC[4];
        uint32_t Size;
    };

    struct SInfo
    {
        SBase    Base;
        int32_t  Entries;
        char     Name[24];
    };

    struct SAnpk
    {
        SBase Base;
        SInfo Info;
    };

    struct SName
    {
        SBase Base;
    };

    struct SDgan 
    {
        SBase Base;
        SInfo Info;
    };

    struct SCpan 
    {
        SBase Base;
    };

    struct SAnim 
    {
        SBase    Base;
        char     Name[28];
        int32_t  Frames;
        int32_t  Unk;
        int32_t  Next;
        int32_t  Previous;

        // According to https://www.gtamodding.com/wiki/IFP, Unk2 should not exist, but for some reason, it's there
        // I don't know why. Let's just go with the flow and ignore it. The value seems to be always zero. 
        int32_t  Unk2;
    };

    struct SKfrm
    {
        SBase Base;
    };

    struct SQuaternion
    {
        float X, Y, Z, W;
    };

    struct SVector
    {
        float X, Y, Z;
    };

    struct SKr00
    {
        SQuaternion Rotation;
        float       Time;
    };

    struct SKrt0
    {
        SQuaternion Rotation;
        SVector     Translation;
        float       Time;
    };

    struct SKrts
    {
        SQuaternion Rotation;
        SVector     Translation;
        SVector     Scale;
        float       Time;
    };

    struct SCompressedQuaternion
    {
        int16_t X, Y, Z, W;
    };

    struct SCompressedCVector
    {
        int16_t X, Y, Z;
    };


    struct SCompressed_KR00
    {
        SCompressedQuaternion Rotation;
        int16_t               Time;
    };

    struct SCompressed_KRT0 : SCompressed_KR00
    {
        SCompressedCVector Translation;
    };


    enum eFrameType
    {
        UNKNOWN_FRAME   = -1,
        KR00            = 0,
        KRT0            = 1,
        KRTS            = 2,
        KR00_COMPRESSED = 3,
        KRT0_COMPRESSED = 4,
    };

    struct SIFPHeaderV2
    {
        int32_t   OffsetEOF;
        char      InternalFilSName[24];
        int32_t   TotalAnimations;
    };

    struct SAnimationHeaderV2
    {
        // 24 + 4 + 4 + 4 = 36 bytes

        char      Name[24];
        int32_t   TotalObjects;
        int32_t   FrameSize;
        int32_t   isCompressed; // The value is always 1
    };


    struct SSequenceHeaderV2
    {
        // 24 + 4 + 4 + 4 = 36 bytes

        char      Name[24];
        int32_t   FrameType;
        int32_t   TotalFrames;
        int32_t   BoneID;
    };

    enum eBoneType 
    {
        UNKNOWN     = -1,
        NORMAL      = 0, // Normal or Root, both are same
        PELVIS      = 1,
        SPINE       = 2,
        SPINE1      = 3,
        NECK        = 4,
        HEAD        = 5,
        JAW         = 8,
        L_BROW      = 6,
        R_BROW      = 7,
        L_CLAVICLE  = 31,
        L_UPPER_ARM = 32,
        L_FORE_ARM  = 33,
        L_HAND      = 34,
        L_FINGER    = 35,
        L_FINGER_01 = 36,
        R_CLAVICLE  = 21,
        R_UPPER_ARM = 22,
        R_FORE_ARM  = 23,
        R_HAND      = 24,
        R_FINGER    = 25,
        R_FINGER_01 = 26,
        L_BREAST    = 302,
        R_BREAST    = 301,
        BELLY       = 201,
        L_THIGH     = 41,
        L_CALF      = 42,
        L_FOOT      = 43,
        L_TOE_0     = 44,
        R_THIGH     = 51,
        R_CALF      = 52,
        R_FOOT      = 53,
        R_TOE_0     = 54    
    };
                                    CClientIFP                   ( class CClientManager* pManager, ElementID ID );
                                    ~CClientIFP                  ( void );

    virtual eClientEntityType       GetType                      ( void ) const              { return CCLIENTIFP; }

    void                            MarkAsUnloading              ( void ) { m_bUnloading = true; }
    bool                            IsUnloading                  ( void ) { return m_bUnloading; }

    bool                            LoadIFP                      ( const char* szFilePath, const SString & strBlockName );
    bool                            LoadIFPFile                  ( const char * szFilePath );
    void                            ReadIFPVersion1              ( void );
    void                            ReadIFPVersion2              ( bool bAnp3 );

    WORD                            ReadSequencesWithDummies     ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );
    WORD                            ReadSequences                ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    WORD                            ReadSequencesVersion1        ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    WORD                            ReadSequencesVersion2        ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    int32_t                         ReadSequenceVersion1         ( SAnim & Anim );
    void                            ReadSequenceVersion2         ( SSequenceHeaderV2 & ObjectNode );

    void                            ReadHeaderVersion1           ( SInfo & Info );
    void                            ReadAnimationNameVersion1    ( SAnimation & IfpAnimation );
    void                            ReadDgan                     ( SDgan & Dgan );
    CClientIFP::eFrameType          ReadKfrm                     ( void );
    void                            ReadAnimationHeaderVersion2  ( SAnimationHeaderV2 & AnimationNode, bool bAnp3 );

    bool                            ReadSequenceKeyFrames        ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, eFrameType iFrameType, int32_t iFrames );
    void                            ReadKeyFramesAsCompressed    ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, eFrameType iFrameType, int32_t iFrames );
    void                            ReadKrtsFramesAsCompressed   ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames );
    void                            ReadKrt0FramesAsCompressed   ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames );
    void                            ReadKr00FramesAsCompressed   ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames );

    template < class T > 
    void                            ReadCompressedFrames         ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t iFrames )
    {
        BYTE * pKeyFrames = pAnimationSequence->GetKeyFrames ( );
        size_t iSizeInBytes = sizeof ( T ) * iFrames;
        ReadBytes ( pKeyFrames, iSizeInBytes );
    }  

    void                            InitializeAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, const char * szAnimationName, const int32_t iSequences );
    void                            InitializeAnimationSequence  ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, const char * szName, const int32_t iBoneID );
    void                            PreProcessAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );    
    void                            CopySequencesWithDummies     ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & mapOfSequences );
    BYTE *                          AllocateSequencesMemory      ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );

    void                            InsertAnimationDummySequence ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, std::string & BoneName, DWORD & dwBoneID );
    void                            CopyDummyKeyFrameByBoneID    ( BYTE * pKeyFrames, DWORD dwBoneID );
    std::string                     ConvertStringToMapKey        ( const char * szString );

    constexpr void                  RoundSize                    ( uint32_t & u32Size );
    constexpr bool                  IsKeyFramesTypeRoot          ( eFrameType iFrameType );

    eFrameType                      GetFrameTypeFromFourCC       ( const char * szFourCC );
    size_t                          GetSizeOfCompressedFrame     ( eFrameType FrameType );
    int32_t                         GetBoneIDFromName            ( std::string const& BoneName );
    std::string                     GetCorrectBoneNameFromName   ( std::string const& BoneName );
    std::string                     GetCorrectBoneNameFromID     ( int32_t & BoneID );

    const SString &                 GetBlockName                 ( void ) { return m_strBlockName; }
    const unsigned int &            GetBlockNameHash             ( void ) { return m_u32Hashkey; }

    CAnimBlendHierarchySAInterface *          GetAnimationHierarchy   ( const SString & strAnimationName );
    inline std::shared_ptr < CIFPAnimations > GetIFPAnimationsPointer ( void ) { return m_pIFPAnimations; }

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

   
private:
    std::shared_ptr < CIFPAnimations > m_pIFPAnimations;
    SString                            m_strBlockName;
    unsigned int                       m_u32Hashkey;
    std::vector < SAnimation > *       m_pVecAnimations;
    bool                               m_bVersion1;
    bool                               m_bUnloading;
    CAnimManager *                     m_pAnimManager;

    // 32 because there are 32 bones in a ped model 
    const unsigned short               m_kcIFPSequences = 32;
    // We'll keep all key frames compressed by default. GTA:SA will decompress
    // them, when it's going to play the animation. We don't need to worry about it.
    const bool                         m_kbAllKeyFramesCompressed = true;

    const DWORD m_karruBoneIds [ 32 ] = 
    { 
        0, 1, 2, 3, 4, 5, 8, 6, 7, 31, 32, 33, 34, 35, 36, 21, 22, 23, 24, 25, 26, 302, 301, 201, 41, 42, 43, 44, 51, 52, 53, 54 
    };
    const char m_karrstrBoneNames [ 32 ] [ 24 ] = 
    {
        "Normal", "Pelvis", "Spine", "Spine1", "Neck", "Head", "Jaw", "L Brow", "R Brow", "Bip01 L Clavicle", "L UpperArm", 
        "L ForeArm", "L Hand", "L Finger", "L Finger01", "Bip01 R Clavicle", "R UpperArm", "R ForeArm", "R Hand", "R Finger", 
        "R Finger01", "L breast", "R breast", "Belly", "L Thigh", "L Calf", "L Foot", "L Toe0", "R Thigh", "R Calf", "R Foot", "R Toe0"
    };
};

#endif
