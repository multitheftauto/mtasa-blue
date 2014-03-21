/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CModelManager.h
*  PURPOSE:     RenderWare Model entity export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MODEL_MANAGER_EXPORT_
#define _MODEL_MANAGER_EXPORT_

typedef void (__cdecl*modelRequestCallback_t)( unsigned short id );
typedef void (__cdecl*modelFreeCallback_t)( unsigned short id );

class CModelManager
{
public:
#ifndef _MTA_BLUE
    virtual CModel*                     CreateModel         ( CFile *file, modelId_t model ) = 0;
#else
    virtual CModel*                     CreateModel         ( const char *filename, modelId_t model, bool loadEmbeddedCollision ) = 0;
#endif
    virtual CModel*                     CloneClump          ( modelId_t model ) = 0;
    virtual CRpAtomic*                  CloneAtomic         ( modelId_t model ) = 0;

    virtual bool                        GetRwModelType      ( modelId_t model, eRwType& type ) const = 0;

    virtual bool                        RestoreModel        ( modelId_t id ) = 0;
    virtual bool                        RestoreCollision    ( modelId_t id ) = 0;

    virtual void                        RestreamByModel     ( modelId_t id ) = 0;
    virtual void                        RestreamByTXD       ( modelId_t id ) = 0;

    virtual void                        SetRequestCallback  ( modelRequestCallback_t callback ) = 0;
    virtual void                        SetFreeCallback     ( modelFreeCallback_t callback ) = 0;
};

#endif //_MODEL_MANAGER_EXPORT_