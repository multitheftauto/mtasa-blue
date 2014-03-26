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

    virtual imports_t               GetImportList   ( void ) const = 0;

    virtual bool                    Replace         ( modelId_t id ) = 0;
    virtual bool                    IsReplaced      ( modelId_t id ) const = 0;
    virtual bool                    Restore         ( modelId_t id ) = 0;
    virtual void                    RestoreAll      ( void ) = 0;
};

#endif
