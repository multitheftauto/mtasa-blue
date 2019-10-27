/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientPrimitiveBufferInterface.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CClientPrimitiveBufferInterface
{
public:
    virtual void PreDraw() = 0;
    virtual void Draw(PrimitiveBufferSettings& settings) = 0;

};
