#include "FunctionThread.h"
#include "RakSleep.h"

using namespace RakNet;

#ifdef _MSC_VER
#pragma warning( push )
#endif

FunctionThread::FunctorAndContext WorkerThreadFunc(FunctionThread::FunctorAndContext input, bool *returnOutput, void* perThreadData)
{
	(void) perThreadData;

	FunctionThread::FunctorAndContext output;
	input.functor->Process(input.context);
	output.functor=input.functor;
	output.context=input.context;
	*returnOutput=true;
	return output;
}
FunctionThread::FunctionThread()
{
	pr=0;
}
FunctionThread::~FunctionThread()
{
	StopThreads(false);
}
void FunctionThread::StartThreads(int numThreads)
{
	threadPool.StartThreads(numThreads, 0, 0, 0);
}
void FunctionThread::StopThreads(bool blockOnCurrentProcessing)
{
	// This ensures all waiting data is ultimately passed to a callback, so there are no leaks
	CancelInput();
	while (blockOnCurrentProcessing && threadPool.IsWorking())
	{
		CallResultHandlers();
		RakSleep(30);
	}
	threadPool.StopThreads();
}
void FunctionThread::Push(Functor *functor, void *context)
{
	FunctorAndContext input;
	input.functor=functor;
	input.context=context;
	threadPool.AddInput(WorkerThreadFunc, input);
}
void FunctionThread::CallResultHandlers(void)
{
	FunctorAndContext functorAndResult;
	while (threadPool.HasOutputFast() && threadPool.HasOutput())
	{
		functorAndResult = threadPool.GetOutput();
		functorAndResult.functor->HandleResult(false, functorAndResult.context);
		if (pr) pr(functorAndResult);
	}
}
void FunctionThread::CancelFunctorsWithContext(bool (*cancelThisFunctor)(FunctionThread::FunctorAndContext func, void *userData), void *userData)
{
	FunctorAndContext functorAndResult;
	unsigned i;
	threadPool.LockInput();
	for (i=0; i < threadPool.InputSize(); i++)
	{
		functorAndResult = threadPool.GetInputAtIndex(i);
		if (cancelThisFunctor(functorAndResult, userData))
		{
			functorAndResult.functor->HandleResult(true, functorAndResult.context);
			if (pr) pr(functorAndResult);
		}
	}
	threadPool.ClearInput();
	threadPool.UnlockInput();
}
void FunctionThread::SetPostResultFunction(void (*postResult)(FunctionThread::FunctorAndContext func))
{
	pr=postResult;
}
void FunctionThread::CancelInput(void)
{
	// We do it this way so that the callbacks get called so user-allocated data can be freed.
	FunctorAndContext functorAndResult;
	unsigned i;
	threadPool.LockInput();
	for (i=0; i < threadPool.InputSize(); i++)
	{
		functorAndResult = threadPool.GetInputAtIndex(i);
		functorAndResult.functor->HandleResult(true, functorAndResult.context);
		if (pr) pr(functorAndResult);
	}
	threadPool.ClearInput();
	threadPool.UnlockInput();
}

FunctionThreadDependentClass::FunctionThreadDependentClass()
{
	functionThreadWasAllocated=false;
	functionThread=0;
}
FunctionThreadDependentClass::~FunctionThreadDependentClass()
{
	if (functionThreadWasAllocated)
		RakNet::OP_DELETE(functionThread, __FILE__, __LINE__);
}

void FunctionThreadDependentClass::AssignFunctionThread(FunctionThread *ft)
{
	if (functionThread && functionThreadWasAllocated)
	{
		functionThread->StopThreads(true);
		RakNet::OP_DELETE(functionThread, __FILE__, __LINE__);
	}

	functionThread=ft;
	functionThreadWasAllocated=false;
}

void FunctionThreadDependentClass::StartFunctionThread(void)
{
	if (functionThread==0)
	{
		functionThread = RakNet::OP_NEW<FunctionThread>( __FILE__, __LINE__ );
		functionThreadWasAllocated=true;
	}

	functionThread->StartThreads(1);
}
FunctionThread *FunctionThreadDependentClass::GetFunctionThread(void) const
{
	return functionThread;
}
bool FunctionThreadDependentClass::GetFunctionThreadWasAllocated(void) const
{
	return functionThreadWasAllocated;
}
void FunctionThreadDependentClass::PushFunctor(Functor *functor, void *context)
{
	StartFunctionThread();
	functionThread->Push(functor, context);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
