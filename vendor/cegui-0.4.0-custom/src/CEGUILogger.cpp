/************************************************************************
    filename:   CEGUILogger.cpp
    created:    21/2/2004
    author:     Paul D Turner
    
    purpose:    Implementation of the Logger class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "CEGUILogger.h"
#include <ctime>
#include <iomanip>

// Start of CEGUI namespace section
namespace CEGUI
{
    /*************************************************************************
        Static Data Definitions
    *************************************************************************/
    // singleton instance pointer
    template<> Logger* Singleton<Logger>::ms_Singleton  = NULL;


    /*************************************************************************
        Constructor
    *************************************************************************/
    Logger::Logger(void) :
            d_level(Standard),
            d_caching(true)
    {
        // create log header
        logEvent("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
        logEvent("+                     Crazy Eddie's GUI System - Event log                    +");
        logEvent("+                          (http://www.cegui.org.uk/)                         +");
        logEvent("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        logEvent("CEGUI::Logger singleton created.");
    }

    /*************************************************************************
        Destructor
    *************************************************************************/
    Logger::~Logger(void)
    {
        if (d_ostream.is_open())
        {
            logEvent((utf8*)"CEGUI::Logger singleton destroyed.");
            d_ostream.close();
        }

    }

    /*************************************************************************
        Logs an event
    *************************************************************************/
    void Logger::logEvent(const String& message, LoggingLevel level /* = Standard */)
    {
        using namespace std;

        time_t  et;
        time(&et);
        tm* etm = localtime(&et);

        if (etm != NULL)
        {
            // clear sting stream
            d_workstream.str("");

            // write date
            d_workstream << setfill('0') << setw(2) << etm->tm_mday << '/' <<
            setfill('0') << setw(2) << 1 + etm->tm_mon << '/' <<
            setw(4) << (1900 + etm->tm_year) << ' ';

            // wite time
            d_workstream << setfill('0') << setw(2) << etm->tm_hour << ':' <<
            setfill('0') << setw(2) << etm->tm_min << ':' <<
            setfill('0') << setw(2) << etm->tm_sec << ' ';

            // write event type code
            switch(level)
            {
            case Errors:
                d_workstream << "(Error)\t";
                break;

            case Standard:
                d_workstream << "(InfL1)\t";
                break;

            case Informative:
                d_workstream << "(InfL2)\t";
                break;

            case Insane:
                d_workstream << "(InfL3)\t";
                break;

            default:
                d_workstream << "(Unkwn)\t";
                break;
            }

            d_workstream << message << endl;

            if (d_caching)
            {
                d_cache.push_back(std::make_pair(d_workstream.str(), level));
            }
            else if (d_level >= level)
            {
                // write message
                d_ostream << d_workstream.str();
                // ensure new event is written to the file, rather than just being buffered.
                d_ostream.flush();
            }
        }
    }

    void Logger::setLogFilename(const String& filename, bool append)
    {
        // close current log file (if any)
        if (d_ostream.is_open())
        {
            d_ostream.close();
        }

        d_ostream.open(filename.c_str(), std::ios_base::out | (append ? std::ios_base::app : std::ios_base::trunc));

        if (!d_ostream)
        {
            throw "Logger::setLogFilename - Failed to open file.";
        }
        
        // initialise width for date & time alignment.
        d_ostream.width(2);

        // write out cahced log strings.
        if (d_caching)
        {
            d_caching = false;

            std::vector<std::pair<String, LoggingLevel> >::iterator iter = d_cache.begin();

            while (iter != d_cache.end())
            {
                if (d_level >= (*iter).second)
                {
                    // write message
                    d_ostream << (*iter).first;
                    // ensure new event is written to the file, rather than just being buffered.
                    d_ostream.flush();
                }

                ++iter;
            }

            d_cache.clear();
        }
    }


    Logger& Logger::getSingleton(void)
    {
        return Singleton<Logger>::getSingleton();
    }

} // End of  CEGUI namespace section
