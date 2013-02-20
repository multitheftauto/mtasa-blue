/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDynamicLibrary.cpp
*  PURPOSE:     Dynamic libraries loader
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CDynamicLibrary::CDynamicLibrary()
{
    // Init
    m_module = NULL;
}

CDynamicLibrary::~CDynamicLibrary()
{
    // Make sure we unload
    Unload();
}

bool CDynamicLibrary::Load( const char *path )
{
    // Unload the previous library
    Unload();

    // Load the new library
#ifdef WIN32
    m_module = LoadLibrary( path );
#else
    m_module = dlopen( path, RTLD_NOW );
    
    // Output error if needed
    if ( !m_module )
        Print( "%s\n", dlerror() );
#endif

    // Return whether we succeeded or not
    return m_module != NULL;
}

void CDynamicLibrary::Unload()
{
    // Got a module?
    if ( m_module == NULL )
        return;

#ifdef _WIN32
    FreeLibrary( m_module );
#else
    dlclose( m_module );
#endif

    // Zero out our library as it's no longer valid
    m_module = NULL;
}

bool CDynamicLibrary::IsLoaded()
{
    return m_module != NULL;
}

void* CDynamicLibrary::GetProcedureAddress( const char *name )
{
    // Got a module?
    if ( m_module == NULL )
        return NULL;

#ifdef _WIN32
    return GetProcAddress( m_module, name );
#else
    const char *error;
    dlerror();

    void *proto = dlsym( m_module, name );

    if ( ( error = dlerror() ) != NULL )
        return NULL;

    return proto;
#endif
}
