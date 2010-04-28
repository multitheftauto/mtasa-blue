/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFileDefs.cpp
*  PURPOSE:     Lua file definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

#define DEFAULT_MAX_FILESIZE 52428800

void CLuaFileDefs::LoadFunctions ( void )
{
    /*
    CLuaCFunctions::AddFunction ( "fileCreate", CLuaFileDefs::fileCreate );
    CLuaCFunctions::AddFunction ( "fileOpen", CLuaFileDefs::fileOpen );
    CLuaCFunctions::AddFunction ( "fileIsEOF", CLuaFileDefs::fileIsEOF );
    CLuaCFunctions::AddFunction ( "fileGetPos", CLuaFileDefs::fileGetPos );
    CLuaCFunctions::AddFunction ( "fileSetPos", CLuaFileDefs::fileSetPos );
    CLuaCFunctions::AddFunction ( "fileRead", CLuaFileDefs::fileRead );
    CLuaCFunctions::AddFunction ( "fileWrite", CLuaFileDefs::fileWrite );
    CLuaCFunctions::AddFunction ( "fileFlush", CLuaFileDefs::fileFlush );
    CLuaCFunctions::AddFunction ( "fileClose", CLuaFileDefs::fileClose );
    */
}

/*
int CLuaFileDefs::fileCreate ( lua_State* luaVM )
{
    // file fileCreate ( string filename, resource from = getThisResource () )

    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    assert ( pLuaMain ); ADD AN IF HERE, MIGHT BE 0

    // Check argument types
    if ( argtype ( 1, LUA_TSTRING ) )
    {
        // Grab the filename
        const char* szFilename = lua_tostring ( luaVM, 1 );

        // We have a resource argument?
        CResource* pResource = pLuaMain->GetResource ();
        if ( argtype ( 2, LUA_TLIGHTUSERDATA ) )
        {
            // Grab and verify it
            pResource = reinterpret_cast < CResource* > ( lua_touserdata ( luaVM, 2 ) );
            if ( !SCRIPT_VERIFY_RESOURCE ( pResource ) )
            {
                // Bad argument
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileCreate", "resource", 2 );

                // Failed
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        // Legal filepath? And don't allow the meta.xml to be overwritten.
        if ( IsValidFilePath ( szFilename ) &&
             stricmp ( szFilename, "meta.xml" ) != 0 )
        {
            // Grab the absolute filepath to the file
            SString strAbsoluteFilename = SString::Printf ( "%s\\%s", pResource->GetResourceDirectoryPath (), szFilename );

            // We have a resource to use?
            if ( pResource )
            {
                // Create the file to create
                CScriptFile* pFile = new CScriptFile ( strAbsoluteFilename, DEFAULT_MAX_FILESIZE );
                assert ( pFile );

                // Try to load it
                if ( pFile->Load ( CScriptFile::MODE_CREATE ) )
                {
                    // Make it a child of the resource's file root
                    pFile->SetParentObject ( pResource->GetDynamicElementRoot () );

                    // Success. Return its ID as unsigned short in a light user data.
                    void* usID = reinterpret_cast < void* > ( pFile->GetID () );
                    lua_pushlightuserdata ( luaVM, usID );
                    return 1;
                }
                else
                {
                    // Delete the file again
                    delete pFile;

                    // Output error
                    m_pScriptDebugging->LogError ( luaVM, "fileCreate; unable to load file" );
                }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "fileCreate; bad resource" );
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "fileCreate; bad filename" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileOpen ( lua_State* luaVM )
{
    // file fileOpen ( string filename, resource from = getThisResource () )

    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    assert ( pLuaMain ); ADD AN IF HERE, MIGHT BE 0

    // Check argument types
    if ( argtype ( 1, LUA_TSTRING ) )
    {
        // Grab the filename
        const char* szFilename = lua_tostring ( luaVM, 1 );

        // We have a read only argument?
        bool bReadOnly = false;
        if ( argtype ( 2, LUA_TBOOLEAN ) )
        {
            bReadOnly = lua_toboolean ( luaVM, 2 ) ? true:false;
        }

        // We have a resource argument?
        CResource* pResource = pLuaMain->GetResource ();
        if ( argtype ( 3, LUA_TLIGHTUSERDATA ) )
        {
            // Grab and verify it
            pResource = reinterpret_cast < CResource* > ( lua_touserdata ( luaVM, 3 ) );
            if ( !SCRIPT_VERIFY_RESOURCE ( pResource ) )
            {
                // Bad argument
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileOpen", "resource", 3 );

                // Failed
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        // Legal filepath? And don't allow the meta.xml to be overwritten.
        if ( IsValidFilePath ( szFilename ) &&
             stricmp ( szFilename, "meta.xml" ) != 0 )
        {
            // Grab the absolute filepath to the file
            SString strAbsoluteFilename = SString::Printf ( "%s\\%s", pResource->GetResourceDirectoryPath (), szFilename );

            // We have a resource to use?
            if ( pResource )
            {
                // Create the file to create
                CScriptFile* pFile = new CScriptFile ( strAbsoluteFilename, DEFAULT_MAX_FILESIZE );
                assert ( pFile );

                // Try to load it
                if ( ( bReadOnly && pFile->Load ( CScriptFile::MODE_READ ) ) ||
                     ( !bReadOnly && pFile->Load ( CScriptFile::MODE_READWRITE ) ) )
                {
                    // Make it a child of the resource's file root
                    pFile->SetParentObject ( pResource->GetDynamicElementRoot () );

                    // Tell the clients about it. This is because other elements might get
                    // parented below this one.
                    CEntityAddPacket Packet;
                    Packet.Add ( pDummy );
                    m_pPlayerManager->BroadcastOnlyJoined ( Packet );

                    // Success. Return its ID as unsigned short in a light user data.
                    void* usID = reinterpret_cast < void* > ( pFile->GetID () );
                    lua_pushlightuserdata ( luaVM, usID );
                    return 1;
                }
                else
                {
                    // Delete the file again
                    delete pFile;

                    // Output error
                    m_pScriptDebugging->LogError ( luaVM, "fileOpen; unable to load file" );
                }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "fileOpen; bad resource" );
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "fileOpen; bad filename" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileIsEOF ( lua_State* luaVM )
{
    // bool fileIsEOF ( file )

    // Check argument types
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Convert it to a file
            CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

            // Return its EOF state
            lua_pushboolean ( luaVM, pFile->IsEOF () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileIsEOF", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileIsEOF", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetPos ( lua_State* luaVM )
{
    // int fileGetPos ( file )

    // Check argument types
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Convert it to a file
            CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

            // Return its position
            lua_pushnumber ( luaVM, pFile->GetPointer () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileIsEOF", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileIsEOF", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileSetPos ( lua_State* luaVM )
{
    // bool fileSetPos ( file )

    // Check argument 1
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Check argument 2
            if ( argtype2 ( 2, LUA_TNUMBER, LUA_TSTRING ) )
            {
                // Grab position
                long lPosition = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );
                if ( lPosition >= 0 )
                {
                    // Convert it to a file
                    CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

                    // Set the position and return where we actually got it put
                    lua_pushnumber ( luaVM, pFile->SetPointer ( static_cast < unsigned long > ( lPosition ) ) );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "number", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "number", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileRead ( lua_State* luaVM )
{
    // string fileRead ( file, count )

    // Check argument 1
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Check argument 2
            if ( argtype2 ( 2, LUA_TNUMBER, LUA_TSTRING ) )
            {
                // Grab count arg
                long lCount = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );
                if ( lCount > 0 )
                {
                    // Convert it to a file
                    CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

                    // Allocate a buffer to read the stuff into and read some shit into it
                    char* pReadContent = new char [lCount + 1];
                    unsigned long lBytesRead = pFile->Read ( static_cast < unsigned long > ( lCount ),
                                                             pReadContent );

                    // Push the string onto the lua stack. Use pushlstring so we are binary
                    // compatible. Normal push string takes zero terminated strings.
                    lua_pushlstring ( luaVM, pReadContent, lBytesRead );

                    // Delete our read content. Lua should've stored it
                    delete [] pReadContent;

                    // We're returning the result string
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "number", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "number", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileWrite ( lua_State* luaVM )
{
    // string fileWrite ( file, string [, string2, string3, ...] )

    // Check argument 1
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Check argument 2
            if ( argtype ( 2, LUA_TSTRING ) )
            {
                // Convert it to a file
                CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

                // While we're not out of string arguments
                unsigned long ulBytesWritten = 0;
                unsigned int uiCurrentArg = 2;
                do
                {
                    // Grab argument and length
                    const char* pData = lua_tostring ( luaVM, uiCurrentArg );
                    unsigned long ulDataLen = static_cast < unsigned long > ( lua_strlen ( luaVM, uiCurrentArg ) );

                    // Write it and add the bytes written to our total bytes written
                    ulBytesWritten += pFile->Write ( ulDataLen, pData );

                    // Increment current argument
                    ++uiCurrentArg;
                }
                while ( argtype ( uiCurrentArg, LUA_TSTRING ) );

                // Return the number of bytes we wrote
                lua_pushnumber ( luaVM, ulBytesWritten );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "string", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileFlush ( lua_State* luaVM )
{
    // string fileFlush ( file )

    // Check argument 1
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Convert it to a file
            CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

            // Flush the file
            pFile->Flush ();

            // Success. Return true
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileFlush", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileFlush", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileClose ( lua_State* luaVM )
{
    // string fileClose ( file, count )

    // Check argument 1
    if ( argtype ( 1, LUA_TLIGHTUSERDATA ) )
    {
        // Grab the pointer to that file and verify it
        unsigned short usID = reinterpret_cast < unsigned short > ( lua_touserdata ( luaVM, 1 ) );
        CElement* pElement = CElementIDs::GetElement ( usID );
        if ( pElement->GetType () == CElement::SCRIPTFILE )
        {
            // Convert it to a file
            CScriptFile* pFile = reinterpret_cast < CScriptFile* > ( pElement );

            // Close the file and delete it
            pFile->Unload ();
            m_pElementDeleter->Delete ( pFile );

            // Success. Return true
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileClose", "file", 1 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileClose", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}
*/
