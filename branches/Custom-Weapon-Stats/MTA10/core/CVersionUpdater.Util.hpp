/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVersionUpdater
*  PURPOSE:     Version update check and message dialog class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVersionUpdater;

// Update master info
#define UPDATER_MASTER_URL1 "http://updatesa.mtasa.com/sa/master/?v=%VERSION%&id=%ID%"
#define UPDATER_MASTER_URL2 "http://updatesa.multitheftauto.com/sa/master/?v=%VERSION%&id=%ID%"

/*

//
// Exmaple of master file:
//
<masterroot>
    <master>
        <revision>2010-09-05-b</revision>
        <serverlist>
            <server>http://updatesa.mtasa.com/sa/master/?v=%VERSION%&amp;id=%ID%</server>
            <server>http://updatesa.multitheftauto.com/sa/master/?v=%VERSION%&amp;id=%ID%</server>
        </serverlist>
        <interval>7d</interval>
    </master>
    <version>
        <serverlist>
            <server priority="3">http://updatesa.mtasa.com/sa/version/?v=%VERSION%&amp;id=%ID%&amp;ty=%TYPE%&amp;da=%DATA%&amp;be=%BETA%&amp;re=%REFER%</server>
            <server priority="3">http://updatesa.multitheftauto.com/sa/version/?v=%VERSION%&amp;id=%ID%&amp;ty=%TYPE%&amp;da=%DATA%&amp;be=%BETA%&amp;re=%REFER%</server>
        </serverlist>
        <interval>12h</interval>
    </version>
    <report>
        <serverlist>
            <server>http://updatesa.mtasa.com/sa/report/?v=%VERSION%&amp;id=%ID%</server>
            <server>http://updatesa.multitheftauto.com/sa/report/?v=%VERSION%&amp;id=%ID%</server>
        </serverlist>
        <interval>12h</interval>
    </report>
    <crashdump>
        <serverlist>
            <server priority="3">http://updatesa.mtasa.com/sa/crashdump/?v=%VERSION%&amp;id=%ID%&amp;file=%FILE%</server>
            <server priority="4">http://updatesa.multitheftauto.com/sa/crashdump/?v=%VERSION%&amp;id=%ID%&amp;file=%FILE%</server>
        </serverlist>
        <interval>12h</interval>
        <duplicates>0</duplicates>
        <maxhistorylength>100</maxhistorylength>
    </crashdump>
    <gtadatafiles>
        <serverlist>
            <server priority="4">http://updatesa.mtasa.com/sa/gtadatafiles/?v=%VERSION%&amp;id=%ID%</server>
            <server priority="3">http://updatesa.multitheftauto.com/sa/gtadatafiles/?v=%VERSION%&amp;id=%ID%</server>
        </serverlist>
    </gtadatafiles>
    <trouble>
        <serverlist>
            <server>http://updatesa.mtasa.com/sa/trouble/?v=%VERSION%&amp;id=%ID%&amp;tr=%TROUBLE%</server>
        </serverlist>
    </trouble>
</masterroot>



//
// Exmaple of version check file:
//
<versioncheckroot>
    <status>update</status>
    <priority>notmandatory</priority>
    <dialog>
        <title>NEW VERSION AVAILABLE</title>
        <msg>There is an update for the version of MTA you are using.

DO YOU WANT TO DOWNLOAD AND INSTALL THE LATEST VERSION ?
        </msg>
        <msg2>Press OK to install the update.

        </msg2>
        <yes>Yes</yes>
        <no>No</no>
    </dialog>
    <file>
        <name>mtasa-1.0.4-rc-2021-6-files-net.exe</name>
        <size>213857</size>
        <md5>1585F5C5FDF77AD3565A75608E0263DD</md5>
    </file>
    <serverlist>
        <server priority="3">http://updatesa1.mtasa.com/mirror/mtasa-1.0.4-rc-2021-6-files-net.exe</server>
        <server priority="3">http://updatesa2.mtasa.com/mirror/mtasa-1.0.4-rc-2021-6-files-net.exe</server>
        <server priority="2">http://updatesa3.mtasa.com/mirror/mtasa-1.0.4-rc-2021-6-files-net.exe</server>
    </serverlist>
    <reportsettings>
        <filter>1000-9999</filter>
        <minsize>10</minsize>
        <maxsize>4000</maxsize>
    </reportsettings>
    <notifymasterrevision>2010-09-05-b</notifymasterrevision>
</versioncheckroot>

*/

