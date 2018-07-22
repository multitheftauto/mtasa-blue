//
// CPollFdSet.h
//
// Array of file descriptors for poll()
//

class CPollFdSet
{
public:
    // FD_ZERO
    void Reset( void )
    {
        m_uiFdCount = 0;
    }

    // FD_SET
    // Assumes the fd has not been added already
    void Add( int fd, short events )
    {
        unsigned int i = m_uiFdCount;
        if ( i < FD_ARRAY_LENGTH )
        {
            m_FdArray[i].fd = fd;
            m_FdArray[i].events = events;
            m_FdArray[i].revents = 0;
            m_uiFdCount = i + 1;
        }
    }

    // FD_ISSET
    // Check if event has been triggered for the fd
    bool IsSet( int fd, short events ) const
    {
        for( unsigned int i = 0 ; i < m_uiFdCount ; i++ )
        {
            if ( m_FdArray[i].fd == fd &&
                (m_FdArray[i].events & events) != 0 )
            {
                return true;
            }
        }
        return false;
    }

    unsigned int GetFdCount( void ) const
    {
        return m_uiFdCount;
    }

    pollfd* GetFdArray( void )
    {
        return m_FdArray;
    }

protected:
    const static int    FD_ARRAY_LENGTH = 2048;
    unsigned int        m_uiFdCount;
    pollfd              m_FdArray[FD_ARRAY_LENGTH];
};
