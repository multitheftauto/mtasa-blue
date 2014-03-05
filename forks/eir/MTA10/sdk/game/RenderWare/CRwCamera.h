/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/RenderWare/CRwCamera.h
*  PURPOSE:     RenderWare RwCamera modification export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_CAMERA_
#define _RW_CAMERA_

class CRwCamera : public virtual CRwObject
{
public:
    virtual                         ~CRwCamera          ( void )    {}

    virtual void                    AddToModel          ( CModel *model ) = 0;
    virtual CModel*                 GetModel            ( void ) = 0;
    virtual void                    RemoveFromModel     ( void ) = 0;

    virtual void                    SetRenderSize       ( int width, int height ) = 0;
    virtual void                    GetRenderSize       ( int& width, int& height ) const = 0;

    virtual bool                    BeginUpdate         ( void ) = 0;
    virtual bool                    IsRendering         ( void ) const = 0;
    virtual void                    EndUpdate           ( void ) = 0;
};

#endif //_RW_CAMERA_