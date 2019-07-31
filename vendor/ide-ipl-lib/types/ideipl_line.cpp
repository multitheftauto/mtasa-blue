#include "ideipl_line.h"

ideipl_line::ideipl_line(std::string const &line) {
    _currentEntry = 0;
    enum scanning_state {
        scanning_before_string, scanning_quoted_string, scanning_string, scanning_after_string
    } state = scanning_before_string;
    std::string entry;
    for (size_t i = 0; i < line.length(); i++) {
        switch (state) {
        case scanning_before_string:
        case scanning_after_string:
            if (line[i] != ' ' && line[i] != '\t') {
                if (line[i] == ',') {
                    if (state == scanning_before_string)
                        _entries.push_back("");
                    else
                        state = scanning_before_string;
                }
                else {
                    entry.clear();
                    if (line[i] == '"')
                        state = scanning_quoted_string;
                    else {
                        state = scanning_string;
                        entry.push_back(line[i]);
                    }
                }
            }
            break;
        case scanning_string:
            if (line[i] == ' ' || line[i] == '\t' || line[i] == ',') {
                _entries.push_back(entry);
                state = line[i] == ',' ? scanning_before_string : scanning_after_string;
            }
            else
                entry.push_back(line[i]);
            break;
        case scanning_quoted_string:
            if (line[i] == '"') {
                _entries.push_back(entry);
                state = scanning_after_string;
            }
            else
                entry.push_back(line[i]);
            break;
        }
    }
    if (state == scanning_string)
        _entries.push_back(entry);
}

ideipl_line &ideipl_line::operator>>(int &out) {
    if (_currentEntry < _entries.size())
        out = std::stoi(_entries[_currentEntry++]);
    return *this;
}

ideipl_line &ideipl_line::operator>>(unsigned int &out) {
    if (_currentEntry < _entries.size())
        out = std::stoi(_entries[_currentEntry++]);
    return *this;
}

ideipl_line &ideipl_line::operator>>(unsigned char &out) {
    if (_currentEntry < _entries.size())
        out = std::stoi(_entries[_currentEntry++]);
    return *this;
}

ideipl_line &ideipl_line::operator>>(float &out) {
    if (_currentEntry < _entries.size())
        out = std::stof(_entries[_currentEntry++]);
    return *this;
}

ideipl_line &ideipl_line::operator>>(std::string &out) {
    if (_currentEntry < _entries.size())
        out = _entries[_currentEntry++];
    return *this;
}

ideipl_line &ideipl_line::operator>>(ideipl_line_options option) {
    if (_currentEntry < _entries.size()) {
        switch (option) {
        case skip:
            _currentEntry++;
            break;
        }
    }
    return *this;
}

ideipl_line &ideipl_line::operator<<(int in) {
    if (_currentEntry < _entries.size())
        _entries[_currentEntry++] = std::to_string(in);
    return *this;
}

ideipl_line &ideipl_line::operator<<(unsigned int in) {
    if (_currentEntry < _entries.size())
        _entries[_currentEntry++] = std::to_string(in);
    return *this;
}

ideipl_line &ideipl_line::operator<<(unsigned char in) {
    if (_currentEntry < _entries.size())
        _entries[_currentEntry++] = std::to_string(in);
    return *this;
}

ideipl_line &ideipl_line::operator<<(float in) {
    if (_currentEntry < _entries.size())
        _entries[_currentEntry++] = std::to_string(in);
    return *this;
}

ideipl_line &ideipl_line::operator<<(std::string const &in) {
    if (_currentEntry < _entries.size())
        _entries[_currentEntry++] = in;
    return *this;
}

size_t ideipl_line::param_count() {
    return _entries.size();
}