namespace
{

    enum
    {
        RES_FAIL,
        RES_OK,
        RES_POLL,
        RES_CANCEL,
    };

    enum
    {
        BUTTON_NONE = -1,
        BUTTON_0    = 0,
        BUTTON_1,
        BUTTON_2,
        BUTTON_3,
    };

    CQuestionBox& GetQuestionBox ( void )
    {
        return *CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    }

}

namespace
{

    ///////////////////////////////////////////////////////////////
    //
    // CValue
    //
    // Value which can be stored/restored using a string
    //
    ///////////////////////////////////////////////////////////////
    class CValue
    {
    public:
        virtual bool        SetFromString   ( const SString& str ) = 0;
        virtual SString     ToString        ( void ) const = 0;
    };


    ///////////////////////////////////////////////////////////////
    //
    // CValueInt
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    class CValueInt : public CValue
    {
        int m_iValue;
    public:
        CValueInt ( int iValue = 0 ) : m_iValue ( iValue ) {}
        void operator= ( int iValue )
        {
            m_iValue = iValue;
        }
        operator int ( void ) const
        {
            return m_iValue;
        }
        virtual bool SetFromString ( const SString& str )
        {
            m_iValue = atoi ( str );
            return true;
        }
        virtual SString ToString ( void ) const
        {
            return SString ( "%d", m_iValue );
        }
    };


    ///////////////////////////////////////////////////////////////
    //
    // CTimeSpan
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    class CTimeSpan : public CValue
    {
        time_t m_Seconds;
    public:
        CTimeSpan ( void ) : m_Seconds ( 0 ) {}
        CTimeSpan ( const SString& strSpan ) { SetFromString ( strSpan ); }

        void SetFromSeconds ( time_t seconds )
        {
            m_Seconds = seconds;
        }

        bool SetFromString ( const SString& strSpan )
        {
            int iSpan = atoi ( strSpan );
            // Last character ie either s(seconds) m(minutes) h(hours) d(days)
            switch ( strSpan.Right ( 1 ).ToLower ()[0] )
            {
                case 's': m_Seconds = iSpan; break;
                case 'm': m_Seconds = iSpan * 60; break;
                case 'h': m_Seconds = iSpan * 60 * 60; break;
                case 'd': m_Seconds = iSpan * 60 * 60 * 24; break;
                default:
                    m_Seconds = 0;
                    return false;
            }
            return true;
        }

        time_t ToSeconds ( void ) const
        {
            return m_Seconds;
        }

        SString ToString ( void ) const
        {
            if ( m_Seconds < 60 )
                return SString ( "%ds", m_Seconds );
            if ( m_Seconds < 60 * 60 )
                return SString ( "%dm", m_Seconds / 60 );
            if ( m_Seconds < 60 * 60 * 24 )
                return SString ( "%dh", m_Seconds / 60 / 60 );
            return SString ( "%dd", m_Seconds / 60 / 60 / 24 );
        }
    };


    ///////////////////////////////////////////////////////////////
    //
    // CDateTime
    //
    // Manipulate dates (slowly)
    //
    ///////////////////////////////////////////////////////////////
    class CDateTime : public CValue
    {
        time_t m_Seconds;
    public:
        CDateTime ( void ) : m_Seconds ( 0 ) {}
        CDateTime ( const SString& strDate ) { SetFromString ( strDate ); }

        CDateTime operator - ( const CDateTime& other ) const
        {
            CDateTime result;
            result.SetFromSeconds ( ToSeconds () - other.ToSeconds () );
            return result;
        }

        static CDateTime Now ( bool bLocal = false )
        {
            CDateTime date;
            date.SetFromSeconds ( time ( NULL ) );
            return date;
        }

        void SetFromSeconds ( time_t seconds )
        {
            m_Seconds = seconds;
        }

