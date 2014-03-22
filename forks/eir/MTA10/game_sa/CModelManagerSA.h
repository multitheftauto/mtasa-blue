/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelManagerSA.h
*  PURPOSE:     DFF model entity manager
*               RenderWare extension
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MODEL_ENTITY_MANAGER_
#define _MODEL_ENTITY_MANAGER_

#include <game/CModelManager.h>

class CModelManagerSA : public CModelManager
{
    friend class CModelSA;
    friend class CRenderWareSA;
public:
                                CModelManagerSA     ( void );
                                ~CModelManagerSA    ( void );

#ifndef _MTA_BLUE
    CModelSA*                   CreateModel         ( CFile *file, modelId_t model );
#else
    CModelSA*                   CreateModel         ( const char *filename, modelId_t model, bool loadEmbeddedCollision );
#endif //_MTA_BLUE
    CModelSA*                   CloneClump          ( modelId_t model );
    CRpAtomicSA*                CloneAtomic         ( modelId_t model );

    bool                        GetRwModelType      ( modelId_t model, eRwType& type ) const;

    bool                        RestoreModel        ( modelId_t id );
    bool                        RestoreCollision    ( modelId_t id );

    void                        RestreamByModel     ( modelId_t id );
    void                        RestreamByTXD       ( modelId_t id );

    void                        UpdateWorldTextures ( int txdId );

    void                        SetRequestCallback  ( modelRequestCallback_t callback );
    void                        SetFreeCallback     ( modelFreeCallback_t callback );

protected:
    typedef std::vector <CModelSA*> models_t;
    models_t                m_models;
};

namespace ModelManager
{
    // Callbacks for model info management.
    extern modelRequestCallback_t modelRequestCallback;
    extern modelFreeCallback_t modelFreeCallback;
};

#endif //_MODEL_ENTITY_MANAGER_