class CVector;
class CMatrix;

using namespace v8;

class CV8Matrix : public CV8BaseClass
{
public:
    inline static EClass      m_eClass = EClass::Matrix;
    inline static const char* m_szName = "Matrix";

    static MaybeLocal<Object> New(CVector position, CVector rotation, CVector scale);

    // Adds new class definition to the context
    static Handle<FunctionTemplate> CreateTemplate(Local<Context> context);

private:
    static bool ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CMatrix* value);

    static CVector GetPosition(CMatrix* internalValue);
    static void    SetPosition(CMatrix* internalValue, CVector value);
    static CVector GetRotation(CMatrix* internalValue);
    static void    SetRotation(CMatrix* internalValue, CVector value);
    static CVector GetScale(CMatrix* internalValue);
    static void    SetScale(CMatrix* internalValue, CVector value);

    static void MethodInverse(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue);
    static void MethodInvert(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue);
    static CVector MethodTransformVector(CV8FunctionCallback& info, Local<Object> self, CMatrix* internalValue);
};
