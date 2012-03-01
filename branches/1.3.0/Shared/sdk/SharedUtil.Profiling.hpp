/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Profiling.hpp
*  PURPOSE:     Shared stuff which measures things
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    //
    // The input buffer from CStatsEvents is converted into SFrameEvents
    //
    class SFrameEvent
    {
    public:
        SFrameEvent ( eStatEventType type, TIMEUS timeStamp ) : type ( type ), timeStamp ( timeStamp ) {}
        eStatEventType  type;
        TIMEUS          timeStamp;
    };

    class SFrameEventList : public std::vector < SFrameEvent >
    {
    public:
        const char* szEventListName;
    };

    class SFrameSection : public std::map < SString, SFrameEventList >
    {
    public:
        const char* szSectionName;
    };

    class SFrameCollection : public std::map < SString, SFrameSection >
    {
    public:
    };


    //
    // SFrameEvents are converted to SStatItems
    //
    struct SStatItem
    {
        SStatItem () : iCounter (0), fMs(0) {}
        int     iCounter;
        float   fMs;
    };

    class SStatSection : public std::map < std::string, SStatItem >
    {
    public:
    };

    class SStatCollection : public std::map < std::string, SStatSection >
    {
    public:
    };



    ///////////////////////////////////////////////////////////////
    //
    // GetNextUnclockIndex
    //
    // Search forward from supplied index looking for an (unpaired) unclock marker
    //
    ///////////////////////////////////////////////////////////////
    #define INVALID_INDEX -1
    int GetNextUnclockIndex ( std::vector < SFrameEvent >& eventList, int i )
    {
        if ( i < 0 || i >= (int)eventList.size () )
            return INVALID_INDEX;

        SFrameEvent& eventItem = eventList[i];

        if ( eventItem.type == STATS_UNCLOCK )
            return i;

        if ( eventItem.type == STATS_CLOCK )
        {
            // Got a clock event instead, so find matching unclock for that first
            i = GetNextUnclockIndex ( eventList, i + 1 );
            if ( i == INVALID_INDEX )
                return INVALID_INDEX;
            i = GetNextUnclockIndex ( eventList, i + 1 );
            return i;
        }

        return INVALID_INDEX;
    }


    ///////////////////////////////////////////////////////////////
    //
    // ValidateEventList
    //
    // Remove clock/unlcock events that occur inside a clock/unclock
    //
    ///////////////////////////////////////////////////////////////
    void ValidateEventList ( std::vector < SFrameEvent >& eventList )
    {
        // check has even number of events
        if ( eventList.size () & 1 )
            return;         // Error

        // remove recursive Clock/Unclocks
        for( int i = 0 ; i < (int)eventList.size () - 1 ; i += 2 )
        {
            SFrameEvent& startEvent = eventList[i];
            SFrameEvent& endEvent = eventList[i + 1];

            if ( startEvent.type == STATS_CLOCK && endEvent.type == STATS_UNCLOCK )
                continue;   // Fast simple case of unclock following a clock

            if ( startEvent.type != STATS_CLOCK )
                return;     // Error

            int iClockIdx = i;
            int iUnclockIdx = GetNextUnclockIndex( eventList, iClockIdx + 1 );

            if ( iUnclockIdx == INVALID_INDEX )
                return;     // Error

            assert ( iUnclockIdx != iClockIdx + 1 );
            {
                // keep iClockIdx and iUnclockIdx - remove items inbetween
                std::vector<SFrameEvent> ::iterator first = eventList.begin () + iClockIdx + 1;
                std::vector<SFrameEvent> ::iterator last  = eventList.begin () + iUnclockIdx;
                eventList.erase( first, last );
            }
        }
    }


    ///////////////////////////////////////////////////////////////
    //
    // CloseOpenEvents
    //
    // If more clocks than unclocks, add missing unclocks, and add clocks for next frame
    // Returns number of missing unclocks inserted
    //
    ///////////////////////////////////////////////////////////////
    int CloseOpenEvents ( std::vector < SFrameEvent >& eventList, const char* szSection, const char* szName, bool bResetFrame )
    {
        if ( !eventList.empty () && eventList[0].type == STATS_UNCLOCK  )
            ListRemoveIndex ( eventList, 0 );

        int iNumClocks = 0;
        int iNumUnclocks = 0;
        for( uint i = 0 ; i < eventList.size () ; i++ )
        {
            if ( eventList[i].type == STATS_CLOCK )
                iNumClocks++;
            else
            if ( eventList[i].type == STATS_UNCLOCK )
            {
                if ( iNumClocks > 0 )
                    iNumUnclocks++;
            }
        }

        int iExtraClocks = Max ( 0, iNumClocks - iNumUnclocks );

        for( int i = 0 ; i < iExtraClocks ; i++ )
        {
            eventList.push_back ( SFrameEvent ( STATS_UNCLOCK, GetTimeUs () ) );
            if ( !bResetFrame )
                g_StatEvents.Add ( szSection, szName, STATS_CLOCK );
        }

        return iExtraClocks;
    }


    ////////////////////////////////////////////////
    //
    // CStatEvents::CStatEvents
    //
    //
    //
    ////////////////////////////////////////////////
    CStatEvents::CStatEvents ( void )
        : m_bEnabled ( false )
        , m_ItemBuffer ( 0 )
        , m_BufferPos ( 0 )
        , m_BufferPosMax ( 0 )
        , m_BufferPosMaxUsing ( 0 )
        , m_bResetFrame ( false )
    {
        ClearBuffer ( true );
    }


    ///////////////////////////////////////////////////////////////
    //
    // CStatEvents::SetEnabled
    //
    // If changing, clear buffer
    //
    ///////////////////////////////////////////////////////////////
    void CStatEvents::SetEnabled ( bool bEnabled )
    {
        if ( bEnabled != m_bEnabled )
        {
            m_bEnabled = bEnabled;
            ClearBuffer ( false );
        }
    }


    ////////////////////////////////////////////////
    //
    // CStatEvents::ClearBuffer
    //
    // Prep for the next set of samples
    //
    ////////////////////////////////////////////////
    void CStatEvents::ClearBuffer ( bool bCanResize )
    {
        assert ( m_BufferPos <= (int)m_ItemBufferArray.size () );

        if ( bCanResize )
        {
            if ( m_BufferPos == (int)m_ItemBufferArray.size () )
                m_bResetFrame = true;

            // Grow quickly, shrink slowly
            m_BufferPosMax = Max ( m_BufferPos * 2, m_BufferPosMax * 1000 / 1001 );
            m_BufferPosMax = Clamp ( 10000, m_BufferPosMax, 1000000 );
            if ( m_BufferPosMax > (int)m_ItemBufferArray.size ()
                || m_BufferPosMax < (int)m_ItemBufferArray.size () / 4 )
                m_ItemBufferArray.resize ( m_BufferPosMax );
        }

        m_ItemBuffer = m_ItemBufferArray.size () ? &m_ItemBufferArray[0] : NULL;
        m_BufferPos = 0;

        m_BufferPosMaxUsing = m_bEnabled ? m_BufferPosMax : 0;
    }


    ///////////////////////////////////////////////////////////////
    //
    // CStatEvents::Sample
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    void CStatEvents::Sample ( SStatCollection& m_StatCollection )
    {
        // For each clock, make a count
        // For each clock/unclock pair, add the time

        //
        // Put data from input buffer into a SFrameCollection as events
        //
        SFrameCollection frameCollection;

        TIMEUS baseTimeStamp = 0;
        {
            for ( int i = 0 ; i < m_BufferPos ; i++ )
            {
                SItem& item = m_ItemBufferArray[i];
                const char* szSection = item.szSection;
                const char* szName = item.szName;

                if ( baseTimeStamp == 0 )
                    baseTimeStamp = item.timeStamp;

                // Get SFrameSection for section
                SFrameSection& frameSection = MapGet ( frameCollection, szSection );
                frameSection.szSectionName = szSection;

                // Get SFrameEventList for name
                SFrameEventList& frameEvents = MapGet ( frameSection, szName );
                frameEvents.szEventListName = szName;

                // Append event
                frameEvents.push_back ( SFrameEvent ( item.type, item.timeStamp ) );
            }
        }

        // Clear input buffer
        ClearBuffer( true );

        //
        // Put events into a SStatCollection
        //
        m_StatCollection.clear ();

        // For each FrameSection
        std::map <SString,SFrameSection> ::iterator itCollection = frameCollection.begin ();
        for ( ; itCollection != frameCollection.end () ; itCollection++ )
        {
            const SString& strSectionName = itCollection->first;
            SFrameSection& frameSection  = itCollection->second;

            // For each FrameEventList
            std::map <SString,SFrameEventList> ::iterator itSection = frameSection.begin ();
            for ( ; itSection != frameSection.end () ; itSection++ )
            {
                const SString& strItemName = itSection->first;
                SFrameEventList& frameEvents  = itSection->second;

                int iFakeHits = CloseOpenEvents ( frameEvents, frameSection.szSectionName, frameEvents.szEventListName, m_bResetFrame );
                ValidateEventList ( frameEvents );

                int iHitCount = frameEvents.size() / 2 - iFakeHits;
                float fTotalMs = 0;

                // Do each SFrameEvent
                std::vector<SFrameEvent> ::const_iterator itEvents = frameEvents.begin ();
                for ( ; itEvents != frameEvents.end () ; )
                {
                    const SFrameEvent& StartEvent = *itEvents++;
                    if ( itEvents == frameEvents.end () )
                        break;
                    const SFrameEvent& EndEvent = *itEvents++;

                    if ( StartEvent.type != STATS_CLOCK || EndEvent.type != STATS_UNCLOCK )
                    {
                        // Error
                        fTotalMs = 999;
                        continue;
                    }

                    float StartMs   = ( StartEvent.timeStamp - baseTimeStamp ) * ( 1 / 1000.0f );   // TIMEUS to ms
                    float EndMs     = ( EndEvent.timeStamp   - baseTimeStamp ) * ( 1 / 1000.0f );
                    float LengthMs  = EndMs-StartMs;
                    fTotalMs += LengthMs;
                }

                // Add an item into SStatCollection
                SStatSection& section = MapGet ( m_StatCollection, strSectionName );
                SStatItem& item       = MapGet ( section, strItemName );
                item.iCounter   += iHitCount;
                item.fMs        += fTotalMs;
            }    
        }

        if ( m_bResetFrame || m_ResetTimer.Get () > 20000 )
        {
            ClearBuffer ( false );
            m_ResetTimer.Reset ();
            m_bResetFrame = false;
        }
    }



    //
    //
    //
    // CStatResults
    //
    //
    //


    ///////////////////////////////////////////////////////////////
    //
    // CStatResults::CStatResults
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    CStatResults::CStatResults ( void )
    {
        m_fNextMaxClearTime = 0;
    }


    ///////////////////////////////////////////////////////////////
    //
    // CStatResults::FrameEnd
    //
    // Save all stats in a ResultCollection
    //
    ///////////////////////////////////////////////////////////////
    void CStatResults::FrameEnd ( void )
    {
        CLOCK( "Profiling", "Compile stats" );

        SStatResultCollection& collection = m_CollectionCombo;

        bool bClearMax = false;

        float fNextLength = (float)GetSecondCount () - m_fNextMaxClearTime;
        if ( fNextLength >= 2.0f )
        {
            bClearMax = true;
            m_fNextMaxClearTime = (float)GetSecondCount ();  
        }

        // Clear max time thing
        for ( std::map < std::string, SStatResultSection > :: iterator itSection = collection.begin () ; itSection != collection.end () ; itSection++ )
        {
            SStatResultSection& section = itSection->second;

            for ( std::map < std::string, SStatResultItem > :: iterator itItem = section.begin () ; itItem != section.end () ; itItem++ )
            {
                SStatResultItem& item = itItem->second;
                item.iCounter = 0;
                item.fMs = 0;
                if ( bClearMax )
                {
                    item.fMsMax = item.fMsMaxNext;
                    item.fMsMaxNext = 0;
                    item.fMsTotal = item.fMsTotalAcc;
                    item.fMsTotalPercent = item.fMsTotalAcc / fNextLength * 0.1f;
                    item.fMsTotalAcc = 0;
                    item.iCounterTotal = item.iCounterTotalAcc;
                    item.iCounterTotalAcc = 0;
                }
            }
        }

        // Retrieve stats from g_StatEvents
        {
            SStatCollection collectionSrc;
            g_StatEvents.Sample ( collectionSrc );
            const SStatCollection* pCollectionSrc = &collectionSrc;

            // Merge collections

            // Merge section maps
            for ( std::map < std::string, SStatSection > :: const_iterator itSectionSrc = pCollectionSrc->begin () ; itSectionSrc != pCollectionSrc->end () ; itSectionSrc++ )
            {
                const std::string& strSectionNameSrc = itSectionSrc->first;
                const SStatSection& sectionSrc = itSectionSrc->second;

                // Merge sections
                SStatResultSection& sectionCombo = MapGet ( m_CollectionCombo, strSectionNameSrc );

                // Merge item maps
                for ( std::map < std::string, SStatItem > :: const_iterator itItemSrc = sectionSrc.begin () ; itItemSrc != sectionSrc.end () ; itItemSrc++ )
                {
                    const std::string& strItemNameSrc = itItemSrc->first;
                    const SStatItem& itemSrc = itItemSrc->second;

                    // Merge item
                    SStatResultItem& itemCombo = MapGet ( sectionCombo, strItemNameSrc );
                    itemCombo.iCounter += itemSrc.iCounter;
                    itemCombo.fMs      += itemSrc.fMs;
                }
            }
        }

        // Update some counters and stuff
        for ( std::map < std::string, SStatResultSection > :: iterator itSection = collection.begin () ; itSection != collection.end () ; itSection++ )
        {
            SStatResultSection& section = itSection->second;

            for ( std::map < std::string, SStatResultItem > :: iterator itItem = section.begin () ; itItem != section.end () ; itItem++ )
            {
                SStatResultItem& item = itItem->second;
                item.fMsMaxNext         = Max ( item.fMsMaxNext, item.fMs );
                item.fMsTotalAcc        += item.fMs;
                item.iCounterTotalAcc   += item.iCounter;
            }
        }

        UNCLOCK( "Profiling", "Compile stats" );
    }

}
