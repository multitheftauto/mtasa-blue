/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CEntryHistory.h
 *  PURPOSE:     Header file for the entry history class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Used for cleaning sensitive info from entries
struct
{
    unsigned int numBlanks;
    const char*  delim;
    const char*  text;
} const g_WordsToCheck[] = {{2, "", "login "}, {2, "", "register "}, {2, "", "addaccount "}, {2, "", "chgpass "}, {2, "", "chgmypass "}, {1, "'", "password"}};

class CEntryHistoryItem
{
public:
    // Contains the original entry
    SString entry;

    // Contains the temporary, edited version of the entry
    SString temp;

    // Contains a clean version of the entry with sensitive info parsed out
    SString clean;

    CEntryHistoryItem(const SString& entry) { SetContent(entry); }

    // Reset the temporary value with the original value
    void Reset() { temp = entry; }

    void SetContent(SString entry);

    bool operator!=(const SString& other) const { return entry != other; }
    bool operator==(const SString& other) const { return entry == other; }
         operator const char*() const { return entry.c_str(); }
};

class CEntryHistory
{
public:
    CEntryHistory(unsigned int historyLength) : m_maxEntries(historyLength) {}

    static void CleanLine(SString& line);
    static int  ReplaceNextWord(SString& line, int pos, const char* blanker);

    void Add(SString line);
    void LoadFromFile(SString filename);

    // Clear all history entries
    void Clear() { m_entries.clear(); }

    // Check whether history contains a line
    bool Contains(const SString& line) const { return std::find(m_entries.begin(), m_entries.end(), line) != m_entries.end(); }

    // Return a boolean representing whether the history is empty or not
    bool Empty() const { return m_entries.empty(); }

    // Return a specific entry from history
    CEntryHistoryItem* Get(unsigned int index)
    {
        auto& iter = std::next(m_entries.begin(), index);
        if (iter != m_entries.end())
            return &(*iter);
        return nullptr;
    }

    // Get the last entry in history
    CEntryHistoryItem& GetLast() { return m_entries.front(); }

    // Resets temporary history edits to their original values
    void ResetChanges()
    {
        for (auto& item : m_entries)
            item.Reset();
    }

    // Return the size of history
    int Size() const { return m_entries.size(); }

private:
    // Contains all history entries
    std::list<CEntryHistoryItem> m_entries;

    // Maximum amount of entries before we start deleting from the end
    unsigned int m_maxEntries;

    // Output filename to save history to
    SString m_outFilename;
};
