/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientIFP.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include "CFileReader.h"
#include "CIFPAnimations.h"
#include <game/CAnimBlendSequence.h>

class CAnimBlendSequence;

class CClientIFP final : public CClientEntity, CFileReader
{
public:
    typedef CIFPAnimations::SAnimation                           SAnimation;
    typedef std::map<DWORD, std::unique_ptr<CAnimBlendSequence>> SequenceMapType;

    struct SBase
    {
        char          FourCC[4];
        std::uint32_t Size;
    };

    struct SInfo
    {
        SBase        Base;
        std::int32_t Entries;
        char         Name[24];
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
        SBase        Base;
        char         Name[28];
        std::int32_t Frames;
        std::int32_t Unk;
        std::int32_t Next;
        std::int32_t Previous;

        // According to https://www.gtamodding.com/wiki/IFP, Unk2 should not exist, but for some reason, it's there
        // I don't know why. Let's just go with the flow and ignore it. The value seems to be always zero.
        std::int32_t Unk2;
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
        std::int16_t X, Y, Z, W;
    };

    struct SCompressedCVector
    {
        std::int16_t X, Y, Z;
    };

    struct SCompressed_KR00
    {
        SCompressedQuaternion Rotation;
        std::int16_t          Time;
    };

    struct SCompressed_KRT0 : SCompressed_KR00
    {
        SCompressedCVector Translation;
    };

    enum eFrameType
    {
        UNKNOWN_FRAME = -1,
        KR00 = 0,
        KRT0 = 1,
        KRTS = 2,
        KR00_COMPRESSED = 3,
        KRT0_COMPRESSED = 4,
    };

    struct SIFPHeaderV2
    {
        std::uint32_t OffsetEOF;
        char          InternalFileName[24];
        std::int32_t  TotalAnimations;
    };

    struct SAnimationHeaderV2
    {
        char         Name[24];
        std::int32_t TotalObjects;
        std::int32_t FrameSize;
        std::int32_t isCompressed;  // The value is always 1
    };

    struct SSequenceHeaderV2
    {
        char         Name[24];
        std::int32_t FrameType;
        std::int32_t TotalFrames;
        std::int32_t BoneID;
    };

    enum eBoneType
    {
        UNKNOWN = -1,
        NORMAL = 0,  // Normal or Root, both are same
        PELVIS = 1,
        SPINE = 2,
        SPINE1 = 3,
        NECK = 4,
        HEAD = 5,
        JAW = 8,
        L_BROW = 6,
        R_BROW = 7,
        L_CLAVICLE = 31,
        L_UPPER_ARM = 32,
        L_FORE_ARM = 33,
        L_HAND = 34,
        L_FINGER = 35,
        L_FINGER_01 = 36,
        R_CLAVICLE = 21,
        R_UPPER_ARM = 22,
        R_FORE_ARM = 23,
        R_HAND = 24,
        R_FINGER = 25,
        R_FINGER_01 = 26,
        L_BREAST = 302,
        R_BREAST = 301,
        BELLY = 201,
        L_THIGH = 41,
        L_CALF = 42,
        L_FOOT = 43,
        L_TOE_0 = 44,
        R_THIGH = 51,
        R_CALF = 52,
        R_FOOT = 53,
        R_TOE_0 = 54,

        // Cutscene bones
        R_Thumb1 = 28,
        R_Thumb2 = 29,
        LLIP11 = 30,
        L_Thumb1 = 38,
        L_Thumb2 = 39,

        JAW22 = 40,

        HEADNUB = 303,
        L_Finger0Nub = 304,
        R_Finger0Nub = 305,
        L_Toe0Nub = 306,
        R_Toe0Nub = 307,

        R_BROW1 = 5001,
        R_BROW2 = 5002,
        L_BROW2 = 5003,
        L_BROW1 = 5004,
        R_LID = 5005,
        L_LID = 5006,
        R_TLIP3 = 5007,
        L_TLIP3 = 5008,
        R_TLIP1 = 5009,
        R_TLIP2 = 5010,
        L_TLIP1 = 5011,
        L_TLIP2 = 5012,
        R_CORNER = 5013,
        L_CORNER = 5014,
        JAW1 = 5015,
        JAW2 = 5016,
        LLIP1 = 5017,
        R_EYE = 5018,
        L_EYE = 5019,
        R_CHEEK = 5020,
        L_CHEEK = 5021,
    };

    CClientIFP(class CClientManager* pManager, ElementID ID);
    virtual eClientEntityType GetType() const { return CCLIENTIFP; }

    void MarkAsUnloading() { m_bUnloading = true; }
    bool IsUnloading() { return m_bUnloading; }

    bool Load(SString blockName, bool isRawData, SString input);

    const SString&      GetBlockName() { return m_strBlockName; }
    const unsigned int& GetBlockNameHash() { return m_u32Hashkey; }

    CAnimBlendHierarchySAInterface* GetAnimationHierarchy(const SString& strAnimationName);
    std::shared_ptr<CIFPAnimations> GetIFPAnimationsPointer() { return m_pIFPAnimations; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition) {};

private:
    bool ReadIFPByVersion();
    void ReadIFPVersion1();
    void ReadIFPVersion2(bool bAnp3);

    WORD         ReadSequencesWithDummies(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy);
    WORD         ReadSequences(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy, SequenceMapType& MapOfSequences);
    WORD         ReadSequencesVersion1(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy, SequenceMapType& MapOfSequences);
    WORD         ReadSequencesVersion2(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy, SequenceMapType& MapOfSequences);
    std::int32_t ReadSequenceVersion1(SAnim& Anim);
    void         ReadSequenceVersion2(SSequenceHeaderV2& ObjectNode);

