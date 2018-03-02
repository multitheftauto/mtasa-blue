#include <StdInc.h>

CClientIFP::CClientIFP ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    SetTypeName ( "IFP" );
}

CClientIFP::~CClientIFP ( void )
{
    UnloadIFP ( );
}

bool CClientIFP::LoadIFP ( const char* szFilePath, const char* szBlockName )
{
    printf ("\nCClientIFP::LoadIFP: szFilePath %s\n szBlockName: %s\n\n", szFilePath, szBlockName);

    // Failed
    return false;
}


void CClientIFP::UnloadIFP ( void )
{
    printf ("CClientIFP::UnloadIFP ( ) called!\n");
}