        bool SetFromString ( const SString& strDate )
        {
            std::vector < int > numbers;

            bool bWasDigit = false;
            // Get first 6 non-digit delimited digits
            for ( uint i = 0 ; i < strDate.length () ; i++ )
            {
                bool bIsDigit = isdigit ( strDate[i] ) != 0;
                if ( bIsDigit && !bWasDigit )
                    numbers.push_back ( atoi ( &strDate[i] ) );
                bWasDigit = bIsDigit;
            }

            if ( numbers.size () < 6 )
                return false;

            tm timeinfo;
            memset ( &timeinfo, 0, sizeof ( timeinfo ) );
            uint i = 0;
            timeinfo.tm_year    = numbers[i++] - 1900;
            timeinfo.tm_mon     = numbers[i++] - 1;
            timeinfo.tm_mday    = numbers[i++];
            timeinfo.tm_hour    = numbers[i++];
            timeinfo.tm_min     = numbers[i++];
            timeinfo.tm_sec     = numbers[i++];

            m_Seconds = mktime ( &timeinfo );
            return true;
        }

        time_t ToSeconds ( void ) const
        {
            return m_Seconds;
        }

        SString ToString ( void ) const
        {
            time_t t = ToSeconds ();
            tm* tmp = gmtime ( &t );
            assert ( tmp );

            char outstr[200] = { 0 };
            strftime ( outstr, sizeof ( outstr ), "%Y-%m-%d %H:%M:%S", tmp );
            return outstr;
        }
    };


    ///////////////////////////////////////////////////////////////
    //
    // CXMLBuffer
    //
    // Simulate non-file based xml document
    //
    ///////////////////////////////////////////////////////////////
    class CXMLBuffer
    {
    public:
        SString m_strTempFileName;
        CXMLFile* m_pXMLFile;
        CXMLNode* m_pRoot;

        CXMLBuffer ()
        {
            m_pXMLFile = NULL;
            m_pRoot = NULL;
        }

        ~CXMLBuffer ()
        {
            // Close XML file
            if ( m_pXMLFile )
            {
                delete m_pXMLFile;
                m_pXMLFile = NULL;
                m_pRoot = NULL;
            }
            // Attempt to delete temp file
            FileDelete ( m_strTempFileName );
        }

        CXMLNode* SetFromData ( char* data, uint uiSize )
        {
            assert ( !m_pXMLFile );

            // Try to save
            m_strTempFileName = MakeUniquePath ( PathJoin ( GetMTADataPath (), "temp", "buffer.xml" ) );
            if ( !FileSave ( m_strTempFileName, &data[0], uiSize ) )
            {
                AddReportLog ( 2501, SString ( "CXMLBuffer::SetFromBuffer: Could not save %s", m_strTempFileName.c_str () ) );
                return NULL;
            }

            m_pXMLFile = CCore::GetSingleton ().GetXML ()->CreateXML ( m_strTempFileName );
            if ( !m_pXMLFile )
            {
                AddReportLog ( 2502, SString ( "CXMLBuffer::SetFromBuffer: Could not CreateXML %s", m_strTempFileName.c_str () ) );
                return NULL;
            }
            if ( !m_pXMLFile->Parse () )
            {
                AddReportLog ( 2503, SString ( "CXMLBuffer::SetFromBuffer: Could not parse %s", m_strTempFileName.c_str () ) );
                return NULL;
            }
            m_pRoot = m_pXMLFile->GetRootNode ();
            if ( !m_pRoot )
            {
                AddReportLog ( 2504, SString ( "CXMLBuffer::SetFromBuffer: No root node in %s", m_strTempFileName.c_str () ) );
                return NULL;
            }

            return m_pRoot;
        }

    };



    // A node and its attributes
    struct SDataInfoItem
    {
        SString strName;
        SString strValue;
        std::map < SString, SString > attributeMap;
        SString GetAttribute ( const SString& strName ) const
        {
            const SString* pValue = MapFind ( attributeMap, strName );
            return pValue ? *pValue : "";
        }
        void SetAttribute ( const SString& strName, const SString& strValue )
        {
            MapSet ( attributeMap, strName, strValue );
        }
    };

    // A list of subnodes and their attributes
    class CDataInfoSet : public std::vector < SDataInfoItem >
    {
    public:
    };

