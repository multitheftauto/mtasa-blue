class CV8PromiseBase
{
public:
    virtual void Resolve() = 0;
    virtual void Resolve(std::string arg) = 0;
};
