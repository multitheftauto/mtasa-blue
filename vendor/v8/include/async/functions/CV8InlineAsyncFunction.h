#include "../CV8AsyncFunction.h"
#include <functional>

class CV8InlineAsyncFunction : public CV8AsyncFunction
{
public:
    CV8InlineAsyncFunction(std::function<void(CV8DelegateBase*)> run) : m_run(run) {
    }

    void Run(CV8DelegateBase* delegate) { m_run(delegate); }
    void Cancel(){};

private:
    std::function<void(CV8DelegateBase*)> m_run;
};
