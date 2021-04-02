using namespace v8;

class CV8AsyncFunction;
typedef Persistent<Promise::Resolver> PersistentResolver;
typedef Persistent<Context>           PersistentContext;

class CV8Promise : public CV8PromiseBase, public Task
{
public:
    CV8Promise(CV8Isolate* pIsolate, std::function<void(CV8AsyncContextBase*)> pFunctionAsyncCallback);
    ~CV8Promise();

    void           Resolve(std::string arg);
    void           Resolve();
    Local<Promise> GetPromise() const { return m_promiseResolver.Get(m_pIsolate->GetIsolate())->GetPromise(); }
    void           Run();

    bool IsPending();
    bool IsFulfilled();
    bool IsRejected();

    void                                          Reject();
    void                                          SetCancelationToken(std::shared_ptr<CV8Isolate::CancelationToken> token) { m_pCancelationToken = token; }
    std::shared_ptr<CV8Isolate::CancelationToken> GetCancelationToken() const { return m_pCancelationToken; }

private:
    void Resolve(Local<Value> value);
    void RejectUndefined();
    void Reject(Local<Value> value);

    CV8Isolate*                                   m_pIsolate;
    PersistentContext                             m_pContext;
    PersistentResolver                            m_promiseResolver;
    std::function<void(CV8AsyncContextBase*)>     m_pFunctionAsyncCallback;
    bool                                          bHasResult = false;
    std::mutex                                    m_lock;
    std::shared_ptr<CV8Isolate::CancelationToken> m_pCancelationToken;
};
