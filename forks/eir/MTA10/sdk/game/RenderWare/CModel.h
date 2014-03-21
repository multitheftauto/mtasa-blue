/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/RenderWare/CModel.h
*  PURPOSE:     RenderWare Model entity export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MODEL_ENTITY_EXPORT_
#define _MODEL_ENTITY_EXPORT_

class CModel : public virtual CRwObject
{
public:
    virtual                         ~CModel             ( void )   {}

    virtual CColModel*              GetCollision        ( void ) = 0;

    virtual CModel*                 Clone               ( void ) const = 0;
    virtual void                    Render              ( void ) = 0;

    typedef void (*importIterCallback_t)( modelId_t id, void *ud );

    virtual unsigned int            GetImportCount      ( void ) const = 0;
    virtual void                    ForAllImports       ( importIterCallback_t cb, void *ud ) = 0;

    virtual bool                    Replace             ( unsigned short id ) = 0;
    virtual bool                    IsReplaced          ( unsigned short id ) const = 0;
    virtual bool                    Restore             ( unsigned short id ) = 0;

    virtual void                    RestoreAll          ( void ) = 0;

    typedef std::list <CRpAtomic*> atomicList_t;
    typedef std::list <CRpLight*> lightList_t;
    typedef std::list <CRwCamera*> cameraList_t;

    virtual const atomicList_t&     GetAtomics          ( void ) const = 0;
    virtual const lightList_t&      GetLights           ( void ) const = 0;
    virtual const cameraList_t&     GetCameras          ( void ) const = 0;
};

#endif //_MODEL_ENTITY_EXPORT_