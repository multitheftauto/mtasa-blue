/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CDuplicateLineFilter.h    
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

template < class T >
class CDuplicateLineFilter
{
public:
    struct SOutputLine : public T
    {
                SOutputLine ( const T& line, uint uiCount ) : T( line ), uiDupCount( uiCount ) {}
        uint    uiDupCount;
    };

    CDuplicateLineFilter( uint uiMaxNumOfLinesInMatch = 4, uint uiMaxDelaySeconds = 5 )
    {
        m_uiMaxNumOfLinesInMatch = uiMaxNumOfLinesInMatch;
        m_uiMaxDelaySeconds = uiMaxDelaySeconds;
        m_bIsMatching = false;
    }

    //////////////////////////////////////////////////////////
    //
    // AddLine
    //
    //////////////////////////////////////////////////////////
    void AddLine( const T& line )
    {
        if ( m_bIsMatching == false )
        {
            // Not currently matching
            // Search history for first line of new match
            for( uint i = 0 ; i < m_History.size(); i++ )
            {
                if ( line == m_History.at( i ) )
                {
                    // Begin match
                    m_bIsMatching = true;
                    m_uiMatchSize = i + 1;
                    m_uiMatchCurLine = i;
                    m_uiDupCount = 0;
                    if ( m_uiMatchCurLine == 0)
                        m_uiDupCount++; // Completed matching set (will only occur here for single line match)
                    return;
                }
            }
        }
        else
        {
            // Is currently matching
            // Check if match will continue
            uint uiNextLine = ( m_uiMatchCurLine - 1 + m_uiMatchSize ) % m_uiMatchSize;
            if ( line == m_History.at( uiNextLine ) )
            {
                // Still matching
                m_uiMatchCurLine = uiNextLine;
                if ( m_uiMatchCurLine == 0)
                    m_uiDupCount++;     // Completed matching set
                return;
            }
            else
            {
                // Match finished
                Flush();
            }
        }

        // Not matching
        // Update history
        m_History.push_front( line );
        while( m_History.size() > m_uiMaxNumOfLinesInMatch )
            m_History.pop_back();
        // Push line to output
        AddLineToOutputBuffer( line );
    }


    //////////////////////////////////////////////////////////
    //
    // Flush
    //
    // Finish current match and push relevant lines to the output buffer
    //
    //////////////////////////////////////////////////////////
    void Flush( void )
    {
        if ( m_bIsMatching == false )
            return;
        m_bIsMatching = false;

        // Handle full set matches
        if ( m_uiDupCount > 0 )
        {
            for( uint i = 0 ; i < m_uiMatchSize; i++ )
            {
                AddLineToOutputBuffer( m_History.at( m_uiMatchSize - 1 - i ), m_uiDupCount );
            }
        }

        // Handle partial set matches
        if ( m_uiMatchCurLine > 0 )
        {
            for( uint i = 0 ; i < m_uiMatchSize - m_uiMatchCurLine ; i++ )
            {
                AddLineToOutputBuffer( m_History.at( m_uiMatchSize - 1 - i ) );
            }
        }
        m_History.clear();
    }


    //////////////////////////////////////////////////////////
    //
    // AddLineToOutputBuffer
    //
    //////////////////////////////////////////////////////////
    void AddLineToOutputBuffer( const T& line, uint uiDupCount = 0 )
    {
        m_PendingOutput.push_back( SOutputLine( line, uiDupCount ) );
    }


    //////////////////////////////////////////////////////////
    //
    // PopOutputLine
    //
    // Returns true if has populated values
    //
    //////////////////////////////////////////////////////////
    bool PopOutputLine( T& outLine )
    {
        if ( m_PendingOutput.empty() )
        {
            if ( time( NULL ) - m_tLastOutputTime > m_uiMaxDelaySeconds )
                Flush();
            if ( m_PendingOutput.empty() )
                return false;
        }
        m_tLastOutputTime = time( NULL );

        const SOutputLine& line = m_PendingOutput.front();
        outLine = line;
        if ( line.uiDupCount > 1 )
            static_cast < SString& > ( outLine ) += SString( "  [DUP x%u]", line.uiDupCount );
        m_PendingOutput.pop_front();
        return true;
    }

protected:
    std::deque < T >            m_History;
    std::list < SOutputLine >   m_PendingOutput;
    bool                        m_bIsMatching;              // true if matching a duplicate set
    uint                        m_uiMatchSize;              // Number of lines in active match
    uint                        m_uiMatchCurLine;           // Current line in active match
    uint                        m_uiDupCount;               // Number of duplicates found for active match
    time_t                      m_tLastOutputTime;
    uint                        m_uiMaxNumOfLinesInMatch;   // Max number lines in a matching set
    uint                        m_uiMaxDelaySeconds;        // Max seconds to delay outputting duplicated lines
};
