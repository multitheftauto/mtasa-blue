using namespace v8;

typedef Persistent<Promise::Resolver> PersistentResolver;
typedef Persistent<Context> PersistentContext;

class CV8Promise : public CV8PromiseBase
{
public:
    CV8Promise(Isolate* pIsolate);
    ~CV8Promise();

    void                                 Resolve(std::string arg);
    Local<Promise>                       GetPromise() const { return m_promise; }
    std::function<void(CV8PromiseBase*)> m_callback;
    bool                                 FulFill();
    bool                                 Execute();

private:
    void Resolve(v8::Local<v8::Value> value);

    Isolate*              m_pIsolate;
    Local<Promise>        m_promise;
    PersistentContext     m_pContext;
    PersistentResolver    m_promiseResolver;
    std::function<void()> m_fulfillCallback;
    bool                  bHasResult = false;
    std::mutex            m_lock;
};