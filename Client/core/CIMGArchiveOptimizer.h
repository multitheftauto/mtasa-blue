#pragma once

class CIMGArchiveOptimizer
{
public:
    CIMGArchiveOptimizer();
    void CreateGUI();
    void SetVisible(bool bVisible);

private:

    bool OnImgGenerateClick(CGUIElement* pElement);

    CGUIElement * m_pWindow;
    CGUILabel*    m_pImgSpaceRequiredLabel;
};