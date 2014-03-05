/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/RenderWare/CTexture.h
*  PURPOSE:     RenderWare Texture GTA:SA export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTexture_
#define _CTexture_

class CTexDictionary;

class CTexture
{
public:
    virtual                         ~CTexture       ( void )   {};

    virtual const char*             GetName         ( void ) const = 0;
    virtual unsigned int            GetHash         ( void ) const = 0;

    virtual void                    SetFiltering    ( bool filter ) = 0;
    virtual bool                    IsFiltering     ( void ) const = 0;

    virtual void                    SetTXD          ( CTexDictionary *txd ) = 0;
    virtual void                    RemoveFromTXD   ( void ) = 0;
    virtual CTexDictionary*         GetTXD          ( void ) = 0;

    virtual bool                    Import          ( unsigned short id ) = 0;
    virtual bool                    ImportTXD       ( unsigned short id ) = 0;
    virtual bool                    Remove          ( unsigned short id ) = 0;
    virtual bool                    RemoveTXD       ( unsigned short id ) = 0;

    virtual void                    ClearImports    ( void ) = 0;

    virtual bool                    IsImported      ( unsigned short id ) const = 0;
    virtual bool                    IsImportedTXD   ( unsigned short id ) const = 0;

    virtual bool                    IsUsed          ( void ) const = 0;
};

#endif //_CTexture_