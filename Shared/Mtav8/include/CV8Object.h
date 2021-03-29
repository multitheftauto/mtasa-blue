class CV8Object : public CV8ObjectBase
{
public:
    CV8Object(Local<Object> object);

private:
    Local<Object> m_object;
};