    void                   ReadHeaderVersion1(SInfo& Info);
    void                   ReadAnimationNameVersion1(SAnimation& IfpAnimation);
    void                   ReadDgan(SDgan& Dgan);
    CClientIFP::eFrameType ReadKfrm();
    void                   ReadAnimationHeaderVersion2(SAnimationHeaderV2& AnimationNode, bool bAnp3);

    bool ReadSequenceKeyFrames(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, eFrameType iFrameType, const std::int32_t& cFrames);
    void ReadKeyFramesAsCompressed(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, eFrameType iFrameType, const std::int32_t& cFrames);
    void ReadKrtsFramesAsCompressed(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, const std::int32_t& cFrames);
    void ReadKrt0FramesAsCompressed(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, const std::int32_t& cFrames);
    void ReadKr00FramesAsCompressed(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, const std::int32_t& cFrames);

    template <class T>
    void ReadCompressedFrames(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, std::int32_t iFrames)
    {
        BYTE*  pKeyFrames = pAnimationSequence->GetKeyFrames();
        size_t iSizeInBytes = sizeof(T) * iFrames;
        if (!pKeyFrames)
        {
            SkipBytes(static_cast<std::uint32_t>(iSizeInBytes));
            return;
        }
        ReadBytes(pKeyFrames, iSizeInBytes);
    }

    void  InitializeAnimationHierarchy(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy, const SString& strAnimationName,
                                       const std::int32_t& iSequences);
    void  InitializeAnimationSequence(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, const SString& strName, const std::int32_t& iBoneID);
    void  PreProcessAnimationHierarchy(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy);
    void  MoveSequencesWithDummies(std::unique_ptr<CAnimBlendHierarchy>&                 pAnimationHierarchy,
                                   std::map<DWORD, std::unique_ptr<CAnimBlendSequence>>& mapOfSequences);
    BYTE* AllocateSequencesMemory(std::unique_ptr<CAnimBlendHierarchy>& pAnimationHierarchy);

    void    InsertAnimationDummySequence(std::unique_ptr<CAnimBlendSequence>& pAnimationSequence, const SString& BoneName, const DWORD& dwBoneID);
    void    CopyDummyKeyFrameByBoneID(BYTE* pKeyFrames, DWORD dwBoneID);
    SString ConvertStringToKey(const SString& strBoneName, bool isANPK = false);

    constexpr void RoundSize(std::uint32_t& u32Size);
    constexpr bool IsKeyFramesTypeRoot(eFrameType iFrameType);

    eFrameType   GetFrameTypeFromFourCC(const char* szFourCC);
    size_t       GetSizeOfCompressedFrame(eFrameType FrameType);
    size_t       GetSourceFrameDataSize(eFrameType iFrameType);
    std::int32_t GetBoneIDFromName(const SString& strBoneName);
    SString      GetCorrectBoneNameFromName(const SString& strBoneName);
    SString      GetCorrectBoneNameFromID(const std::int32_t& iBoneID);

    std::shared_ptr<CIFPAnimations> m_pIFPAnimations;
    SString                         m_strBlockName;
    unsigned int                    m_u32Hashkey;
    std::vector<SAnimation>*        m_pVecAnimations;
    bool                            m_bVersion1;
    bool                            m_bUnloading;
    CAnimManager*                   m_pAnimManager;

    // 64 because there are 32 bones in a ped model + 32 possibly cutscene bones
    const unsigned short m_kcIFPSequences = 64;
    // We'll keep all key frames compressed by default. GTA:SA will decompress
    // them, when it's going to play the animation. We don't need to worry about it.
    const bool m_kbAllKeyFramesCompressed = true;

    const DWORD m_karruBoneIds[64] = {0,    1,    2,    3,    4,    5,    8,    6,    7,    31,   32,   33,   34,   35,   36,   21,
                                      22,   23,   24,   25,   26,   302,  301,  201,  41,   42,   43,   44,   51,   52,   53,   54,
                                      28,   29,   30,   38,   39,   40,   303,  304,  305,  306,  307,  5001, 5002, 5003, 5004, 5005,
                                      5006, 5007, 5008, 5009, 5010, 5011, 5012, 5013, 5014, 5015, 5016, 5017, 5018, 5019, 5020, 5021};
    const char  m_karrstrBoneNames[64][24] = {"Normal",       "Pelvis",
                                              "Spine",        "Spine1",
                                              "Neck",         "Head",
                                              "Jaw",          "L Brow",
                                              "R Brow",       "Bip01 L Clavicle",
                                              "L UpperArm",   "L ForeArm",
                                              "L Hand",       "L Finger",
                                              "L Finger01",   "Bip01 R Clavicle",
                                              "R UpperArm",   "R ForeArm",
                                              "R Hand",       "R Finger",
                                              "R Finger01",   "L breast",
                                              "R breast",     "Belly",
                                              "L Thigh",      "L Calf",
                                              "L Foot",       "L Toe0",
                                              "R Thigh",      "R Calf",
                                              "R Foot",       "R Toe0",
                                              "RThumb1",      "RThumb2",
                                              "llip11",       "LThumb1",
                                              "LThumb2",      "jaw22",
                                              "HeadNub",      "L Finger0Nub",
                                              "R Finger0Nub", "L Toe0Nub",
                                              "R Toe0Nub",    "rbrow1",
                                              "rbrow2",       "lbrow2",
                                              "lbrow1",       "rlid",
                                              "llid",         "rtlip3",
                                              "ltlip3",       "rtlip1",
                                              "rtlip2",       "ltlip1",
                                              "ltlip2",       "rcorner",
                                              "lcorner",      "jaw1",
                                              "jaw2",         "llip1",
                                              "reye",         "leye",
                                              "rcheek",       "lcheek"};
};
