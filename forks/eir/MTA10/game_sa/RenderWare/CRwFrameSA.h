/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRwFrameSA.h
*  PURPOSE:     RenderWare RwFrame export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_FRAME_SA_
#define _RW_FRAME_SA_

class CRwFrameSA : public virtual CRwFrame, public CRwObjectSA
{
    friend class CRwObjectSA;
public:
                                    CRwFrameSA      ( RwFrame *obj );
                                    ~CRwFrameSA     ( void );

    inline RwFrame*                 GetObject       ( void )                        { return (RwFrame*)m_object; }
    inline const RwFrame*           GetObject       ( void ) const                  { return (const RwFrame*)m_object; }

    void                            SetName         ( const char *name )            { strncpy( GetObject()->szName, name, 31 ); }
    const char*                     GetName         ( void ) const                  { return GetObject()->szName; }
    unsigned int                    GetHash         ( void ) const;
    eRwType                         GetType         ( void ) const                  { return RW_NULL; }

    void                            Link            ( CRwFrame *child );
    void                            Unlink          ( void );

    objectList_t&                   GetObjects      ( void )                        { return m_objects; }
    childList_t&                    GetChildren     ( void )                        { return m_children; }

    void                            SetLTM          ( const RwMatrix& mat )         { GetObject()->ltm = mat; }   // todo: synchronize objects and children frames.
    const RwMatrix&                 GetLTM          ( void ) const                  { return GetObject()->ltm; }

    void                            SetModelling    ( const RwMatrix& mat )         { GetObject()->SetModelling( mat ); }
    const RwMatrix&                 GetModelling    ( void ) const                  { return GetObject()->GetModelling(); }

    void                            SetPosition     ( const CVector& pos )          { GetObject()->SetPosition( pos ); }
    const CVector&                  GetPosition     ( void ) const                  { return GetObject()->GetPosition(); }

    childList_t                     m_children;

    // Static accessors
    static RwObject* RwFrameObjectAssign( RwObject *obj, CRwFrameSA *parent );

private:
    objectList_t                    m_objects;
};

#endif //_RW_FRAME_SA_