/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CModelNames.h
*  PURPOSE:     Model names class header
*  DEVELOPERS:  aidiot
*
*****************************************************************************/

#define CLOTHES_MODEL_ID_FIRST  30000
#define CLOTHES_MODEL_ID_LAST   30151
#define CLOTHES_TEX_ID_FIRST    30152
#define CLOTHES_TEX_ID_LAST     30541
#define INVALID_MODEL_ID        32000

class CModelNames
{
public:
    static ushort           GetModelID              ( const SString& strName );
    static ushort           GetClothesTexID         ( const SString& strName );
    static const char*      GetModelName            ( ushort usModelID );
    static ushort           ResolveModelID          ( const SString& strModelNameOrNumber );
    static ushort           ResolveClothesTexID     ( const SString& strTexNameOrNumber );

protected:
    static void             InitializeMaps          ( void );

    static std::map < ushort, const char* >     ms_ModelIDNameMap;
    static std::map < SString, ushort >         ms_NameModelIDMap;
    static std::map < ushort, const char* >     ms_ClothesModelIDNameMap;
    static std::map < SString, ushort >         ms_NameClothesModelIDMap;
    static std::map < ushort, const char* >     ms_ClothesTexIDNameMap;
    static std::map < SString, ushort >         ms_NameClothesTexIDMap;
};
