using namespace v8;

class CV8TaskDelegate
{

};

template <typename T>
class CV8Task : public Task
{
public:
    CV8Task(CV8Isolate* pIsolate, CV8Promise* pPrimise, std::function<T()> task) : m_pIsolate(pIsolate), m_pPromise(pPrimise), m_task(task) {}

    ~CV8Task(){};
    void Run(){};

private:
    CV8Isolate*        m_pIsolate;
    CV8Promise*        m_pPromise;
    std::function<T()> m_task;
    std::mutex         m_lock;
};