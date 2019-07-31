#include <vector>
#include <string>

struct ideipl_line
{
private:
    std::vector<std::string> _entries;
    unsigned int             _currentEntry;

public:
    enum ideipl_line_options
    {
        skip
    };

    ideipl_line(unsigned int parameterCount)
    {
        _currentEntry = 0;
        _entries.resize(parameterCount);
    }
    ideipl_line(std::string const& line);
    ideipl_line& operator>>(int& out);
    ideipl_line& operator>>(unsigned int& out);
    ideipl_line& operator>>(unsigned char& out);
    ideipl_line& operator>>(float& out);
    ideipl_line& operator>>(std::string& out);

    template <size_t Size>
    ideipl_line& operator>>(char (&out)[Size])
    {
        if (_currentEntry < _entries.size())
            strcpy_s(out, _entries[_currentEntry++].c_str());
        return *this;
    }

    ideipl_line& operator>>(ideipl_line_options option);
    ideipl_line& operator<<(int in);
    ideipl_line& operator<<(unsigned int in);
    ideipl_line& operator<<(unsigned char in);
    ideipl_line& operator<<(float in);
    ideipl_line& operator<<(std::string const& in);

    template <size_t Size>
    ideipl_line& operator<<(char (&in)[Size])
    {
        if (_currentEntry < _entries.size())
            _entries[_currentEntry++] = in;
        return *this;
    }

    void GetEntriesAsConcatenatedLine(std::string& out)
    {
        for (size_t i = 0; i < _entries.size(); i++)
        {
            std::string& entry = _entries[i];
            if (i == 0)
            {
                out = entry;
            }
            else
            {
                out += (", " + entry);
            }
        }
    }
    size_t param_count();
};