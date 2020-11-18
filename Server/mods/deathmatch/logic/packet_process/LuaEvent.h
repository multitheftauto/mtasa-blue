void CGame::Packet_LuaEvent(CLuaEventPacket& Packet)
{
    // Grab the source player, even name, element id and the arguments passed
    CPlayer*       pCaller = Packet.GetSourcePlayer();
    const char*    szName = Packet.GetName();
    ElementID      ElementID = Packet.GetElementID();
    CLuaArguments* pArguments = Packet.GetArguments();

    // Grab the element
    CElement* pElement = CElementIDs::GetElement(ElementID);
    if (pElement)
    {
        // Make sure the event exists and that it allows clientside triggering
        SEvent* pEvent = m_Events.Get(szName);
        if (pEvent)
        {
            if (pEvent->bAllowRemoteTrigger)
            {
                pElement->CallEvent(szName, *pArguments, pCaller);
            }
            else
                m_pScriptDebugging->LogError(NULL, "Client (%s) triggered serverside event %s, but event is not marked as remotly triggerable",
                                             pCaller->GetNick(), szName);
        }
        else
            m_pScriptDebugging->LogError(NULL, "Client (%s) triggered serverside event %s, but event is not added serverside", pCaller->GetNick(), szName);
    }
}