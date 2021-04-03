using namespace v8;

class CV8Utils
{
public:
    static Local<String> ToV8String(const char* szString);
    static Local<String> ToV8String(std::string str);
    static Local<Number> ToV8Number(double number);

    static const char* ToString(const String::Utf8Value& value);
};
