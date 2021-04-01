using namespace v8;

class CV8Task : public Task
{
public:
    CV8Task(std::shared_ptr<CV8Isolate::CancelationToken> pCancelationToken, std::function<void()> task) : m_pCancelationToken(pCancelationToken), m_task(task)
    {
    }
    void Run()
    {
        if (!m_pCancelationToken->IsCanceled())
        {
            m_task();
        }
    };

private:
    std::function<void()>                         m_task;
    std::mutex                                    m_lock;
    std::shared_ptr<CV8Isolate::CancelationToken> m_pCancelationToken;
};
