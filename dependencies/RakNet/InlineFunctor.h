#include "FunctionThread.h"
#include "DS_List.h"

class InlineFunctorProcessor;

/// A base class to derive functors from for the InlineFunctorProcessor system
class InlineFunctor : public RakNet::Functor
{
protected:
	/// \internal - Calls InlineFunctorProcessor to signal that the functor is done
	virtual void HandleResult(bool wasCancelled, void *context);

	/// Tracks the call depth when this functor was pushed. It allows the system to return from functions in the correct order
	int callDepth;

	/// Pointer to the calling instance of InlineFunctorProcessor
	InlineFunctorProcessor *ifp;
	friend class InlineFunctorProcessor;
};

/// A base class that will allow you to call YieldOnFunctor() from within a function, and continue with that function when the asynchronous processing has completed
class InlineFunctorProcessor
{
public:
	InlineFunctorProcessor();
	~InlineFunctorProcessor();

	/// Start the threads. Should call this first
	/// \param[in] numThreads How many worker threads to start
	/// \note If only one thread is started, then the calls to YieldOnFunctor will process in that order
	void StartThreads(int numThreads);

	/// Stop the threads
	/// \param[in] blockOnCurrentProcessing Wait for the current processing to finish?
	void StopThreads(bool blockOnCurrentProcessing);

	/// Yield processing in the current function, continuing with the function implemented by CallYieldFunction
	/// When the functor completes, this function will return and the caller will continue processing
	/// \param[in] inlineFunctor A class that implements Functor::Process() to perform processing that can work asynchronously, such as loading a file or doing a database call
	void YieldOnFunctor(InlineFunctor *inlineFunctor);

	/// \internal
	/// If the functor is done, continue processing the caller
	/// \return True if the topmost functor has completed, false otherwise
	bool UpdateIFP(void);

	/// \internal
	/// Notify the caller that the functor is done
	void Pop(int threadCallDepth);

protected:

	/// Returns the number of functors that were passed to the system
	unsigned GetCallDepth(void) const;

	/// Used to create a thread that processes functors
	RakNet::FunctionThread functionThread;

	/// Tracks which threads have been completed
	DataStructures::List<bool> completedThreads;
};
