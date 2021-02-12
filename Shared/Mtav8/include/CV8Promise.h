using namespace v8;

typedef Persistent<Promise::Resolver> PersistentResolver;
typedef Persistent<Context> PersistentContext;

class CV8Promise : public CV8PromiseBase, public Task
{
public:
    CV8Promise(CV8Isolate* pIsolate, std::unique_ptr<CV8AsyncFunction> pAsyncFunction);
    ~CV8Promise();

    void                                 Resolve(std::string arg);
    Local<Promise>                       GetPromise() const { return m_promise; }
    void                                 Run();

private:
    void Resolve(Local<Value> value);

    CV8Isolate*           m_pIsolate;
    Local<Promise>        m_promise;
    PersistentContext     m_pContext;
    PersistentResolver    m_promiseResolver;
    std::unique_ptr<CV8AsyncFunction> m_pAsyncFunction;
    std::function<void()> m_fulfillCallback;
    bool                  bHasResult = false;
    std::mutex            m_lock;
};