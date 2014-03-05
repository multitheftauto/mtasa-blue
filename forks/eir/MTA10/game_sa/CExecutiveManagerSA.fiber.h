/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExecutiveManagerSA.fiber.h
*  PURPOSE:     Executive manager fiber logic
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _EXECUTIVE_MANAGER_FIBER_
#define _EXECUTIVE_MANAGER_FIBER_

struct FiberStatus;

struct Fiber
{
    // Preserve __cdecl
    unsigned int ebx;   // 0
    unsigned int edi;   // 4
    unsigned int esi;   // 8
    unsigned int esp;   // 12
    unsigned int eip;   // 16
    unsigned int ebp;   // 20
    
    unsigned int *stack_base, *stack_limit;
    void *except_info;

    size_t stackSize;

    // Stack manipulation routines.
    template <typename dataType>
    inline void pushdata( const dataType& data )
    {
        *--((dataType*&)esp) = data;
    }
};

enum eFiberStatus
{
    FIBER_RUNNING,
    FIBER_SUSPENDED,
    FIBER_TERMINATED
};

struct FiberStatus
{
    Fiber *callee;      // yielding information

    typedef void (__cdecl*termfunc_t)( FiberStatus *userdata );

    termfunc_t termcb;  // function called when fiber terminates

    eFiberStatus status;
};

typedef void    (__stdcall*FiberProcedure) ( FiberStatus *status );

namespace ExecutiveFiber
{
    // Native assembler methods.
    Fiber*          newfiber        ( FiberStatus *userdata, size_t stackSize, FiberProcedure proc, FiberStatus::termfunc_t termcb );
    Fiber*          makefiber       ( void );
    void            closefiber      ( Fiber *env );

    typedef void* (__cdecl*memalloc_t)( size_t memSize );
    typedef void (__cdecl*memfree_t)( void *ptr );

    void            setmemfuncs     ( memalloc_t malloc, memfree_t mfree );

    void __cdecl    eswitch         ( Fiber *from, Fiber *to );
    void __cdecl    qswitch         ( Fiber *from, Fiber *to );
};

class CFiberSA : public FiberStatus
{
public:
    friend class CExecutiveManagerSA;
    friend class CExecutiveGroupSA;

    Fiber *runtime;                 // storing Fiber runtime context
    void *userdata;

    typedef void (__stdcall*fiberexec_t)( CFiberSA *fiber, void *userdata );

    fiberexec_t callback;           // routine set by the fiber request.

    RwListEntry <CFiberSA> node;        // node in fiber manager
    RwListEntry <CFiberSA> groupNode;   // node in fiber group

    CExecutiveGroupSA *group;       // fiber group this fiber is in.

    CFiberSA( CExecutiveManagerSA *manager, CExecutiveGroupSA *group, Fiber *runtime )
    {
        // Using "this" is actually useful.
        this->runtime = runtime;
        this->callee = ExecutiveFiber::makefiber();

        this->manager = manager;
        this->group = group;

        status = FIBER_SUSPENDED;
    }

    // Native functions that skip manager activity.
    inline void resume( void )
    {
        if ( status == FIBER_SUSPENDED )
        {
            // Save the time that we resumed from this.
            resumeTimer = ExecutiveManager::GetPerformanceTimer();

            status = FIBER_RUNNING;

            ExecutiveFiber::eswitch( callee, runtime );
        }
    }

    inline void yield( void )
    {
        // WARNING: only call this function from the fiber stack!
        status = FIBER_SUSPENDED;

        ExecutiveFiber::qswitch( runtime, callee );
    }

    // Managed methods that use logic.
    void yield_proc( void );

    CExecutiveManagerSA *manager;

    double resumeTimer;
};

#endif //_EXECUTIVE_MANAGER_FIBER_