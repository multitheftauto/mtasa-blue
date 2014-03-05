/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwObjectSA.h
*  PURPOSE:     RenderWare RwObject export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_OBJECT_SA_
#define _RW_OBJECT_SA_

class CRwFrameSA;

class CRwObjectSA : public virtual CRwObject
{
public:
                                    CRwObjectSA         ( RwObject *obj );
    virtual                         ~CRwObjectSA        ( void );

    inline RwObject*                GetObject           ( void )                { return m_object; }
    inline const RwObject*          GetObject           ( void ) const          { return m_object; }

    virtual eRwType                 GetType             ( void ) const = 0;
    virtual bool                    IsFrameExtension    ( void ) const          { return false; }

    CRwFrame*                       GetFrame            ( void );
    const CRwFrame*                 GetFrame            ( void ) const;
    void                            SetFrame            ( CRwFrame *frame );

    CRwFrameSA*         m_frame;

protected:
    RwObject*           m_object;
};

#endif //_RW_OBJECT_SA_