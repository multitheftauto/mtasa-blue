/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTextItem.h
*  PURPOSE:     Text display item class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTextItem;

#ifndef __CTEXTITEM_H
#define __CTEXTITEM_H

#include <CVector2D.h>
#include "CTextDisplay.h"
#include <list>

enum eTextPriority 
{
    PRIORITY_LOW = 0,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
};

class CTextItem
{
    friend class CTextDisplay;
    friend class CPlayerTextManager;

public:
                            CTextItem               ( const char* szText, const CVector2D& vecPosition, eTextPriority Priority = PRIORITY_LOW, unsigned char ucRed = 255, unsigned char ucGreen = 255, unsigned char ucBlue = 255, unsigned char ucAlpha = 255, float fScale = 1.0f, unsigned char ucFormat = 0 );
                            CTextItem               ( const CTextItem& TextItem );
                            ~CTextItem              ( void );

    bool                    operator=               ( const CTextItem& TextItem );

    void                    SetText                 ( const char* szText );
    char*                   GetText                 ( char* pBuffer, size_t bufferSize );

    inline const CVector2D& GetPosition             ( void )                            { return m_vecPosition; };
    void                    SetPosition             ( const CVector2D& vecPosition );

    void                    SetColor                ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha );
    void                    GetColor                ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha );

    inline float            GetScale                ( void )                            { return m_fScale; };
    void                    SetScale                ( float fScale );

    inline unsigned char    GetFormat               ( void )                            { return m_ucFormat; }
    inline void             SetFormat               ( unsigned char ucFormat )          { m_ucFormat = ucFormat; }

    inline void             SetPriority             ( eTextPriority Priority )          { m_Priority = Priority; };
    inline eTextPriority    GetPriority             ( void )                            { return m_Priority; };

    inline unsigned long    GetID                   ( void )                            { return m_ulUniqueId; };
    inline bool             IsBeingDeleted          ( void )                            { return m_bDeletable; };

private:
    char *                  m_szText;
    CVector2D               m_vecPosition;
    unsigned char           m_ucRed;
    unsigned char           m_ucGreen;
    unsigned char           m_ucBlue;
    unsigned char           m_ucAlpha;
    float                   m_fScale;
    unsigned char           m_ucFormat;
    unsigned long           m_ulUniqueId;
    eTextPriority           m_Priority;
    bool                    m_bDeletable;

    list <CTextDisplay *>   m_Observers;
    int                     m_iUsageCount;  // the number of CLIENTS that can see this. 
                                            // Used when deleting to make sure it isn't destroyed until
                                            // all clients have been informed

    void                    AddObserver             ( CTextDisplay* pObserver );
    inline void             RemoveObserver          ( CTextDisplay* pObserver )   { if ( !m_Observers.empty() ) m_Observers.remove ( pObserver ); };
    void                    NotifyObservers         ( void );

};

#endif
