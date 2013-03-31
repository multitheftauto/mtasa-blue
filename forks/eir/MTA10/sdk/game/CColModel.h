/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CColModel.h
*  PURPOSE:     Collision model entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_COLMODEL
#define __CGAME_COLMODEL

class CColModelSAInterface;

class CColModel
{
public:
    virtual                         ~CColModel      ( void )    {}

    typedef std::vector <unsigned short> imports_t;

    virtual const imports_t&        GetImportList   ( void ) const = 0;

    virtual bool                    Replace         ( unsigned short id ) = 0;
    virtual bool                    IsReplaced      ( unsigned short id ) const = 0;
    virtual bool                    Restore         ( unsigned short id ) = 0;
    virtual void                    RestoreAll      ( void ) = 0;
};

#endif
