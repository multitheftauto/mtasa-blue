using namespace v8;

typedef Persistent<Promise::Resolver> PersistentResolver;
typedef Persistent<Context>           PersistentContext;
class CV8AsyncFunction;

class CV8Promise : public CV8PromiseBase, public Task
{
public:
    CV8Promise(CV8Isolate* pIsolate, std::unique_ptr<CV8AsyncFunction> pAsyncFunction);
    ~CV8Promise();

    void           Resolve(std::string arg);
    Local<Promise> GetPromise() const { return m_promiseResolver.Get(m_pIsolate->GetIsolate())->GetPromise(); }
    void           Run();

private:
    void Resolve(Local<Value> value);

    CV8Isolate*                       m_pIsolate;
    PersistentContext                 m_pContext;
    PersistentResolver                m_promiseResolver;
    std::unique_ptr<CV8AsyncFunction> m_pAsyncFunction;
    bool                              bHasResult = false;
    std::mutex                        m_lock;
};
