/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTextureManagerSA.instance.h
*  PURPOSE:     Internal texture management definitions
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _TEXTUREMANAGER_TXD_INSTANCE_
#define _TEXTUREMANAGER_TXD_INSTANCE_

// Entry in the GTA:SA internal TXD pool
class CTxdInstanceSA    //size: 12 bytes
{
    friend class CTextureManagerSA;
public:
                    CTxdInstanceSA          ( const char *name );
                    ~CTxdInstanceSA         ( void );

    void*   operator new( size_t );
    void    operator delete( void *ptr );

    void            Allocate                ( void );
    void            Deallocate              ( void );

    bool            LoadTXD                 ( RwStream *stream );
    bool            LoadTXD                 ( CFile *file );
    void            InitParent              ( void );

    void            Reference               ( void );
    void            Dereference             ( void );
    void            DereferenceNoDestroy    ( void );

    void            SetCurrent              ( void );

    RwTexDictionary*    m_txd;              // 0
    unsigned short      m_references;       // 4
    unsigned short      m_parentTxd;        // 6
    unsigned int        m_hash;             // 8
};

#endif //_TEXTUREMANAGER_TXD_INSTANCE_