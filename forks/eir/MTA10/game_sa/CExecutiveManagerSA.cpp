/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExecutiveManagerSA.cpp
*  PURPOSE:     MTA thread and fiber execution manager for workload smoothing
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static void __cdecl _FiberTerm( FiberStatus *status )
{
    CFiberSA *fiber = (CFiberSA*)status;

    fiber->manager->TerminateFiber( fiber );
}

static void __stdcall _FiberProc( FiberStatus *status )
{
    CFiberSA *fiber = (CFiberSA*)status;

    try
    {
        fiber->yield();

        fiber->callback( fiber, fiber->userdata );
    }
    catch( fiberTerminationException& )
    {
        // The runtime requested the fiber to terminate.
    }
    catch( ... )
    {
        // An unexpected error was triggered.
    }
}

static void* fiberMemAllocate( size_t memSize )
{
    return malloc( memSize );
}

static void fiberMemFree( void *ptr )
{
    return free( ptr );
}

CExecutiveManagerSA::CExecutiveManagerSA( void )
{
    LIST_CLEAR( fibers.root );
    LIST_CLEAR( groups.root );

    // Set up runtime callbacks.
    ExecutiveFiber::setmemfuncs( fiberMemAllocate, fiberMemFree );

    defGroup = new CExecutiveGroupSA( this );

    frameTime = ExecutiveManager::GetPerformanceTimer();
    frameDuration = 0;
}

CExecutiveManagerSA::~CExecutiveManagerSA( void )
{
    // Destroy all groups.
    while ( !LIST_EMPTY( groups.root ) )
    {
        CExecutiveGroupSA *group = LIST_GETITEM( CExecutiveGroupSA, groups.root.next, managerNode );

        delete group;
    }

    // Destroy all executive runtimes.
    while ( !LIST_EMPTY( fibers.root ) )
    {
        CFiberSA *fiber = LIST_GETITEM( CFiberSA, fibers.root.next, node );

        CloseFiber( fiber );
    }
}

CFiberSA* CExecutiveManagerSA::CreateFiber( CFiberSA::fiberexec_t proc, void *userdata )
{
    CFiberSA *fiber = new CFiberSA( this, NULL, NULL );

    Fiber *runtime = ExecutiveFiber::newfiber( fiber, 0, _FiberProc, _FiberTerm );

    // Set first step into it, so the fiber can set itself up.
    fiber->runtime = runtime;
    fiber->resume();

    // Set the function that should be executed next cycle.
    fiber->callback = proc;
    fiber->userdata = userdata;

    // Add it to our manager list.
    LIST_APPEND( fibers.root, fiber->node );

    // Add it to the default fiber group.
    // The user can add it to another if he wants.
    defGroup->AddFiberNative( fiber );

    // Return it.
    return fiber;
}

static void _FiberExceptTerminate( CFiberSA *fiber )
{
    throw fiberTerminationException( fiber );
}

void CExecutiveManagerSA::TerminateFiber( CFiberSA *fiber )
{
    if ( !fiber->runtime )
        return;

    Fiber *env = fiber->runtime;

    if ( fiber->status != FIBER_TERMINATED )
    {
        // Throw an exception on the fiber
        env->pushdata( fiber );
        env->eip = (unsigned int)_FiberExceptTerminate;

        // We want to eventually return back
        fiber->resume();
    }
	else
	{
		// Threads clean their environments after themselves
        ExecutiveFiber::closefiber( env );

		fiber->runtime = NULL;
	}
}

void CExecutiveManagerSA::CloseFiber( CFiberSA *fiber )
{
    TerminateFiber( fiber );

    LIST_REMOVE( fiber->node );
    LIST_REMOVE( fiber->groupNode );

    delete fiber;
}

CExecutiveGroupSA* CExecutiveManagerSA::CreateGroup( void )
{
    CExecutiveGroupSA *group = new CExecutiveGroupSA( this );

    return group;
}

void CExecutiveManagerSA::DoPulse( void )
{
    {
        HighPrecisionMathWrap mathWrap;

        // Update frame timer.
        double timeNow = ExecutiveManager::GetPerformanceTimer();

        frameDuration = timeNow - frameTime;
        frameTime = timeNow;
    }

    LIST_FOREACH_BEGIN( CExecutiveGroupSA, groups.root, managerNode )
        item->DoPulse();
    LIST_FOREACH_END
}