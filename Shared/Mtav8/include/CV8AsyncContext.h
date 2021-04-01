class CV8Promise;

class CV8AsyncContext : public CV8AsyncContextBase
{
public:
    CV8AsyncContext(CV8Promise* pPromise);
    void Resolve(std::string value);

    bool IsPending() const { return m_pPromise->IsPending(); }
    bool IsFulfilled() const { return m_pPromise->IsFulfilled(); }
    bool IsRejected() const { return m_pPromise->IsRejected(); }

    bool HasResult() const { return hasResult; }

private:
    CV8Promise*                                   m_pPromise;
    std::shared_ptr<CV8Isolate::CancelationToken> m_pCancelationToken;
    bool                                          hasResult = false;
};
