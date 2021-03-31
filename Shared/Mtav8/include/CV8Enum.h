class CV8Enum : public CV8EnumBase
{
public:
    CV8Enum();

    Local<Object> Initialize(CV8Isolate* pIsolate, std::string name);
    void          SetValue(std::string key, std::string value);

private:
    std::unordered_map<std::string, std::string> m_stringKeys;
};