    ///////////////////////////////////////////////////////////////
    //
    // CXMLAccess
    //
    // Heavy lifting done here
    //
    ///////////////////////////////////////////////////////////////
    class CXMLAccess
    {
    public:
        CXMLNode* m_pRoot;

        CXMLAccess ( CXMLNode* pNode )
        {
            m_pRoot = pNode;
        }

        // Get/create node at path
        CXMLNode* GetSubNode ( const SString& strPath, bool bCreateIfRequired = false )
        {
            std::vector < SString > parts;
            strPath.Split ( ".", parts );
            // Follow path
            CXMLNode* pNode = m_pRoot;
            for ( uint i = 0 ; i < parts.size () && pNode ; i++ )
            {
                CXMLNode* pNext = pNode->FindSubNode ( parts[i], 0 );
                if ( !pNext )
                    pNext = pNode->CreateSubNode ( parts[i] );
                pNode = pNext;
            }
            return pNode;
        }

        // Set value from a CValue
        bool SetSubNodeValue ( const SString& strPath, const CValue& strValue )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath, true ) )
            {
                pNode->SetTagContent ( strValue.ToString () );
                return true;
            }
            return false;
        }

        // Set value from a string
        bool SetSubNodeValue ( const SString& strPath, const SString& strValue )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath, true ) )
            {
                pNode->SetTagContent ( strValue );
                return true;
            }
            return false;
        }

        // Set subnodes values from a string list
        bool SetSubNodeValue ( const SString& strPath, const std::vector < SString > & valueList, const SString& strKey = "item" )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath, true ) )
            {
                // Process each subnode
                for ( uint i = 0 ; i < valueList.size () ; i++ )
                    if ( CXMLNode* pSubNode = pNode->CreateSubNode ( strKey ) )
                        pSubNode->SetTagContent ( valueList[i] );
                return true;
            }
            return false;
        }

        // Set subnodes values from a CDataInfoSet
        bool SetSubNodeValue ( const SString& strPath, const CDataInfoSet& dataInfoSet )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath ) )
            {
                // Process each subnode
                for ( uint i = 0 ; i < dataInfoSet.size () ; i++ )
                {
                    const SDataInfoItem& item = dataInfoSet[i];
                    if ( CXMLNode* pSubNode = pNode->CreateSubNode ( item.strName ) )
                    {
                        pSubNode->SetTagContent ( item.strValue );
                        CXMLAttributes& attributes = pSubNode->GetAttributes ();
                        for ( std::map < SString, SString >::const_iterator iter = item.attributeMap.begin () ; iter != item.attributeMap.end () ; ++iter )
                        {
                            CXMLAttribute* attrib = attributes.Create ( iter->first );
                            attrib->SetValue ( iter->second );
                        }
                    }
                }
                return true;
            }
            return false;
        }

        // First result as CValue
        bool GetSubNodeValue ( const SString& strPath, CValue& strOut, const char* szDefault = "" )
        {
            assert ( szDefault );
            if ( CXMLNode* pNode = GetSubNode ( strPath ) )
            {
                strOut.SetFromString ( pNode->GetTagContent () );
                return true;
            }
            strOut.SetFromString ( szDefault );
            return false;
        }

        // First result as string
        bool GetSubNodeValue ( const SString& strPath, SString& strOut, const char* szDefault = "" )
        {
            assert ( szDefault );
            if ( CXMLNode* pNode = GetSubNode ( strPath ) )
            {
                strOut = pNode->GetTagContent ();
                return true;
            }
            strOut = szDefault;
            return false;
        }

        // All subnodes values as a string list
        bool GetSubNodeValue ( const SString& strPath, std::vector < SString > & outList )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath ) )
            {
                // Process each subnode
                for ( uint i = 0 ; i < pNode->GetSubNodeCount () ; i++ )
                    outList.push_back ( pNode->GetSubNode ( i )->GetTagContent () );
                return true;
            }
            return false;
       }

        // All subnodes as a CDataInfoSet
        bool GetSubNodeValue ( const SString& strPath, CDataInfoSet& outMap )
        {
            if ( CXMLNode* pNode = GetSubNode ( strPath ) )
            {
                // Process each subnode
                for ( uint i = 0 ; i < pNode->GetSubNodeCount () ; i++ )
                {
                    CXMLNode* pSubNode = pNode->GetSubNode ( i );

                    SDataInfoItem item;
                    item.strName = pSubNode->GetTagName ();
                    item.strValue = pSubNode->GetTagContent ();
                    CXMLAttributes& attributes = pSubNode->GetAttributes ();
                    for ( uint i = 0 ; i < attributes.Count () ; i++ )
                    {
                        CXMLAttribute* attribute = attributes.Get ( i );
                        MapSet ( item.attributeMap, SString ( attribute->GetName () ), SString ( attribute->GetValue () ) );
                    }
                    outMap.push_back ( item );
                }
                return true;
            }
            return false;
        }
    };


    ///////////////////////////////////////////////////////////////
    //
    // CReportWrap
    //
    // gawd knows
    //
    ///////////////////////////////////////////////////////////////
    class CReportWrap
    {
        SString strFilter;
        int iMinSize;
        int iMaxSize;
        const static int DEFAULT_MIN_SIZE = 1;
        const static int DEFAULT_MAX_SIZE = 5000;
    public:

        CReportWrap ()
            : iMinSize ( DEFAULT_MIN_SIZE )
            , iMaxSize ( DEFAULT_MAX_SIZE )
        {
            LoadReportSettings ();
        }

        void SaveReportSettings ( void ) const
        {
            CArgMap m_ArgMap ( "@", ";" );
            m_ArgMap.Set ( "filter2", strFilter );
            m_ArgMap.Set ( "min", iMinSize );
            m_ArgMap.Set ( "max", iMaxSize );
            CVARS_SET ( "reportsettings", m_ArgMap.ToString () );
            SetApplicationSetting ( "reportsettings", m_ArgMap.ToString () );
        }

        void LoadReportSettings ( void )
        {
            SString strSettings;
            CVARS_GET ( "reportsettings", strSettings );
            SetSettings ( strSettings );
        }

        void SetSettings ( const SString& strSettings )
        {
            CArgMap m_ArgMap ( "@", ";" );
            m_ArgMap.SetFromString ( strSettings );
            // If build is 30 days old, default no report logging
            m_ArgMap.Get ( "filter2", strFilter, GetBuildAge () < 30 ? "+all" : "-all" );
            m_ArgMap.Get ( "min", iMinSize, DEFAULT_MIN_SIZE );
            m_ArgMap.Get ( "max", iMaxSize, DEFAULT_MAX_SIZE );
            iMinSize = 11;      // To fix parse bug in previous builds
            iMaxSize = 4001;
            SaveReportSettings ();
        }

        SString GetFilter ( void ) const
        {
            return strFilter != "" ? strFilter : "+all";
        }

        int GetMinSize ( void ) const
        {
            return iMinSize;
        }

        int GetMaxSize ( void ) const
        {
            return iMaxSize;
        }

        static void ClearLogContents ( const SString& strIdFilter )
        {
            if ( strIdFilter == "-all" && FileExists ( CalcMTASAPath ( "_keep_report_" ) ) )
                return;
            if ( FileExists ( CalcMTASAPath ( "_keep_report_all_" ) ) )
                return;
            SetReportLogContents ( GetLogContents ( strIdFilter ) );
        }

        static SString GetLogContents ( const SString& strIdFilter, int iMaxSize = 0 )
        {
            CFilterMap filterMap ( strIdFilter );

            // Load file into a string
            SString strContent = GetReportLogContents ();

            // Split into lines
            std::vector < SString > lines;
            strContent.Split ( "\n", lines );

            // Filter each line
            int iSize = 0;
            std::vector < SString > filteredLines;
            for ( int i = lines.size () - 1 ; i  >= 0 ; i-- )
            {
                const SString& strLine = lines[i];
                if ( !strLine.empty () && !filterMap.IsFiltered ( atoi ( strLine ) ) )
                {
                    iSize += strLine.length ();
                    if ( iMaxSize && iSize > iMaxSize )
                        break;
                    filteredLines.push_back ( strLine );
                }
            }

            // Compose final output
            SString strResult;
            for ( int i = filteredLines.size () - 1 ; i  >= 0 ; i-- )
                strResult += filteredLines[i] + "\n";

            return strResult;
        }
    };

}

