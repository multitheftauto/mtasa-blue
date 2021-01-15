/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientVectorGraphic.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <svgdocument.h>

using namespace lunasvg;

CClientVectorGraphic::CClientVectorGraphic(CClientManager* pManager, ElementID ID, CVectorGraphicItem* pVectorGraphicItem)
    : ClassInit(this), CClientTexture(pManager, ID, pVectorGraphicItem)
{
    m_pResource = nullptr;
    m_pManager = pManager;

    m_pDocument = CreateDocument();
}

SVGDocument* CClientVectorGraphic::CreateDocument()
{
    SVGDocument* document = new SVGDocument();

    return document;
}

CClientVectorGraphic::~CClientVectorGraphic()
{
    delete m_pDocument;
    m_pDocument = nullptr;
}
