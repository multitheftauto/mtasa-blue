class CV8IsolateBase
{
public:
    virtual void RunCode(std::string& code, std::string originFileName) = 0;
    virtual bool GetErrorMessage(std::string& error) = 0;
    virtual bool GetMissingModulesErrorMessage(std::string& error) = 0;

    virtual void SetEvalEnabled(bool value) = 0;

    virtual void Evaluate() = 0;
};