namespace
{

    ///////////////////////////////////////////////////////////////
    //
    //
    // SUpdaterVarConfig
    //
    //
    ///////////////////////////////////////////////////////////////
    struct SUpdaterVarConfig
    {
        CDateTime       master_lastCheckTime;
        SString         master_highestNotifyRevision;
        CDateTime       version_lastCheckTime;
        CDateTime       news_lastCheckTime;
        SString         news_lastNewsDate;
        CDataInfoSet    crashdump_history;
    };

    ///////////////////////////////////////////////////////////////
    //
    //
    // SUpdaterMasterConfig
    //
    //
    ///////////////////////////////////////////////////////////////
    struct SUpdaterMasterConfig
    {
        struct {
            SString         strRevision;
            CDataInfoSet    serverInfoMap;
            CTimeSpan       interval;
        } master;

        struct {
            CDataInfoSet    serverInfoMap;
            CTimeSpan       interval;
        } version;

        struct {
            CDataInfoSet    serverInfoMap;
            CTimeSpan       interval;
            SString         strFilter;
            CValueInt       iMinSize;
            CValueInt       iMaxSize;
        } report;

        struct {
            CDataInfoSet    serverInfoMap;
            CValueInt       iDuplicates;
            CValueInt       iMaxHistoryLength;
        } crashdump;

