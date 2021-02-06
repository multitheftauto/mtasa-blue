using namespace v8;

class CV8Utils
{
public:
    static Local<String> ToV8String(const char* szString);
    static Local<String> ToV8String(std::string str);
};
