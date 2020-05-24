/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientGuiFont.h
 *  PURPOSE:     Custom font bucket
 *
 *****************************************************************************/

class CClientGuiFont : public CClientRenderElement
{
    DECLARE_CLASS(CClientGuiFont, CClientRenderElement)
public:
    CClientGuiFont(CClientManager* pManager, ElementID ID, CGuiFontItem* pFontItem);
    ~CClientGuiFont();

    eClientEntityType GetType() const { return CCLIENTGUIFONT; }
    void              Unlink();

    // CClientGuiFont methods
    CGuiFontItem*  GetGuiFontItem() { return (CGuiFontItem*)m_pRenderItem; }
    const SString& GetCEGUIFontName();
    void           NotifyGUIElementAttach(CClientGUIElement* pGUIElement);
    void           NotifyGUIElementDetach(CClientGUIElement* pGUIElement);

protected:
    std::set<CClientGUIElement*> m_GUIElementUserList;
};