        struct {
            CDataInfoSet    serverInfoMap;
        } gtadatafiles;

        struct {
            CDataInfoSet    serverInfoMap;
        } trouble;

        struct {
            CDataInfoSet    serverInfoMap;
        } ase;

        struct {
            CDataInfoSet    serverInfoMap;
            CDataInfoSet    nobrowseInfoMap;
            CDataInfoSet    onlybrowseInfoMap;
        } sidegrade;

        struct {
            CDataInfoSet    serverInfoMap;
            CTimeSpan       interval;
            SString         strOldestPost;
            CValueInt       iMaxHistoryLength;
        } news;

        struct {
            struct {
                SString         strFilter;
            } debug;
        } misc;

        bool IsValid () const
        {
            return master.strRevision.length ()
                    && version.serverInfoMap.size ()
                    && report.serverInfoMap.size ()
                    && crashdump.serverInfoMap.size ()
                    && gtadatafiles.serverInfoMap.size ()
                    && trouble.serverInfoMap.size ()
                    && ase.serverInfoMap.size ()
                    && !news.strOldestPost.empty ()
                    ;
        }
    };

}

namespace
{

    ///////////////////////////////////////////////////////////////
    //
    //
    // SJobInfo
    //
    //
    ///////////////////////////////////////////////////////////////
    struct SJobInfo
    {
                                SJobInfo ( void )
                                    : iMaxServersToTry ( 3 )
                                    , iTimeoutConnect ( 10000 )
                                    , iTimeoutTransfer ( 25000 )
                                    , bShowDownloadPercent ( true )
                                    , bPostContentBinary ( false )
                                    , iCurrent ( 0 )
                                    , iRetryCount ( 0 )
                                    , iIdleTime ( 0 )
                                    , iIdleTimeLeft ( 0 )
                                    , uiBytesDownloaded ( 0 )
                                    , iFilesize ( 0 )
                                {}
        // Input
        int                     iMaxServersToTry;
        int                     iTimeoutConnect;
        int                     iTimeoutTransfer;
        bool                    bShowDownloadPercent;
        std::vector < SString > serverList;
        std::vector < char >    postContent;
        bool                    bPostContentBinary;
        SString                 strPostFilename;

        // Using
        int                     iCurrent;
        int                     iRetryCount;
        int                     iIdleTime;
        int                     iIdleTimeLeft;
        unsigned int            uiBytesDownloaded;

        // Result
        std::vector < char >    downloadBuffer;
        SString                 strStatus;
        SString                 strTitle;
        SString                 strMsg;
        SString                 strMsg2;
        SString                 strYes;
        SString                 strNo;
        SString                 strPriority;
        SString                 strFilename;
        CValueInt               iFilesize;
        CDataInfoSet            serverInfoMap;
        SString                 strMD5;
        SString                 strSaveLocation;
    };

}

namespace
{

    struct CStringPair
    {
        SString strValue1;
        SString strValue2;
    };

