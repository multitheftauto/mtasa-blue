/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CVectorGraphic.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <svgdocument.h>

using namespace lunasvg;

class CVectorGraphic
{
public:
    ZERO_ON_NEW
    CVectorGraphic(CClientManager* pManager);
    ~CVectorGraphic();

    bool TestFunc();
private:
    CClientManager*             m_pManager;
    std::vector<SVGDocument*>   m_svgQueue;
};
