/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/RenderWare/CTexDictionary.h
*  PURPOSE:     Texture Dictionary GTA:SA export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTexDictionary_
#define _CTexDictionary_

class CTexDictionary abstract : public virtual CRwObject
{
public:
    virtual                         ~CTexDictionary     ( void )   {};

    virtual std::list <CTexture*>&  GetTextures         ( void ) = 0;
    virtual void                    Clear               ( void ) = 0;

    virtual void                    SetGlobalEmitter    ( void ) = 0;

    virtual bool                    Import              ( unsigned short id ) = 0;
    virtual bool                    ImportTXD           ( unsigned short id ) = 0;
    virtual bool                    Remove              ( unsigned short id ) = 0;
    virtual bool                    RemoveTXD           ( unsigned short id ) = 0;

    virtual void                    ClearImports        ( void ) = 0;

    virtual bool                    IsImported          ( unsigned short id ) const = 0;
    virtual bool                    IsImportedTXD       ( unsigned short id ) const = 0;

    virtual bool                    IsUsed              ( void ) const = 0;

    typedef std::list <unsigned short> importList_t;

    virtual const importList_t&     GetImportedList     ( void ) const = 0;
};

#endif //_CTexDictionary_