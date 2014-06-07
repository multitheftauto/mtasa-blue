/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/deathmatch/logic/CIdArray.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/*
    ID Usage:

    Server:
        CElement                                0x00000000 - 0x0001FFFF                 - 131,072
        CXMLNode                                0x01000000 - 0x01FFFFFF                 - 16,777,216
        CAccount                shared array >> 0x02000000 - 0x02FFFFFF                 - 16,777,216        
        CAccessControlListGroup shared array
        CAccessControlList      shared array
        CBan                    shared array                    
        CDbJobData              shared array
        CResource               shared array
        CTextItem               shared array
        CTextDisplay            shared array
        CLuaTimer               shared array

    Client:
        CClientElement                          0x00020000 - 0x0003FFFF                 - 131,072
        CXMLNode                                0x01000000 - 0x01FFFFFF                 - 16,777,216
        CResource               shared array >> 0x02000000 - 0x02FFFFFF                 - 16,777,216
        CLuaTimer               shared array     
*/



typedef unsigned long SArrayId;
#define INVALID_ARRAY_ID    0xFFFFFFFF

namespace EIdClass
{
    enum EIdClassType
    {
        NONE,
        ACCOUNT,
        ACL,
        ACL_GROUP,
        BAN,
        DB_JOBDATA,
        RESOURCE,
        TEXT_DISPLAY,
        TEXT_ITEM,
        TIMER,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        MATRIX
    };
};

using EIdClass::EIdClassType;


struct SIdEntry
{
    void*           pObject;
    EIdClassType    idClass;
};


class CIdArray
{
public:
    static void                 Initialize          ( void );

    static SArrayId             PopUniqueId         ( void* pObject, EIdClassType idClass );
    static void                 PushUniqueId        ( void* pObject, EIdClassType idClass, SArrayId id );

    static void*                FindEntry           ( SArrayId id, EIdClassType idClass );
    static uint                 GetCapacity         ( void );
    static uint                 GetUnusedAmount     ( void );

private:
    static void                 ExpandBy            ( uint uiAmount );

    static CStack < SArrayId, 1 >           m_IDStack;
    static std::vector < SIdEntry >         m_Elements;
    static uint                             m_uiCapacity;
    static bool                             m_bInitialized;
    static uint                             m_uiPopIdCounter;
    static uint                             m_uiMinTicksBetweenIdReuse;
    static CTickCount                       m_LastPeriodStart;
};

