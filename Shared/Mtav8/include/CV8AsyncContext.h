class CV8Promise;

class CV8AsyncContext : public CV8AsyncContextBase
{
public:
    CV8AsyncContext(CV8Promise* pPromise);
    void Resolve(std::string value);

private:
    CV8Promise* m_pPromise;
};
