#pragma once
#include <map>
#include <vector>
#include <variant>

class entry {
    std::map<std::string, std::variant<int, float, bool, std::string>> properties;
public:
    template <typename T>
    T getProperty(std::string const &name) {
        return std::get<T>(properties[name]);
    }

    template <typename T>
    T getProperty(std::string const &name, T const &defaultValue) {
        auto prop = properties.find(name);
        if (prop == properties.end()) {
            properties[name] = defaultValue;
            return defaultValue;
        }
        return *prop;
    }

    template <typename T>
    void setProperty(std::string const &name, T const &value) {
        properties[name] = value;
    }
};