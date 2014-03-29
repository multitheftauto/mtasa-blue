/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExecutiveManagerSA.fiber.cpp
*  PURPOSE:     Executive manager fiber logic
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#ifdef _WIN32
#include <excpt.h>
#include <windows.h>

typedef struct _EXCEPTION_REGISTRATION
{
     struct _EXCEPTION_REGISTRATION* Next;
     void *Handler;
} EXCEPTION_REGISTRATION, *PEXCEPTION_REGISTRATION;

static EXCEPTION_DISPOSITION _defaultHandler( EXCEPTION_REGISTRATION *record, void *frame, CONTEXT *context, void *dispatcher )
{
    exit( 0x0DAB00B5 );
    return ExceptionContinueSearch;
}

static EXCEPTION_REGISTRATION _baseException =
{
    (EXCEPTION_REGISTRATION*)0xFFFFFFFF,
    _defaultHandler
};
#endif

#pragma warning(disable:4733)

static void __declspec(naked) _retHandler( FiberStatus *userdata )
{
	__asm
	{
		// Set thread status to terminated
		mov eax,[esp+4]
        mov [eax]FiberStatus.status,FIBER_TERMINATED

        // Cache the termination function
        mov edx,[eax]FiberStatus.termcb

		mov ecx,eax

        // Apply registers
        mov eax,[eax]FiberStatus.callee
        mov ebx,[eax]Fiber.ebx
        mov edi,[eax]Fiber.edi
        mov esi,[eax]Fiber.esi
        mov esp,[eax]Fiber.esp
        mov ebp,[eax]Fiber.ebp

        push [eax]Fiber.eip // The return address
		push ecx			// Push userdata as preparation

#ifdef _WIN32
        // Save the termination function.
        push edx

        // Apply exception and stack info
        mov ecx,[eax]Fiber.stack_base
        mov edx,[eax]Fiber.stack_limit
        mov fs:[4],ecx
        mov fs:[8],edx
        mov ecx,[eax]Fiber.except_info
        mov fs:[0],ecx

        // Pop the termination function.
        pop edx
#endif

		// Terminate our thread
        call edx
        add esp,4

        ret
	}
}

// Global memory allocation functions.
static ExecutiveFiber::memalloc_t fiberMemAlloc = NULL;
static ExecutiveFiber::memfree_t fiberMemFree = NULL;

Fiber* ExecutiveFiber::newfiber( FiberStatus *userdata, size_t stackSize, FiberProcedure proc, FiberStatus::termfunc_t termcb )
{
    Fiber *env = (Fiber*)fiberMemAlloc( sizeof(Fiber) );

    // Allocate stack memory
    if ( stackSize == 0 )
        stackSize = 2 << 17;    // about 128 kilobytes of stack space (is this enough?)

    unsigned int *stack = (unsigned int*)fiberMemAlloc( stackSize );
    env->stack_base = (unsigned int*)( (unsigned int)stack + stackSize );
    env->stack_limit = stack;

    stack = env->stack_base;

    // Once entering, the first argument should be the thread
    *--stack = (unsigned int)userdata;
    *--stack = (unsigned int)exit;
    *--stack = (unsigned int)userdata;
    *--stack = (unsigned int)&_retHandler;
    env->esp = (unsigned int)stack;

    env->eip = (unsigned int)proc;
    
#ifdef _WIN32
    env->except_info = &_baseException;
#endif

    userdata->termcb = termcb;

    env->stackSize = stackSize;
    return env;
}

Fiber* ExecutiveFiber::makefiber( void )
{
    Fiber *fiber = (Fiber*)fiberMemAlloc( sizeof(Fiber) );
    fiber->stackSize = 0;
    return fiber;
}

void ExecutiveFiber::closefiber( Fiber *fiber )
{
    if ( fiber->stackSize )
        fiberMemFree( fiber->stack_limit );

    fiberMemFree( fiber );
}

void ExecutiveFiber::setmemfuncs( ExecutiveFiber::memalloc_t malloc, ExecutiveFiber::memfree_t mfree )
{
    fiberMemAlloc = malloc;
    fiberMemFree = mfree;
}

void __declspec(naked) ExecutiveFiber::eswitch( Fiber *from, Fiber *to )
{
    __asm
    {
        // Save current environment
        mov eax,[esp+4]
        mov [eax]Fiber.ebx,ebx
        mov [eax]Fiber.edi,edi
        mov [eax]Fiber.esi,esi
        add esp,4
        mov [eax]Fiber.esp,esp
        mov ebx,[esp-4]
        mov [eax]Fiber.eip,ebx
        mov [eax]Fiber.ebp,ebp

#ifdef _WIN32
        // Save exception and stack info
        mov ebx,fs:[0]
        mov ecx,fs:[4]
        mov edx,fs:[8]
        mov [eax]Fiber.stack_base,ecx
        mov [eax]Fiber.stack_limit,edx
        mov [eax]Fiber.except_info,ebx
#endif

        // Apply registers
        mov eax,[esp+4]
        mov ebx,[eax]Fiber.ebx
        mov edi,[eax]Fiber.edi
        mov esi,[eax]Fiber.esi
        mov esp,[eax]Fiber.esp
        mov ebp,[eax]Fiber.ebp

#ifdef _WIN32
        // Apply exception and stack info
        mov ecx,[eax]Fiber.stack_base
        mov edx,[eax]Fiber.stack_limit
        mov fs:[4],ecx
        mov fs:[8],edx
        mov ecx,[eax]Fiber.except_info
        mov fs:[0],ecx
#endif

        jmp dword ptr[eax]Fiber.eip
    }
}

// For use with yielding
void __declspec(naked) ExecutiveFiber::qswitch( Fiber *from, Fiber *to )
{
    __asm
    {
        // Save current environment
        mov eax,[esp+4]
        mov [eax]Fiber.ebx,ebx
        mov [eax]Fiber.edi,edi
        mov [eax]Fiber.esi,esi
        add esp,4
        mov [eax]Fiber.esp,esp
        mov ebx,[esp-4]
        mov [eax]Fiber.eip,ebx
        mov [eax]Fiber.ebp,ebp

#ifdef _WIN32
        // Save exception info
        mov ebx,fs:[0]
        mov [eax]Fiber.except_info,ebx
#endif

        // Apply registers
        mov eax,[esp+4]
        mov ebx,[eax]Fiber.ebx
        mov edi,[eax]Fiber.edi
        mov esi,[eax]Fiber.esi
        mov esp,[eax]Fiber.esp
        mov ebp,[eax]Fiber.ebp

#ifdef _WIN32
        // Apply exception and stack info
        mov ecx,[eax]Fiber.stack_base
        mov edx,[eax]Fiber.stack_limit
        mov fs:[4],ecx
        mov fs:[8],edx
        mov ecx,[eax]Fiber.except_info
        mov fs:[0],ecx
#endif

        jmp dword ptr[eax]Fiber.eip
    }
}

void CFiberSA::yield_proc( void )
{
    bool needYield = false;
    {
        // We need very high precision math here.
        HighPrecisionMathWrap mathWrap;
        
        // Do some test with timing.
        double currentTimer = ExecutiveManager::GetPerformanceTimer();
        double perfMultiplier = group->perfMultiplier;

        needYield = ( currentTimer - resumeTimer ) > manager->GetFrameDuration() * perfMultiplier;
    }

    if ( needYield )
        yield();
}