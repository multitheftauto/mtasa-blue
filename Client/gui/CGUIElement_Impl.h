/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIElement.h>

class CGUI_Impl;

class CGUIElement_Impl : public CGUIElement
{
public:
    CGUIElement_Impl();
    ~CGUIElement_Impl();

    void Begin();
    void End();

    void SetPosition(CVector2D pos);
    void SetSize(CVector2D size);

    CVector2D GetInitialPosition() const;
    CVector2D GetInitialSize() const;

    void ProcessPosition();
    void ProcessSize();

    bool IsDeleted();

protected:
    CGUI_Impl* m_pManager = nullptr;

    std::string m_title = {};
    std::string m_uid;

    CVector2D m_initialPosition = {};
    CVector2D m_initialSize = {};

    CVector2D m_pendingPosition = {};
    CVector2D m_pendingSize = {};

    bool m_initialized = false;
    bool m_deleted = false;

    bool m_updatePosition = false;
    bool m_updateSize = false;
};
