/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CVectorGraphic.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <svgdocument.h>

using namespace lunasvg;

CVectorGraphic::CVectorGraphic(CClientManager* pManager)
{
    m_pManager = pManager;
    // start
}

bool CVectorGraphic::TestFunc()
{
    SVGDocument* document = new SVGDocument();
    m_svgQueue.emplace_back(document);

    return true;
}

CVectorGraphic::~CVectorGraphic()
{
    for (auto document : m_svgQueue)
        delete document;

    m_svgQueue.clear();
}