    ///////////////////////////////////////////////////////////////
    //
    // CConditionMap
    //
    //
    //
    ///////////////////////////////////////////////////////////////
    class CConditionMap : public std::map < SString, CStringPair >
    {
    public:

        void SetCondition ( const SString& strType, const SString& strValue1, const SString& strValue2 = "" )
        {
            CStringPair pair;
            pair.strValue1 = strValue1.ToLower ();
            pair.strValue2 = strValue2.ToLower ();
    #if MTA_DEBUG
            CStringPair* pPair = MapFind ( *this, strType.ToLower () );
            if ( !pPair || pPair->strValue1 != pair.strValue1 || pPair->strValue2 != pair.strValue2 )
                OutputDebugLine ( SString ( "SetCondition %s %s %s", strType.c_str (), strValue1.c_str (), strValue2.c_str () ) );
    #endif
            MapSet ( *this, strType.ToLower (), pair );
        }

        bool IsConditionTrue ( const SString& strCondition )
        {
            if ( strCondition.length () == 0 )
                return true;

            bool bResult = false;

            SString strType, strTemp, strValue1, strValue2;
            strCondition.ToLower ().Split( ".", &strType, &strTemp );
            strTemp.Split( ".", &strValue1, &strValue2 );

            bool bInvert = strValue1[0] == '!';
            if ( bInvert )
                strValue1 = strValue1.substr ( 1 );

            if ( CStringPair *pPair = MapFind ( *this, strType ) )
            {
                bResult = ( !strValue1.size () || pPair->strValue1 == strValue1 )
                       && ( !strValue2.size () || pPair->strValue2 == strValue2 );
            }

            if ( bInvert )
                bResult = !bResult;

            return bResult;
        }
    };




    typedef void (CVersionUpdater::*PFNVOIDVOID) ( void );

    ///////////////////////////////////////////////////////////////
    //
    // CInstruction
    //
    // One update instruction
    //
    ///////////////////////////////////////////////////////////////
    struct CInstruction
    {
        SString     strLabel;
        SString     strCondition;
        SString     strGoto;
        PFNVOIDVOID pfnCmdFunc;

        CInstruction ( PFNVOIDVOID pfnCmdFunc )
        {
            this->pfnCmdFunc = pfnCmdFunc;
        }
        CInstruction ( const SString& strText )
        {
            this->pfnCmdFunc = NULL;
            std::vector < SString > parts;
            strText.Split ( " ", parts );
            if ( parts.size () == 1 )
                strLabel = parts[0].TrimEnd ( ":" );    // Label
            else
            if ( parts.size () == 4 )
            {
                strCondition = parts[1];                // Conditional goto
                strGoto = parts[3];
            }
        }
        bool IsLabel() const            { return strLabel.length () != 0; }
        bool IsConditionalGoto() const  { return strCondition.length () != 0; }
        bool IsFunction() const         { return pfnCmdFunc != NULL; }
    };


    ///////////////////////////////////////////////////////////////
    //
    // CProgram
    //
    //  Many update instructions
    //
    ///////////////////////////////////////////////////////////////
    struct CProgram
    {
        unsigned int pc;
        std::vector < CInstruction >    instructions;

        CProgram() : pc ( 0 ) {}
        bool IsValid () const
        {
            return ( pc >= 0 && pc < instructions.size () );
        }
        const CInstruction* GetNextInstruction()
        {
            if ( IsValid () )
                return &instructions[ pc++ ];
            return NULL;
        }
        void GotoLabel ( const SString& strLabel )
        {
            pc = FindLabel ( strLabel );
        }
        int FindLabel( const SString& strLabel ) const
        {
            for ( unsigned int i = 0 ; i < instructions.size () ; i++ )
                if ( instructions[i].IsLabel () && instructions[i].strLabel == strLabel )
                    return i;
            return -1;
        }
    };


    #define Push(F) \
        m_Stack.push_back ( &CVersionUpdater::F )

    #define ADDINST( expr ) \
        program.instructions.push_back ( CInstruction ( &CVersionUpdater::expr ) );
    #define ADDCOND( condition ) \
        program.instructions.push_back ( CInstruction ( condition ) );
    #define ADDLABL( label ) \
        program.instructions.push_back ( CInstruction ( label ) );

}
