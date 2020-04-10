/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CGraphStats.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

namespace
{
    #define GRAPHSTAT_HISTORY_SIZE 256

    struct SGraphStatLine
    {
        TIMEUS              prevData;
        std::vector<TIMEUS> dataHistory;
        int                 iDataPos;
        SColor              color;
        std::string         strName;
    };
}            // namespace

///////////////////////////////////////////////////////////////
//
// CGraphStats class
//
///////////////////////////////////////////////////////////////
class CGraphStats : public CGraphStatsInterface
{
public:
    ZERO_ON_NEW
    CGraphStats();
    ~CGraphStats();

    // CGraphStatsInterface methods
    virtual void Draw();
    virtual void SetEnabled(bool bEnabled);
    virtual bool IsEnabled();
    virtual void AddTimingPoint(const char* szName);

protected:
    bool                              m_bEnabled;
    std::map<SString, SGraphStatLine> m_LineList;
    TIMEUS                            m_StartTime;
};

///////////////////////////////////////////////////////////////
//
// CGraphStats instantiation
//
///////////////////////////////////////////////////////////////
CGraphStats* g_pGraphStats = NULL;

CGraphStatsInterface* GetGraphStats()
{
    if (!g_pGraphStats)
        g_pGraphStats = new CGraphStats();
    return g_pGraphStats;
}

///////////////////////////////////////////////////////////////
//
// CGraphStats implementation
//
///////////////////////////////////////////////////////////////
CGraphStats::CGraphStats()
{
}

CGraphStats::~CGraphStats()
{
}

///////////////////////////////////////////////////////////////
//
// CGraphStats::SetEnabled
//
//
//
///////////////////////////////////////////////////////////////
void CGraphStats::SetEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    m_LineList.clear();
    m_StartTime = 0;
}

///////////////////////////////////////////////////////////////
//
// CGraphStats::IsEnabled
//
//
//
///////////////////////////////////////////////////////////////
bool CGraphStats::IsEnabled()
{
    return m_bEnabled;
}

///////////////////////////////////////////////////////////////
//
// CGraphStats::AddTimingPoint
//
//
//
///////////////////////////////////////////////////////////////
void CGraphStats::AddTimingPoint(const char* szName)
{
    if (!IsEnabled())
        return;

    // Start of next frame?
    if (szName[0] == 0)
    {
        TIMEUS endTime = GetTimeUs();
        TIMEUS frameTime = endTime - m_StartTime;
        m_StartTime = endTime;

        // Duplicate data points to make up for missed time
        int Dups = frameTime / 33000;
        if (Dups > 0)
        {
            Dups = std::min(100, Dups);
            for (std::map<SString, SGraphStatLine>::iterator iter = m_LineList.begin(); iter != m_LineList.end(); ++iter)
            {
                SGraphStatLine* pLine = &iter->second;

                float Data = pLine->dataHistory[pLine->iDataPos];
                for (int i = 0; i < Dups; i++)
                {
                    pLine->iDataPos++;
                    if (pLine->iDataPos > GRAPHSTAT_HISTORY_SIZE - 1)
                        pLine->iDataPos = 0;
                    pLine->dataHistory[pLine->iDataPos] = Data;
                }
            }
        }
        return;
    }

    if (m_StartTime == 0)
        return;

    // Find existing line
    SGraphStatLine* pLine = MapFind(m_LineList, szName);

    if (!pLine)
    {
        // Add new line
        MapSet(m_LineList, szName, SGraphStatLine());
        pLine = MapFind(m_LineList, szName);
        pLine->dataHistory.resize(GRAPHSTAT_HISTORY_SIZE);
        memset(&pLine->dataHistory[0], 0, pLine->dataHistory.size());
        pLine->iDataPos = 0;
        pLine->prevData = 0;
        pLine->strName = szName;

        // Random color based on line name
        MD5 md5;
        CMD5Hasher().Calculate(szName, strlen(szName), md5);
        uchar* p = md5.data;
        while (p[0] + p[1] + p[2] < 128)
        {
            int f = rand() % NUMELMS(md5.data);
            int t = rand() % 3;
            p[t] = std::min(255, p[t] + p[f] + 1);
        }
        pLine->color = SColorRGBA(p[0], p[1], p[2], 255);
    }

    // Calc timing averaged with previous frame
    TIMEUS NewData = GetTimeUs() - m_StartTime;
    TIMEUS AvgData = (NewData + pLine->prevData) / 2;
    pLine->prevData = NewData;

    // Inc position
    pLine->iDataPos++;
    if (pLine->iDataPos > GRAPHSTAT_HISTORY_SIZE - 1)
        pLine->iDataPos = 0;

    // Insert data point
    pLine->dataHistory[pLine->iDataPos] = AvgData;
}

///////////////////////////////////////////////////////////////
//
// CGraphStats::Draw
//
//
//
///////////////////////////////////////////////////////////////
void CGraphStats::Draw()
{
    if (!m_bEnabled)
        return;

    CGraphicsInterface* pGraphics = g_pCore->GetGraphics();

    uint  uiViewportHeight = pGraphics->GetViewportHeight();
    uint  uiOriginX = 10;
    uint  uiOriginY = std::min<int>(500, uiViewportHeight - 10);
    uint  uiSizeX = GRAPHSTAT_HISTORY_SIZE;
    uint  uiSizeY = 150;
    uint  uiRangeY = 100;            // 100ms
    float fLineScale = 1 / 1000.f / uiRangeY * uiSizeY;
    float fLineHeight = pGraphics->GetDXFontHeight();

    // Backgroung box
    pGraphics->DrawRectQueued(uiOriginX, uiOriginY - uiSizeY, uiSizeX, uiSizeY, SColorRGBA(0, 0, 0, 128), true);

    // Draw data lines
    float fLabelX = uiOriginX + uiSizeX + 22;
    float fLabelY = uiOriginY - m_LineList.size() * fLineHeight;
    for (const auto& dataLine : m_LineList)
    {
        const SGraphStatLine& line = dataLine.second;
        int                   iDataPos = line.iDataPos;
        int                   iDataPosPrev = iDataPos;

        for (int i = uiSizeX - 1; i > 0; i--)
        {
            float fY0 = line.dataHistory[iDataPos] * fLineScale;
            float fY1 = line.dataHistory[iDataPosPrev] * fLineScale;

            iDataPosPrev = iDataPos;
            iDataPos--;
            if (iDataPos == -1)
                iDataPos = GRAPHSTAT_HISTORY_SIZE - 1;

            pGraphics->DrawLineQueued(uiOriginX + i - 1, uiOriginY - fY0, uiOriginX + i, uiOriginY - fY1, 1, line.color, true);

            if (i == uiSizeX - 1)
            {
                // Line from graph to label
                pGraphics->DrawLineQueued(uiOriginX + i - 1, uiOriginY - fY0, fLabelX - 2, fLabelY + fLineHeight / 2, 1, line.color, true);
            }
        }

        pGraphics->DrawStringQueued(fLabelX, fLabelY, fLabelX, fLabelY, line.color, line.strName.c_str(), 1.0f, 1.0f, DT_NOCLIP, nullptr, true);
        fLabelY += fLineHeight;
    }
}
