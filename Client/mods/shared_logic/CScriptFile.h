/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CScriptFile.h
*  PURPOSE:     Script file class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __CFILE_H
#define __CFILE_H

#include "CClientEntity.h"
#include <stdio.h>
#include <string>

class CScriptFile : public CClientEntity
{
    DECLARE_CLASS( CScriptFile, CClientEntity )
public:
    enum eMode
    {
        MODE_READ,
        MODE_READWRITE,
        MODE_CREATE,
    };

                            CScriptFile             ( uint uiScriptId, const char* szFilename, unsigned long ulMaxSize, eAccessType accessType );
                            ~CScriptFile            ( void );

    // Functions required for linking
    void                    GetPosition             ( CVector& vecPosition ) const {};
    void                    SetPosition             ( const CVector& vecPosition ) {};

    // Functions required by CClientEntity
    eClientEntityType       GetType                 ( void ) const  { return SCRIPTFILE; };
    void                    Unlink                  ( void ) {};
    bool                    ReadSpecialData         ( void )    { return true; };

    // Load and unload routines
    bool                    Load                    (  CResource* pResourceForFilePath, eMode Mode );
    void                    Unload                  ( void );
    bool                    IsLoaded                ( void )    { return m_pFile != NULL; };
    const SString&          GetFilePath             ( void )    { return m_strFilename; };
    const SString&          GetAbsPath              ( void )    { return m_strAbsPath; };

    // Get the owning resource
    CResource*              GetResource             ( void );

    // Only call functions belw this if you're sure that the file is loaded.
    // Or you will crash.
    bool                    IsEOF                   ( void );
    long                    GetPointer              ( void );
    long                    GetSize                 ( void );

    long                    SetPointer              ( unsigned long ulPosition );

    void                    Flush                   ( void );
    long                    Read                    ( unsigned long ulSize, CBuffer& outBuffer );
    long                    Write                   ( unsigned long ulSize, const char* pData );

private:
    void                    DoResourceFileCheck     ( void );

    CBinaryFileInterface*   m_pFile;
    SString                 m_strFilename;      // Resource relative
    SString                 m_strAbsPath;       // Absolute
    unsigned long           m_ulMaxSize;
    bool                    m_bDoneResourceFileCheck;
    unsigned int            m_uiScriptId;
    CResource*              m_pResource;
    eAccessType             m_accessType;
};

#endif
