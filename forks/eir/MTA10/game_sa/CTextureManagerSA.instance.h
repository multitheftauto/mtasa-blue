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

    unsigned int    GetHash                 ( void )        { return m_hash; }

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

// Exports of game functions.
int __cdecl TxdCreate                   ( const char *name );
void __cdecl TxdDestroy                 ( int txdIndex );
int __cdecl TxdFind                     ( const char *name );
bool __cdecl TxdLoadBinary              ( int id, RwStream *stream );
int __cdecl TxdLoadEx                   ( int id, const char *name, const char *filename );
int __cdecl TxdLoad                     ( int id, const char *filename );
RwTexDictionary* __cdecl TxdGetRwObject ( int id );
bool __cdecl TxdSetCurrentSafe          ( int id );
void __cdecl TxdSetCurrent              ( int id );
void __cdecl TxdDeallocate              ( int id );
void __cdecl TxdInitParent              ( int id );
void __cdecl TxdAddRef                  ( int id );
void __cdecl TxdRemoveRef               ( int id );
void __cdecl TxdRemoveRefNoDestroy      ( int id );

// Links used by MTA:BLUE
inline int __cdecl CTxdStore_LoadTxd                ( int id, RwStream *stream )        { return TxdLoadBinary( id, stream ); }
inline RwTexDictionary* __cdecl CTxdStore_GetTxd    ( int id )                          { return TxdGetRwObject( id ); }
inline void __cdecl CTxdStore_RemoveTxd             ( int id )                          { return TxdDeallocate( id ); }
inline void __cdecl CTxdStore_RemoveRef             ( int id )                          { return TxdRemoveRef( id ); }
inline void __cdecl CTxdStore_AddRef                ( int id )                          { return TxdAddRef( id ); }

#endif //_TEXTUREMANAGER_TXD_INSTANCE_