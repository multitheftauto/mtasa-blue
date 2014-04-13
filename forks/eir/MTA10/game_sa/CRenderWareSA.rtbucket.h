/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtbucket.h
*  PURPOSE:     Bucket rendering optimizations using rendering metadata
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_BUCKET_OPTIMIZATIONS_
#define _RENDERWARE_BUCKET_OPTIMIZATIONS_

namespace RenderBucket
{
    struct renderSystemState
    {
        // All variables here have to declare the state that is required for rendering.
        DWORD                           renderStates[210];
        DWORD                           textureStageStates[8][33];
        D3DLIGHT9                       lightStates[8];
        D3DMATRIX                       transformations[260];
        D3DMATERIAL9                    materialState;
        IDirect3DVertexBuffer9*         vertexBuffer;
        IDirect3DVertexDeclaration9*    vertexDeclaration;
        DWORD                           fvf;
    };

    struct RwRenderBucket
    {
        RwRenderBucket( void );
        ~RwRenderBucket( void );
    };
};

/*
    A render bucket system is an optimization routine based on metadata collection
    that assumes the collected data is invariant.
*/
namespace RenderBucket
{
    void                    Initialize( void );
    void                    Shutdown( void );

    void                    Begin( void );
    void                    End( void );

    void                    BeginPass( void );
    void                    EndPass( void );

    RwRenderBucket*         AllocateRenderBucket( void );
    RwRenderBucket*         FindRenderBucket( void );
};

#endif //_RENDERWARE_BUCKET_OPTIMIZATIONS_