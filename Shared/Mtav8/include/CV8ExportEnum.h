class CV8ExportEnum : public CV8ExportEnumBase
{
public:
    CV8ExportEnum();

    Local<Object> Initialize(CV8Isolate* pIsolate, std::string name);
    void          SetValue(std::string key, std::string value);

private:
    std::unordered_map<std::string, std::string> m_stringKeys;
};
