/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwCameraSA.h
*  PURPOSE:     RenderWare RwCamera export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_CAMERA_SA_
#define _RW_CAMERA_SA_

class CRwCameraSA : public virtual CRwCamera, public CRwObjectSA
{
    friend class CModelSA;
public:
                                    CRwCameraSA         ( RwCamera *cam );
                                    ~CRwCameraSA        ( void );

    inline RwCamera*                GetObject           ( void )                            { return (RwCamera*)m_object; }
    inline const RwCamera*          GetObject           ( void ) const                      { return (const RwCamera*)m_object; }

    eRwType                         GetType             ( void ) const                      { return RW_CAMERA; }
    bool                            IsFrameExtension    ( void ) const                      { return true; }

    void                            AddToModel          ( CModel *model );
    CModel*                         GetModel            ( void );
    void                            RemoveFromModel     ( void );

    void                            SetRenderSize       ( int width, int height );
    void                            GetRenderSize       ( int& width, int& height ) const;

    bool                            BeginUpdate         ( void );
    bool                            IsRendering         ( void ) const                      { return m_isRendering; }
    void                            EndUpdate           ( void );

private:
    void                            DestroyBuffers      ( void );

    CModelSA*   m_model;
    bool        m_isRendering;
};

#endif //_RW_CAMERA_SA_