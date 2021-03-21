/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CEntryHistory.cpp
 *  PURPOSE:     Maintaining the history of whatever
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntryHistory.h"

//
// Set the item entry content
//
void CEntryHistoryItem::SetContent(SString entry)
{
    this->entry = entry;
    this->temp = this->entry;
    this->clean = this->entry;
    CEntryHistory::CleanLine(this->clean);
}

//
// Add a new line to history
//
void CEntryHistory::Add(SString line)
{
    if (line.empty())
        return;

    // Remove any matching entry
    m_entries.remove(line);

    // Push new entry to front
    m_entries.push_front(line);

    // Is the list too big? Remove the last items
    while (m_entries.size() > m_maxEntries)
        m_entries.pop_back();

    // If a filename has been specified, write the history, one per line
    if (!m_outFilename.empty())
    {
        std::ofstream outfile(FromUTF8(m_outFilename));
        for (const auto& item : m_entries)
            outfile << item.clean << std::endl;
        outfile.close();
    }
}

//
// Clean line of sensitive info
//
void CEntryHistory::CleanLine(SString& line)
{
    const char* blankText = "";
    for (unsigned int i = 0; i < NUMELMS(g_WordsToCheck); i++)
    {
        int         numBlanks = g_WordsToCheck[i].numBlanks;
        const char* delim = g_WordsToCheck[i].delim;
        const char* text = g_WordsToCheck[i].text;
        int         pos = line.ToLower().find(text);
        if (pos != std::string::npos)
        {
            pos += strlen(text);
            if (delim[0])
                pos = line.find_first_of(delim, pos);
            if (pos != std::string::npos)
            {
                while (numBlanks--)
                {
                    pos = ReplaceNextWord(line, pos, blankText);
                    // Also remove trailing space if present
                    if (line.SubStr(pos, 1) == " ")
                        line = line.SubStr(0, pos) + line.SubStr(pos + 1);
                }
            }
        }
    }
}

//
// Load history from a filename
//
void CEntryHistory::LoadFromFile(SString filename)
{
    SString filepath = CalcMTASAPath(filename);
    m_outFilename = filepath;

    // Load the history lines
    char          buffer[256];
    std::ifstream infile(FromUTF8(filepath));
    while (infile.getline(buffer, 256))
        if (buffer[0] && !Contains(buffer))
            m_entries.push_back(CEntryHistoryItem(buffer));
    infile.close();
}

//
// Replace next word in string with something
//
int CEntryHistory::ReplaceNextWord(SString& line, int pos, const char* blankText)
{
    int start = line.find_first_not_of("': ", pos);
    if (start != std::string::npos)
    {
        int end = line.find_first_of("' ", start);
        if (end == std::string::npos)
            end = line.length();
        line = line.SubStr(0, start) + blankText + line.SubStr(end);
        pos = start + strlen(blankText);
    }
    return pos;
}
