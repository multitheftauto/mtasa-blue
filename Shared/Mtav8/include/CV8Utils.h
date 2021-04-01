using namespace v8;

class CV8Utils
{
    struct any
    {
        enum class eType
        {
            Double,            // double
            Float,             // float
            String,            // std::string
            Object,            // Local<Object>
        };

        any(double v)
        {
            m_data.dValue = v;
            m_type = eType::Double;
        }

        any(float v)
        {
            m_data.fValue = v;
            m_type = eType::Float;
        }

        any(const char* v)
        {
            m_data.szValue = v;
            m_type = eType::String;
        }

        any(Local<Object> v)
        {
            objectValue = v;
            m_type = eType::Object;
        }

        any(MaybeLocal<Object> v)
        {
            assert(!v.IsEmpty());
            objectValue = v.ToLocalChecked();
            m_type = eType::Object;
        }

        eType         GetType() const { return m_type; }
        double        GetDouble() const { return m_data.dValue; }
        float         GetFloat() const { return m_data.fValue; }
        const char*   GetString() const { return m_data.szValue; }
        Local<Object> GetObject() const { return objectValue; }

    private:
        eType m_type;
        union
        {
            double      dValue;
            float       fValue;
            const char* szValue;
        } m_data;
        Local<Object> objectValue;
    };

public:
    static Local<String> ToV8String(const char* szString);
    static Local<String> ToV8String(std::string str);
    static Local<Number> ToV8Number(double number);

    static const char* ToString(const String::Utf8Value& value);

    // Returns global variable, "print(name)"
    static MaybeLocal<Value> GetGlobal(const char* name);
    static MaybeLocal<Value> GetClassByName(const char* className);

    // equivalent to "new className(...)" in javascript, throws js exception if class not found.
    // Ty can be: "double", "const char*"
    template <typename... Ty>
    static MaybeLocal<Object> NewObject(const char* className, const Ty&... args)
    {
        Isolate* isolate = Isolate::GetCurrent();
        assert(isolate);
        EscapableHandleScope handleScope(isolate);
        MaybeLocal<Value>    maybeV8Class = GetClassByName(className);
        Local<Value>         v8Class;
        if (!maybeV8Class.ToLocal(&v8Class))
        {
            isolate->ThrowException(CV8Utils::ToV8String(std::string(className) + " class not found or constructor does not exists."));
            return handleScope.EscapeMaybe(MaybeLocal<Object>());
        }

        Handle<Function> classConstructor = Handle<Function>::Cast(v8Class);

        Handle<Value> arguments[sizeof...(Ty)];
        int           argumentsCount = sizeof...(Ty);

        std::vector<any> funcArguments = {args...};
        for (unsigned i = 0; i < funcArguments.size(); ++i)
        {
            const any& value = funcArguments[i];
            switch (value.GetType())
            {
                case any::eType::Float:
                    arguments[i] = ToV8Number((double)value.GetFloat());
                    break;
                case any::eType::Double:
                    arguments[i] = ToV8Number(value.GetDouble());
                    break;
                case any::eType::String:
                    arguments[i] = ToV8String(value.GetString());
                    break;
                case any::eType::Object:
                    arguments[i] = value.GetObject();
                    break;
            }
        }
        Local<Object> result =
            Local<Object>::Cast(classConstructor->CallAsConstructor(isolate->GetCurrentContext(), argumentsCount, arguments).ToLocalChecked());
        return handleScope.Escape(result);
        //        Handle<Function> v8Class = Handle<Function>::Cast(value);
    }
};
