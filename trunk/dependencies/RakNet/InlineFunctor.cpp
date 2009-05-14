#include "InlineFunctor.h"

void InlineFunctor::HandleResult(bool wasCancelled, void *context)
{
	(void) wasCancelled;
	(void) context;
	ifp->Pop(callDepth);
}

InlineFunctorProcessor::InlineFunctorProcessor()
{

}
InlineFunctorProcessor::~InlineFunctorProcessor()
{
	StopThreads(false);
}

void InlineFunctorProcessor::StartThreads(int numThreads)
{
	functionThread.StartThreads(numThreads);
}
void InlineFunctorProcessor::StopThreads(bool blockOnCurrentProcessing)
{
	functionThread.StopThreads(blockOnCurrentProcessing);
}
void InlineFunctorProcessor::YieldOnFunctor(InlineFunctor *inlineFunctor)
{
	inlineFunctor->callDepth=GetCallDepth();
	inlineFunctor->ifp=this;
	functionThread.Push(inlineFunctor);
	completedThreads.Push(false, __FILE__, __LINE__);
}
bool InlineFunctorProcessor::UpdateIFP(void)
{
	functionThread.CallResultHandlers();
	if (completedThreads.Size() && completedThreads[completedThreads.Size()-1]==true)
	{
		completedThreads.Pop();
		return true;
	}
	return false;
}
void InlineFunctorProcessor::Pop(int threadCallDepth)
{
	completedThreads[threadCallDepth]=true;
}
unsigned InlineFunctorProcessor::GetCallDepth(void) const
{
	return completedThreads.Size();
}
