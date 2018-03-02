#pragma once
#ifndef IFPLOADER_H
#define IFPLOADER_H
#include "wrapper.h"
#include "FileLoader.h"
#include <vector>

#define ROUNDSIZE(x) if((x) & 3) (x) += 4 - ((x)&3) // credits to github.com/aap for the great macro
#define IFP_TOTAL_SEQUENCES 32                      // 32 because there are 32 bones in a ped model 


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
    UNKNOWN_FRAME = -1,
    KR00 = 0,
    KRT0 = 1,
    KRTS = 2
};

struct IFPHeaderV2
{
    int32_t   OffsetEOF;
    char      InternalFileName[24];
    int32_t   TotalAnimations;
};

struct IFP : FileLoader
{   
    bool isVersion1;
    IFPHeaderV2 HeaderV2;
    std::vector <_CAnimBlendHierarchy> AnimationHierarchies;
    std::vector <_CAnimBlendSequence> AnimationSequences;
    unsigned char * KeyFramesArray;
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

typedef void *(__cdecl* hCMemoryMgr_Malloc)
(
    size_t TotalBytesToAllocate
);


void LoadIFPFile(const char * FilePath);
void ReadIFPVersion1();
void ReadIFPVersion2( bool anp3);

void insertAnimDummySequence(bool anp3, _CAnimBlendHierarchy * pAnimHierarchy, size_t SequenceIndex);
int32_t getBoneIDFromName(std::string const& BoneName);
std::string getCorrectBoneNameFromName(std::string const& BoneName);
std::string getCorrectBoneNameFromID(int32_t & BoneID);
size_t getCorrectBoneIndexFromID(int32_t & BoneID);

void _CAnimBlendHierarchy_Constructor(_CAnimBlendHierarchy * pAnimHierarchy);
void _CAnimBlendSequence_Constructor(_CAnimBlendSequence * pSequence);




// --------------------------------------  For Hierarchy ----------------------------------------------------

void Call__CAnimBlendHierarchy_RemoveQuaternionFlips(_CAnimBlendHierarchy * pAnimHierarchy);

void Call__CAnimBlendHierarchy_CalcTotalTime(_CAnimBlendHierarchy * pAnimHierarchy);

#endif
