/************************************************************************
	filename: 	CEGUILogger.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for the Logger class
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
#ifndef _CEGUILogger_h_
#define _CEGUILogger_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "CEGUISingleton.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4275)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Enumeration of logging levels
*/
enum LoggingLevel
{
	Errors,			//!< Only actual error conditions will be logged.
	Standard,		//!< Basic events will be logged (default level).
	Informative,	//!< Useful tracing (object creations etc) information will be logged.
	Insane			//!< Mostly everything gets logged (use for heavy tracing only, log WILL be big).
};

/*!
\brief
	Class that implements logging for the GUI system
*/
class CEGUIEXPORT Logger : public Singleton <Logger>
{
public:
	/*!
	\brief
		Constructor for Logger object.
	*/
	Logger(void);

	/*!
	\brief Destructor for Logger object.
	*/
	~Logger(void);

	
	/*!
	\brief
		Return the singleton Logger object

	\return
		Reference to the one and only Logger object
	*/
	static Logger&	getSingleton(void);


	/*!
	\brief
		Set the level of logging information that will get out to the log file

	\param level
		One of the LoggingLevel enumerated values that specified the level of logging information required.

	\return
		Nothing
	*/
	void	setLoggingLevel(LoggingLevel level)		{d_level = level;}


	/*!
	\brief
		return the current logging level setting

	\return
		One of the LoggingLevel enumerated values specifying the current level of logging
	*/
	LoggingLevel	getLoggingLevel(void) const		{return d_level;}


	/*!
	\brief
		Add an event to the log.

	\param message
		String object containing the message to be added to the event log.

	\param level
		LoggingLevel for this message.  If \a level is greater than the current set logging level, the message is not logged.

	\return
		Nothing
	*/
	void	logEvent(const String& message, LoggingLevel level = Standard);

    /*!
    \brief
        Set the name of the log file where all subsequent log entries should be written.

    \note
        When this is called, and the log file is created, any cached log entries are
        flushed to the log file.

    \param filename
        Name of the file to put log messages.

    \param append
        - true if events should be added to the end of the current file.
        - false if the current contents of the file should be discarded.
     */
    void    setLogFilename(const String& filename, bool append = false);

protected:
    /*************************************************************************
		Implementation Data
	*************************************************************************/
	LoggingLevel	d_level;		//!< Holds current logging level
	std::ofstream	d_ostream;		//!< Stream used to implement the logger
    std::vector<std::pair<String, LoggingLevel> > d_cache;    //!< Used to cache log entries before log file is created.
    std::ostringstream d_workstream;//!< Used to build log entry strings. 
    bool d_caching;                 //!< true while log entries are beign cached (prior to logfile creation)
    
private:
	/*************************************************************************
		Copy constructor and assignment usage is denied.
	*************************************************************************/
	Logger(const Logger& logger) {}
	Logger& operator=(const Logger& logger) {return *this;}

};

/*************************************************************************
	This macro is used for 'Insane' level logging so that those items are
	excluded from non-debug builds
*************************************************************************/
#if defined(DEBUG) || defined (_DEBUG)
#	define CEGUI_LOGINSANE( message ) CEGUI::Logger::getSingleton().logEvent((message), CEGUI::Insane);
#else
#	define CEGUI_LOGINSANE( message )
#endif

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUILogger_h_
