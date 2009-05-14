/// \file
/// \brief A set of classes to make it easier to perform asynchronous function processing.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __FUNCTION_THREAD_H
#define __FUNCTION_THREAD_H

#include "SingleProducerConsumer.h"
#include "ThreadPool.h"
#include "RakMemoryOverride.h"
#include "Export.h"

namespace RakNet
{

// Forward declarations
class Functor;

/// FunctionThread takes a stream of classes that implement a processing function, processes them in a thread, and calls a callback with the result.
/// It's a useful way to call blocking functions that you do not want to block, such as file writes and database operations.
class RAK_DLL_EXPORT FunctionThread
{
public:
	FunctionThread();
	~FunctionThread();

	struct FunctorAndContext
	{
		Functor *functor;
		void *context;
	};

	/// Starts the thread up.
	void StartThreads(int numThreads);

	/// Stop processing. Will also call FunctorResultHandler callbacks with /a wasCancelled set to true.
	/// \param[in] blockOnCurrentProcessing Wait for the current processing to finish?
	void StopThreads(bool blockOnCurrentProcessing);

	/// Add a functor to the incoming stream of functors
	/// \note functor MUST be a valid pointer until Functor::HandleResult() is called, at which point the pointer is returned to you.
	/// \note For practical purposes this means the instance of functor you pass to this function has to be allocated using new and delete.
	/// \note You should deallocate the pointer inside Functor::HandleResult() 
	/// \param[in] functor A pointer to an implemented Functor class
	/// \param[in] If there is some context to this functor you want to look up to cancel it, you can set it here. Returned back to you in Functor::HandleResult
	void Push(Functor *functor, void *context=0);

	/// Call FunctorResultHandler callbacks
	/// Normally you would call this once per update cycle, although you do not have to.
	void CallResultHandlers(void);

	/// If you want to cancel input and output functors associated with some context, you can pass a function to do that here
	/// \param[in] cancelThisFunctor Function should return true to cancel the functor, false to let it process
	/// \param[in] userData Pointer to whatever you want. Passed to the cancelThisFunctor call
	void CancelFunctorsWithContext(bool (*cancelThisFunctor)(FunctorAndContext func, void *userData), void *userData);

	/// If you want to automatically do some kind of processing on every functor after Functor::HandleResult is called, set it here.
	/// Useful to cleanup FunctionThread::Push::context
	/// \param[in] postResult pointer to a C function to do post-processing 
	void SetPostResultFunction(void (*postResult)(FunctorAndContext func));


protected:
	void CancelInput(void);
	ThreadPool<FunctorAndContext, FunctorAndContext> threadPool;

	void (*pr)(FunctorAndContext func);
};

/// A functor is a single unit of processing to send to the Function thread.
/// Derive from it, add your data, and implement the processing function.
class Functor
{
public:
	Functor() {}
	virtual ~Functor() {}

	/// Do whatever processing you want.
	/// \param[in] context pointer passed to FunctionThread::Push::context
	virtual void Process(void *context)=0;
	/// Called from FunctionThread::CallResultHandlers with wasCancelled false OR
	/// Called from FunctionThread::StopThread or FunctionThread::~FunctionThread with wasCancelled true
	/// \param[in] wasCancelledTrue if CallResultHandlers was called, false if StopThreads or CancelInputWithContext was called before Functor::Process()
	/// \param[in] context pointer passed to FunctionThread::Push::context
	virtual void HandleResult(bool wasCancelled, void *context)=0;
};

class RAK_DLL_EXPORT FunctionThreadDependentClass
{
public:
	FunctionThreadDependentClass();
	virtual ~FunctionThreadDependentClass();

	/// Assigns a function thread to process asynchronous calls. If you do not assign one then one will be created automatically.
	/// \param[in] ft An instance of a running function thread class. This class can be shared and used for other functors as well.
	virtual void AssignFunctionThread(FunctionThread *ft);

	/// \return Returns the function thread held in the class
	FunctionThread *GetFunctionThread(void) const;

	/// \returns Whether or not this class allocated the function thread by itself
	bool GetFunctionThreadWasAllocated(void) const;

	/// Allocates and starts the thread if needed, and pushes the functor
	/// \param[in] functor Functor to push
	/// \param[in] context Sent to FunctionThread::Push::context
	virtual void PushFunctor(Functor *functor, void *context=0);
protected:
	/// Allocates and starts the function thread, if necessary
	void StartFunctionThread();
	FunctionThread *functionThread;
	bool functionThreadWasAllocated;
};

}

#endif
