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

#define INVALID_MODEL_ID 32000

class CModelNames
{
public:
    static ushort           GetModelID              ( const SString& strName );
    static const char*      GetModelName            ( ushort usModelID );
    static ushort           ResolveModelID          ( const SString& strModelName );

protected:
    static void             InitializeMaps          ( void );

    static std::map < ushort, const char* >     ms_ModelIDNameMap;
    static std::map < SString, ushort >         ms_NameModelIDMap;
};
