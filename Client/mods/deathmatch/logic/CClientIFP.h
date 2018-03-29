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
    typedef CIFPAnimations::IFP_Animation IFP_Animation;

    struct IFP_BASE 
    {
        char     FourCC[4];
        uint32_t Size;
    };

    struct IFP_INFO
    {
        IFP_BASE Base;
        int32_t  Entries;
        char     Name[24];
    };

    struct IFP_ANPK
    {
        IFP_BASE Base;
        IFP_INFO Info;
    };

    struct IFP_NAME
    {
        IFP_BASE Base;
    };

    struct IFP_DGAN 
    {
        IFP_BASE Base;
        IFP_INFO Info;
    };

    struct IFP_CPAN 
    {
        IFP_BASE Base;
    };

    struct IFP_ANIM 
    {
        IFP_BASE Base;
        char     Name[28];
        int32_t  Frames;
        int32_t  Unk;
        int32_t  Next;
        int32_t  Previous;

        // According to https://www.gtamodding.com/wiki/IFP, Unk2 should not exist, but for some reason, it's there
        // I don't know why. Let's just go with the flow and ignore it. The value seems to be always zero. 
        int32_t  Unk2;
    };

    struct IFP_KFRM
    {
        IFP_BASE Base;
    };

    struct Quaternion
    {
        float X, Y, Z, W;
    };

    struct IFP_CVector
    {
        float X, Y, Z;
    };

    struct IFP_KR00
    {
        Quaternion Rotation;
        float      Time;
    };

    struct IFP_KRT0
    {
        Quaternion Rotation;
        IFP_CVector    Translation;
        float      Time;
    };

    struct IFP_KRTS
    {
        Quaternion Rotation;
        IFP_CVector    Translation;
        IFP_CVector    Scale;
        float      Time;
    };

    struct CompressedQuaternion
    {
        int16_t X, Y, Z, W;
    };

    struct CompressedCVector
    {
        int16_t X, Y, Z;
    };


    struct IFP_Compressed_KR00
    {
        CompressedQuaternion Rotation;
        int16_t              Time;
    };

    struct IFP_Compressed_KRT0 : IFP_Compressed_KR00
    {
        CompressedCVector Translation;
    };


    enum IFP_FrameType
    {
        UNKNOWN_FRAME   = -1,
        KR00            = 0,
        KRT0            = 1,
        KRTS            = 2,
        KR00_COMPRESSED = 3,
        KRT0_COMPRESSED = 4,
    };

    struct IFPHeaderV2
    {
        int32_t   OffsetEOF;
        char      InternalFileName[24];
        int32_t   TotalAnimations;
    };

    struct Animation
    {
        // 24 + 4 + 4 + 4 = 36 bytes

        char      Name[24];
        int32_t   TotalObjects;
        int32_t   FrameSize;
        int32_t   isCompressed; // The value is always 1
    };


    struct Object
    {
        // 24 + 4 + 4 + 4 = 36 bytes

        char      Name[24];
        int32_t   FrameType;
        int32_t   TotalFrames;
        int32_t   BoneID;
    };

    struct IFP2_ChildFrame
    {
        int16_t x, y, z, w, time;
    };

    enum BoneType 
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

    const SString &                 GetBlockName                 ( void ) { return m_strBlockName; }
    bool                            LoadIFP                      ( const char* szFilePath, const SString & strBlockName );

    bool                            LoadIFPFile                  ( const char * szFilePath );
    void                            ReadIFPVersion1              ( void );
    void                            ReadIFPVersion2              ( bool bAnp3 );

    WORD                            ReadSequencesWithDummies     ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );
    WORD                            ReadSequences                ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    WORD                            ReadSequencesVersion1        ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    WORD                            ReadSequencesVersion2        ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences );
    int32_t                         ReadSequenceVersion1         ( IFP_ANIM & Anim );
    void                            ReadSequenceVersion2         ( Object & ObjectNode );

    void                            ReadHeaderVersion1           ( IFP_INFO & Info );
    void                            ReadAnimationNameVersion1    ( SString & strAnimationName);
    void                            ReadDgan                     ( IFP_DGAN & Dgan );
    CClientIFP::IFP_FrameType       ReadKfrm                     ( void );
    void                            ReadAnimationHeaderVersion2  ( Animation & AnimationNode, bool bAnp3 );

    bool                            ReadSequenceKeyFrames        ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, IFP_FrameType iFrameType, int32_t iFrames );
    void                            ReadKeyFramesAsCompressed    ( IFP_FrameType iFrameType, BYTE * pKeyFrames, int32_t iFrames );
    void                            ReadKrtsFramesAsCompressed   (  BYTE * pKeyFrames, int32_t TotalFrames );
    void                            ReadKrt0FramesAsCompressed   (  BYTE * pKeyFrames, int32_t TotalFrames );
    void                            ReadKr00FramesAsCompressed   (  BYTE * pKeyFrames, int32_t TotalFrames );
    void                            ReadKr00CompressedFrames     (  BYTE * pKeyFrames, int32_t TotalFrames );
    void                            ReadKrt0CompressedFrames     (  BYTE * pKeyFrames, int32_t TotalFrames );

    void                            InitializeAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, const char * szAnimationName, const int32_t iSequences );
    void                            InitializeAnimationSequence  ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, const char * szName, const int32_t iBoneID );
    void                            PreProcessAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );    
    void                            CopySequencesWithDummies     ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & mapOfSequences );
    BYTE *                          AllocateSequencesMemory      ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy );

    void                            InsertAnimationDummySequence ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, std::string & BoneName, DWORD & dwBoneID );
    void                            CopyDummyKeyFrameByBoneID    ( BYTE * pKeyFrames, DWORD dwBoneID );
    std::string                     ConvertStringToMapKey        ( const char * szString );

    constexpr void                  RoundSize                    ( uint32_t & u32Size );
    constexpr bool                  IsKeyFramesTypeRoot          ( IFP_FrameType iFrameType );

    IFP_FrameType                   GetFrameTypeFromFourCC       ( const char * szFourCC );
    size_t                          GetSizeOfCompressedFrame     ( IFP_FrameType FrameType );
    int32_t                         GetBoneIDFromName            ( std::string const& BoneName );
    std::string                     GetCorrectBoneNameFromName   ( std::string const& BoneName );
    std::string                     GetCorrectBoneNameFromID     ( int32_t & BoneID );

    CAnimBlendHierarchySAInterface *          GetAnimationHierarchy   ( const SString & strAnimationName );
    inline std::shared_ptr < CIFPAnimations > GetIFPAnimationsPointer ( void ) { return m_pIFPAnimations; }

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

   
private:
    std::shared_ptr < CIFPAnimations > m_pIFPAnimations;
    SString                            m_strBlockName;
    std::vector < IFP_Animation > *    m_pVecAnimations;
    bool                               m_bVersion1;
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
