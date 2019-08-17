#pragma once
/*
#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)
#define NAKED __declspec(naked)

struct tArrayItem;

struct tArrayDescriptor
{
    const tArrayItem* pItems;
    uint32_t          count;
};

union tVariableValue {
    uint64_t         Integer;
    double           Double;
    float            Float;
    void*            Ptr;
    tArrayDescriptor Array;

    // Empty constructor
    tVariableValue() {}

    // Constructor for uint64_t
    tVariableValue(const uint64_t value) { this->Integer = value; }

    // Constructor for void*
    tVariableValue(void* value) { this->Ptr = value; }


};

struct tArrayItem
{
    uint64_t       index;
    tVariableValue value;

    tArrayItem(uint64_t index, const tVariableValue& value)
    {
        this->index = index;
        this->value = value;
    }
};


#define UNDEFINED_FILE_ID_INT32 0xFFFFFFFF
typedef int32_t tManagerBaseID;

enum eFileTypeGeneral
{
    GENERAL_FILE_TYPE_UNDEFINED,

    GENERAL_FILE_TYPE_MODEL_ARCHIVE,
    GENERAL_FILE_TYPE_TEXTURE_ARCHIVE,
    GENERAL_FILE_TYPE_COLLISION_ARCHIVE,
    GENERAL_FILE_TYPE_ANIMATION_ARCHIVE,
    GENERAL_FILE_TYPE_ITEM_DEFINITION,
    GENERAL_FILE_TYPE_MAP_PLACEMENT,
    GENERAL_FILE_TYPE_PATH_NODES,
    GENERAL_FILE_TYPE_CAR_RECORDING,
    GENERAL_FILE_TYPE_SCM,
    GENERAL_FILE_TYPE_LUA
};

#define MAKE_FILE_TYPE_LIST(formatMacro) \
	formatMacro(FILE_TYPE_MODEL, GENERAL_FILE_TYPE_MODEL_ARCHIVE) \
	formatMacro(FILE_TYPE_DFF, GENERAL_FILE_TYPE_MODEL_ARCHIVE) \
	formatMacro(FILE_TYPE_NIF, GENERAL_FILE_TYPE_MODEL_ARCHIVE) \
	formatMacro(FILE_TYPE_TXD, GENERAL_FILE_TYPE_TEXTURE_ARCHIVE) \
	formatMacro(FILE_TYPE_NFT, GENERAL_FILE_TYPE_TEXTURE_ARCHIVE) \
	formatMacro(FILE_TYPE_WTD, GENERAL_FILE_TYPE_TEXTURE_ARCHIVE) \
	formatMacro(FILE_TYPE_COL, GENERAL_FILE_TYPE_COLLISION_ARCHIVE) \
	formatMacro(FILE_TYPE_AGR, GENERAL_FILE_TYPE_UNDEFINED) \
	formatMacro(FILE_TYPE_IPB, GENERAL_FILE_TYPE_MAP_PLACEMENT) \
	formatMacro(FILE_TYPE_IPL, GENERAL_FILE_TYPE_MAP_PLACEMENT) \
	formatMacro(FILE_TYPE_MAP_PLACEMENT, GENERAL_FILE_TYPE_MAP_PLACEMENT) \
	formatMacro(FILE_TYPE_DAT, GENERAL_FILE_TYPE_PATH_NODES) \
	formatMacro(FILE_TYPE_IFP, GENERAL_FILE_TYPE_ANIMATION_ARCHIVE) \
	formatMacro(FILE_TYPE_WAD, GENERAL_FILE_TYPE_ANIMATION_ARCHIVE) \
	formatMacro(FILE_TYPE_RRR, GENERAL_FILE_TYPE_CAR_RECORDING) \
	formatMacro(FILE_TYPE_LIP, GENERAL_FILE_TYPE_UNDEFINED) \
	formatMacro(FILE_TYPE_CAT, GENERAL_FILE_TYPE_UNDEFINED) \
	formatMacro(FILE_TYPE_SCM, GENERAL_FILE_TYPE_SCM) \
	formatMacro(FILE_TYPE_LUR, GENERAL_FILE_TYPE_UNDEFINED) \
	formatMacro(FILE_TYPE_LOADED_START, GENERAL_FILE_TYPE_UNDEFINED) \
	formatMacro(FILE_TYPE_REQUESTED_START, GENERAL_FILE_TYPE_UNDEFINED) \

#define PUT_ENUM_MEMBER_FROM_LIST(member, generalFileType) member,

enum eFileType
{
    MAKE_FILE_TYPE_LIST(PUT_ENUM_MEMBER_FROM_LIST)

    FILE_TYPE_UNDEFINED,

    FILE_TYPE_ENUM_MAX = FILE_TYPE_UNDEFINED,
    FILE_TYPE_FIRST = FILE_TYPE_MODEL
};

// DEFINE_ENUM_OPERATORS(eFileType);

struct tFileTypeDescriptor
{
    eFileType        fileType;
    const char*      fileTypeName;
    eFileTypeGeneral generalFileType;
};

class CFileTypeInfo
{
public:
    static const tFileTypeDescriptor fileDescriptorArray[];

    // Finds enum member by name 
    static eFileType GetEnumMemberByName(const char* name);

    // Finds name by enum member 
    static const char* GetEnumNameByMember(eFileType member);

    // Finds general file type descriptor by enum member 
    static const tFileTypeDescriptor* GetFileInfoDescriptorByMember(eFileType member);
};


// GTA San Andreas
namespace Game_GTASA
{

#pragma pack(push, 1)
class CStreamingInfoInGTASA
{
public:
    uint16_t usNext;		// unused when 32-bit indices on
    uint16_t usPrev;		// unused when 32-bit indices on
    uint16_t usNextOnCd;	// unused when 32-bit indices on
    char ucFlags;
    uint8_t ucImgId;
    int iBlockOffset;		// m_OriginalSizeInSectors
    int iBlockCount;
    uint8_t uiLoadFlag;
    char field_11[3];
};
#pragma pack(pop)


#pragma pack(push, 1)
class CStreamingInfo : public CStreamingInfoInGTASA
{
public:
    // Returns position and size of file
    bool GetCdPosnAndSize(unsigned int& outBlockOffsetWithStreamHandle, unsigned int& outBlockCount);

    // Sets position and size of file
    void SetCdPosnAndSize(unsigned int BlockOffset, unsigned int BlockCount);
};
#pragma pack(pop)

VALIDATE_SIZE(CStreamingInfo, 0x14);

#pragma pack(push, 1)
struct TxdDef
{
    void* dictionary;
    int16_t ref_counter;
    int16_t parent_index;
    int hash;
};
#pragma pack(pop)

VALIDATE_SIZE(TxdDef, 0xC);

#pragma pack(push, 1)
struct CIplFile
{
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    char name[16];
    int16_t field_20;
    int16_t field_22;
    int16_t field_24;
    int16_t field_26;
    int16_t field_28;
    int16_t iplId;
    char field_2C;
    char field_2D;
    char field_2E;
    char field_2F;
    char field_30;
    char field_31;
    char field_32;
    char field_33;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CCollisionFile
{
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    int field_14;
    int field_18;
    int field_1C;
    int16_t field_20;
    int16_t lowestModelID;
    int16_t highestModelID;
    int16_t nReferenceCount;
    char bFileLoaded;
    char field_29;
    char field_2A;
    char interiorCollision;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CAnimBlock
{
    char name[16];
    char loaded;
    char field_11;
    int16_t usageCount;
    int startAnimation;
    int animationCount;
    int animationStyle;
};
#pragma pack(pop)

VALIDATE_SIZE(CAnimBlock, 0x20);

#pragma pack(push, 1)
struct CPath
{
    int number;
    int pData;
    int field_8;
    char usageCount;
    char __pad[3];
};
#pragma pack(pop)

VALIDATE_SIZE(CPath, 0x10);

#pragma pack(push, 1)
struct CStreamedScriptInfo
{
    void* data;
    char status;
    char field_5;
    int16_t wScmIndex;
    char name[20];
    int size;
};
#pragma pack(pop)

VALIDATE_SIZE(CStreamedScriptInfo, 0x20);

#pragma pack(push, 1)
struct CStreamedScripts_Footer
{
    int largestExternalSize;
    int16_t countOfScripts;
    int16_t field_A46;
};
#pragma pack(pop)

// CDarkel
class CDarkel
{
public:
    // Count of killable model IDs
    static unsigned int CountOfKillableModelIDs;

    // Registered kills
    static int16_t* RegisteredKills;

    // Finds total number of peds killed by player.
    static int __cdecl FindTotalPedsKilledByPlayer(int playerID);
};
}


#pragma pack(push, 4)
class CStreamingInfoExtension
{
public:
    uint32_t dwSize;
    int32_t exUsNext;
    int32_t exUsPrev;
    int32_t exNextOnCd;

    uint32_t m_NumberOfPaddedBytesInAlignedOriginalSize;

    uint32_t m_PackedSizeInSectors;

    uint32_t m_NumberOfPaddedBytesInAlignedPackedSize;

    uint32_t m_exFlags;

#ifdef STORE_FILE_NAME_FIELD_IN_CSTREAMINGINFO_EXTENSION
    char m_Filename[IMG_FASTMAN92_GTASA_MAX_FILENAME_LENGTH];
#endif

    // Initialize
    void Init();
};
#pragma pack(pop)


// CColModel_extension
struct CColModel_extension
{
    uint16_t exCOL_file_ID;

    // Inits
    void Init()
    {
        this->exCOL_file_ID = 0;
    }
};

*/