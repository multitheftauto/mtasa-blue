//////////////////////////////////////////////////////////////////////////////
//
//  Detours Disassembler (disasm.cpp of detours.lib)
//
//  Microsoft Research Detours Package, Version 3.0 Build_339.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#if _MSC_VER >= 1900
#pragma warning(push)
#pragma warning(disable:4091) // empty typedef
#endif

#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1
#include <windows.h>
#include <limits.h>

// #define DETOUR_DEBUG 1
#define DETOURS_INTERNAL

#include "detours.h"

#if DETOURS_VERSION != 30001
#error detours.h version mismatch
#endif

#if _MSC_VER >= 1900
#pragma warning(pop)
#endif

#undef ASSERT
#define ASSERT(x)

//////////////////////////////////////////////////////////////////////////////
//
//  Special macros to handle the case when we are building disassembler for
//  offline processing.
//


#if defined(DETOURS_X86_OFFLINE_LIBRARY) \
 || defined(DETOURS_X64_OFFLINE_LIBRARY) \
 || defined(DETOURS_ARM_OFFLINE_LIBRARY) \
 || defined(DETOURS_ARM64_OFFLINE_LIBRARY) \
 || defined(DETOURS_IA64_OFFLINE_LIBRARY)

#undef DETOURS_X64
#undef DETOURS_X86
#undef DETOURS_IA64
#undef DETOURS_ARM
#undef DETOURS_ARM64

#if defined(DETOURS_X86_OFFLINE_LIBRARY)

#define DetourCopyInstruction   DetourCopyInstructionX86
#define DetourSetCodeModule     DetourSetCodeModuleX86
#define CDetourDis              CDetourDisX86
#define DETOURS_X86

#elif defined(DETOURS_X64_OFFLINE_LIBRARY)

#if !defined(DETOURS_64BIT)
// Fix this as/if bugs are discovered.
//#error X64 disassembler can only build for 64-bit.
#endif

#define DetourCopyInstruction   DetourCopyInstructionX64
#define DetourSetCodeModule     DetourSetCodeModuleX64
#define CDetourDis              CDetourDisX64
#define DETOURS_X64

#elif defined(DETOURS_ARM_OFFLINE_LIBRARY)

#define DetourCopyInstruction   DetourCopyInstructionARM
#define DetourSetCodeModule     DetourSetCodeModuleARM
#define CDetourDis              CDetourDisARM
#define DETOURS_ARM

#elif defined(DETOURS_ARM64_OFFLINE_LIBRARY)

#define DetourCopyInstruction   DetourCopyInstructionARM64
#define DetourSetCodeModule     DetourSetCodeModuleARM64
#define CDetourDis              CDetourDisARM64
#define DETOURS_ARM64

#elif defined(DETOURS_IA64_OFFLINE_LIBRARY)

#define DetourCopyInstruction   DetourCopyInstructionIA64
#define DetourSetCodeModule     DetourSetCodeModuleIA64
#define DETOURS_IA64

#else

#error

#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  Function:
//      DetourCopyInstruction(PVOID pDst,
//                            PVOID *ppDstPool
//                            PVOID pSrc,
//                            PVOID *ppTarget,
//                            LONG *plExtra)
//  Purpose:
//      Copy a single instruction from pSrc to pDst.
//
//  Arguments:
//      pDst:
//          Destination address for the instruction.  May be NULL in which
//          case DetourCopyInstruction is used to measure an instruction.
//          If not NULL then the source instruction is copied to the
//          destination instruction and any relative arguments are adjusted.
//      ppDstPool:
//          Destination address for the end of the constant pool.  The
//          constant pool works backwards toward pDst.  All memory between
//          pDst and *ppDstPool must be available for use by this function.
//          ppDstPool may be NULL if pDst is NULL.
//      pSrc:
//          Source address of the instruction.
//      ppTarget:
//          Out parameter for any target instruction address pointed to by
//          the instruction.  For example, a branch or a jump insruction has
//          a target, but a load or store instruction doesn't.  A target is
//          another instruction that may be executed as a result of this
//          instruction.  ppTarget may be NULL.
//      plExtra:
//          Out parameter for the number of extra bytes needed by the
//          instruction to reach the target.  For example, lExtra = 3 if the
//          instruction had an 8-bit relative offset, but needs a 32-bit
//          relative offset.
//
//  Returns:
//      Returns the address of the next instruction (following in the source)
//      instruction.  By subtracting pSrc from the return value, the caller
//      can determinte the size of the instruction copied.
//
//  Comments:
//      By following the pTarget, the caller can follow alternate
//      instruction streams.  However, it is not always possible to determine
//      the target based on static analysis.  For example, the destination of
//      a jump relative to a register cannot be determined from just the
//      instruction stream.  The output value, pTarget, can have any of the
//      following outputs:
//          DETOUR_INSTRUCTION_TARGET_NONE:
//              The instruction has no targets.
//          DETOUR_INSTRUCTION_TARGET_DYNAMIC:
//              The instruction has a non-deterministic (dynamic) target.
//              (i.e. the jump is to an address held in a register.)
//          Address:   The instruction has the specified target.
//
//      When copying instructions, DetourCopyInstruction insures that any
//      targets remain constant.  It does so by adjusting any IP relative
//      offsets.
//

#pragma data_seg(".detourd")
#pragma const_seg(".detourc")

//////////////////////////////////////////////////// X86 and X64 Disassembler.
//
//  Includes full support for all x86 chips prior to the Pentium III, and some newer stuff.
//
#if defined(DETOURS_X64) || defined(DETOURS_X86)

class CDetourDis
{
  public:
    CDetourDis(_Out_opt_ PBYTE *ppbTarget,
               _Out_opt_ LONG *plExtra);

    PBYTE   CopyInstruction(PBYTE pbDst, PBYTE pbSrc);
    static BOOL SanityCheckSystem();
    static BOOL SetCodeModule(PBYTE pbBeg, PBYTE pbEnd, BOOL fLimitReferencesToModule);

  public:
    struct COPYENTRY;
    typedef const COPYENTRY * REFCOPYENTRY;

    typedef PBYTE (CDetourDis::* COPYFUNC)(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);

    // nFlagBits flags.
    enum {
        DYNAMIC     = 0x1u,
        ADDRESS     = 0x2u,
        NOENLARGE   = 0x4u,
        RAX         = 0x8u,
    };

    // ModR/M Flags
    enum {
        SIB         = 0x10u,
        RIP         = 0x20u,
        NOTSIB      = 0x0fu,
    };

    struct COPYENTRY
    {
        // Many of these fields are often ignored. See ENTRY_DataIgnored.
        ULONG       nOpcode         : 8;    // Opcode (ignored)
        ULONG       nFixedSize      : 4;    // Fixed size of opcode
        ULONG       nFixedSize16    : 4;    // Fixed size when 16 bit operand
        ULONG       nModOffset      : 4;    // Offset to mod/rm byte (0=none)
        ULONG       nRelOffset      : 4;    // Offset to relative target.
        ULONG       nTargetBack     : 4;    // Offset back to absolute or rip target
        ULONG       nFlagBits       : 4;    // Flags for DYNAMIC, etc.
        COPYFUNC    pfCopy;                 // Function pointer.
    };

  protected:
    // These macros define common uses of nFixedSize..pfCopy.
#define ENTRY_DataIgnored           0, 0, 0, 0, 0, 0,
#define ENTRY_CopyBytes1            1, 1, 0, 0, 0, 0, &CDetourDis::CopyBytes
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
#define ENTRY_CopyBytes1Address     5, 3, 0, 0, 0, ADDRESS, &CDetourDis::CopyBytes
#endif
#define ENTRY_CopyBytes1Dynamic     1, 1, 0, 0, 0, DYNAMIC, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2            2, 2, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2Jump        ENTRY_DataIgnored &CDetourDis::CopyBytesJump
#define ENTRY_CopyBytes2CantJump    2, 2, 0, 1, 0, NOENLARGE, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2Dynamic     2, 2, 0, 0, 0, DYNAMIC, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3            3, 3, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3Dynamic     3, 3, 0, 0, 0, DYNAMIC, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3Or5         5, 3, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3Or5Dynamic  5, 3, 0, 0, 0, DYNAMIC, &CDetourDis::CopyBytes // x86 only
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
#define ENTRY_CopyBytes3Or5Rax      5, 3, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3Or5Target   5, 3, 0, 1, 0, 0, &CDetourDis::CopyBytes
#endif
#define ENTRY_CopyBytes4            4, 4, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes5            5, 5, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes5Or7Dynamic  7, 5, 0, 0, 0, DYNAMIC, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes7            7, 7, 0, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2Mod         2, 2, 1, 0, 0, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2ModDynamic  2, 2, 1, 0, 0, DYNAMIC, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2Mod1        3, 3, 1, 0, 1, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes2ModOperand  6, 4, 1, 0, 4, 0, &CDetourDis::CopyBytes
#define ENTRY_CopyBytes3Mod         3, 3, 2, 0, 0, 0, &CDetourDis::CopyBytes // SSE3 0F 38 opcode modrm
#define ENTRY_CopyBytes3Mod1        4, 4, 2, 0, 1, 0, &CDetourDis::CopyBytes // SSE3 0F 3A opcode modrm .. imm8
#define ENTRY_CopyBytesPrefix       ENTRY_DataIgnored &CDetourDis::CopyBytesPrefix
#define ENTRY_CopyBytesSegment      ENTRY_DataIgnored &CDetourDis::CopyBytesSegment
#define ENTRY_CopyBytesRax          ENTRY_DataIgnored &CDetourDis::CopyBytesRax
#define ENTRY_CopyF2                ENTRY_DataIgnored &CDetourDis::CopyF2
#define ENTRY_CopyF3                ENTRY_DataIgnored &CDetourDis::CopyF3   // 32bit x86 only
#define ENTRY_Copy0F                ENTRY_DataIgnored &CDetourDis::Copy0F
#define ENTRY_Copy0F78              ENTRY_DataIgnored &CDetourDis::Copy0F78
#define ENTRY_Copy0F00              ENTRY_DataIgnored &CDetourDis::Copy0F00 // 32bit x86 only
#define ENTRY_Copy0FB8              ENTRY_DataIgnored &CDetourDis::Copy0FB8 // 32bit x86 only
#define ENTRY_Copy66                ENTRY_DataIgnored &CDetourDis::Copy66
#define ENTRY_Copy67                ENTRY_DataIgnored &CDetourDis::Copy67
#define ENTRY_CopyF6                ENTRY_DataIgnored &CDetourDis::CopyF6
#define ENTRY_CopyF7                ENTRY_DataIgnored &CDetourDis::CopyF7
#define ENTRY_CopyFF                ENTRY_DataIgnored &CDetourDis::CopyFF
#define ENTRY_CopyVex2              ENTRY_DataIgnored &CDetourDis::CopyVex2
#define ENTRY_CopyVex3              ENTRY_DataIgnored &CDetourDis::CopyVex3
#define ENTRY_Invalid               ENTRY_DataIgnored &CDetourDis::Invalid
#define ENTRY_End                   ENTRY_DataIgnored NULL

    PBYTE CopyBytes(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyBytesPrefix(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyBytesSegment(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyBytesRax(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyBytesJump(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);

    PBYTE Invalid(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);

    PBYTE AdjustTarget(PBYTE pbDst, PBYTE pbSrc, UINT cbOp,
                       UINT cbTargetOffset, UINT cbTargetSize);

  protected:
    PBYTE Copy0F(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE Copy0F00(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc); // x86 only sldt/0 str/1 lldt/2 ltr/3 err/4 verw/5 jmpe/6/dynamic invalid/7
    PBYTE Copy0F78(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc); // vmread, 66/extrq/ib/ib, F2/insertq/ib/ib
    PBYTE Copy0FB8(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc); // jmpe or F3/popcnt
    PBYTE Copy66(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE Copy67(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyF2(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyF3(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc); // x86 only
    PBYTE CopyF6(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyF7(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyFF(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyVex2(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyVex3(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc);
    PBYTE CopyVexCommon(BYTE m, PBYTE pbDst, PBYTE pbSrc);

  protected:
    static const COPYENTRY  s_rceCopyTable[257];
    static const COPYENTRY  s_rceCopyTable0F[257];
    static const BYTE       s_rbModRm[256];
    static PBYTE            s_pbModuleBeg;
    static PBYTE            s_pbModuleEnd;
    static BOOL             s_fLimitReferencesToModule;

  protected:
    BOOL                m_bOperandOverride;
    BOOL                m_bAddressOverride;
    BOOL                m_bRaxOverride; // AMD64 only
    BOOL                m_bVex;
    BOOL                m_bF2;
    BOOL                m_bF3; // x86 only
    BYTE                m_nSegmentOverride;

    PBYTE *             m_ppbTarget;
    LONG *              m_plExtra;

    LONG                m_lScratchExtra;
    PBYTE               m_pbScratchTarget;
    BYTE                m_rbScratchDst[64];
};

PVOID WINAPI DetourCopyInstruction(_In_opt_ PVOID pDst,
                                   _Inout_opt_ PVOID *ppDstPool,
                                   _In_ PVOID pSrc,
                                   _Out_opt_ PVOID *ppTarget,
                                   _Out_opt_ LONG *plExtra)
{
    UNREFERENCED_PARAMETER(ppDstPool);  // x86 & x64 don't use a constant pool.

    CDetourDis oDetourDisasm((PBYTE*)ppTarget, plExtra);
    return oDetourDisasm.CopyInstruction((PBYTE)pDst, (PBYTE)pSrc);
}

/////////////////////////////////////////////////////////// Disassembler Code.
//
CDetourDis::CDetourDis(_Out_opt_ PBYTE *ppbTarget, _Out_opt_ LONG *plExtra)
{
    m_bOperandOverride = FALSE;
    m_bAddressOverride = FALSE;
    m_bRaxOverride = FALSE;
    m_bF2 = FALSE;
    m_bF3 = FALSE;
    m_bVex = FALSE;

    m_ppbTarget = ppbTarget ? ppbTarget : &m_pbScratchTarget;
    m_plExtra = plExtra ? plExtra : &m_lScratchExtra;

    *m_ppbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_NONE;
    *m_plExtra = 0;
}

PBYTE CDetourDis::CopyInstruction(PBYTE pbDst, PBYTE pbSrc)
{
    // Configure scratch areas if real areas are not available.
    if (NULL == pbDst) {
        pbDst = m_rbScratchDst;
    }
    if (NULL == pbSrc) {
        // We can't copy a non-existent instruction.
        SetLastError(ERROR_INVALID_DATA);
        return NULL;
    }

    // Figure out how big the instruction is, do the appropriate copy,
    // and figure out what the target of the instruction is if any.
    //
    REFCOPYENTRY pEntry = &s_rceCopyTable[pbSrc[0]];
    return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyBytes(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    UINT nBytesFixed;

    ASSERT(!m_bVex || pEntry->nFlagBits == 0);
    ASSERT(!m_bVex || pEntry->nFixedSize == pEntry->nFixedSize16);

    UINT const nModOffset = pEntry->nModOffset;
    UINT const nFlagBits = pEntry->nFlagBits;
    UINT const nFixedSize = pEntry->nFixedSize;
    UINT const nFixedSize16 = pEntry->nFixedSize16;

    if (nFlagBits & ADDRESS) {
        nBytesFixed = m_bAddressOverride ? nFixedSize16 : nFixedSize;
    }
#ifdef DETOURS_X64
#error Feature not supported in this release.



#endif
    else {
        nBytesFixed = m_bOperandOverride ? nFixedSize16 : nFixedSize;
    }

    UINT nBytes = nBytesFixed;
    UINT nRelOffset = pEntry->nRelOffset;
    UINT cbTarget = nBytes - nRelOffset;
    if (nModOffset > 0) {
        ASSERT(nRelOffset == 0);
        BYTE const bModRm = pbSrc[nModOffset];
        BYTE const bFlags = s_rbModRm[bModRm];

        nBytes += bFlags & NOTSIB;

        if (bFlags & SIB) {
            BYTE const bSib = pbSrc[nModOffset + 1];

            if ((bSib & 0x07) == 0x05) {
                if ((bModRm & 0xc0) == 0x00) {
                    nBytes += 4;
                }
                else if ((bModRm & 0xc0) == 0x40) {
                    nBytes += 1;
                }
                else if ((bModRm & 0xc0) == 0x80) {
                    nBytes += 4;
                }
            }
            cbTarget = nBytes - nRelOffset;
        }
#ifdef DETOURS_X64
#error Feature not supported in this release.











#endif
    }
    CopyMemory(pbDst, pbSrc, nBytes);

    if (nRelOffset) {
        *m_ppbTarget = AdjustTarget(pbDst, pbSrc, nBytes, nRelOffset, cbTarget);
#ifdef DETOURS_X64
#error Feature not supported in this release.



#endif
    }
    if (nFlagBits & NOENLARGE) {
        *m_plExtra = -*m_plExtra;
    }
    if (nFlagBits & DYNAMIC) {
        *m_ppbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_DYNAMIC;
    }
    return pbSrc + nBytes;
}

PBYTE CDetourDis::CopyBytesPrefix(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    pbDst[0] = pbSrc[0];
    pEntry = &s_rceCopyTable[pbSrc[1]];
    return (this->*pEntry->pfCopy)(pEntry, pbDst + 1, pbSrc + 1);
}

PBYTE CDetourDis::CopyBytesSegment(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
{
    m_nSegmentOverride = pbSrc[0];
    return CopyBytesPrefix(0, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyBytesRax(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
{ // AMD64 only
    if (pbSrc[0] & 0x8) {
        m_bRaxOverride = TRUE;
    }
    return CopyBytesPrefix(0, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyBytesJump(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    (void)pEntry;

    PVOID pvSrcAddr = &pbSrc[1];
    PVOID pvDstAddr = NULL;
    LONG_PTR nOldOffset = (LONG_PTR)*(signed char*&)pvSrcAddr;
    LONG_PTR nNewOffset = 0;

    *m_ppbTarget = pbSrc + 2 + nOldOffset;

    if (pbSrc[0] == 0xeb) {
        pbDst[0] = 0xe9;
        pvDstAddr = &pbDst[1];
        nNewOffset = nOldOffset - ((pbDst - pbSrc) + 3);
        *(UNALIGNED LONG*&)pvDstAddr = (LONG)nNewOffset;

        *m_plExtra = 3;
        return pbSrc + 2;
    }

    ASSERT(pbSrc[0] >= 0x70 && pbSrc[0] <= 0x7f);

    pbDst[0] = 0x0f;
    pbDst[1] = 0x80 | (pbSrc[0] & 0xf);
    pvDstAddr = &pbDst[2];
    nNewOffset = nOldOffset - ((pbDst - pbSrc) + 4);
    *(UNALIGNED LONG*&)pvDstAddr = (LONG)nNewOffset;

    *m_plExtra = 4;
    return pbSrc + 2;
}

PBYTE CDetourDis::AdjustTarget(PBYTE pbDst, PBYTE pbSrc, UINT cbOp,
                               UINT cbTargetOffset, UINT cbTargetSize)
{
    PBYTE pbTarget = NULL;
#if 1 // fault injection to test test code
#if defined(DETOURS_X64)
    typedef LONGLONG T;
#else
    typedef LONG T;
#endif
    T nOldOffset;
    T nNewOffset;
    PVOID pvTargetAddr = &pbDst[cbTargetOffset];

    switch (cbTargetSize) {
      case 1:
        nOldOffset = *(signed char*&)pvTargetAddr;
        break;
      case 2:
        nOldOffset = *(UNALIGNED SHORT*&)pvTargetAddr;
        break;
      case 4:
        nOldOffset = *(UNALIGNED LONG*&)pvTargetAddr;
        break;
#if defined(DETOURS_X64)
      case 8:
        nOldOffset = *(UNALIGNED LONGLONG*&)pvTargetAddr;
        break;
#endif
      default:
        ASSERT(!"cbTargetSize is invalid.");
        nOldOffset = 0;
        break;
    }

    pbTarget = pbSrc + cbOp + nOldOffset;
    nNewOffset = nOldOffset - (T)(pbDst - pbSrc);

    switch (cbTargetSize) {
      case 1:
        *(CHAR*&)pvTargetAddr = (CHAR)nNewOffset;
        if (nNewOffset < SCHAR_MIN || nNewOffset > SCHAR_MAX) {
            *m_plExtra = sizeof(ULONG) - 1;
        }
        break;
      case 2:
        *(UNALIGNED SHORT*&)pvTargetAddr = (SHORT)nNewOffset;
        if (nNewOffset < SHRT_MIN || nNewOffset > SHRT_MAX) {
            *m_plExtra = sizeof(ULONG) - 2;
        }
        break;
      case 4:
        *(UNALIGNED LONG*&)pvTargetAddr = (LONG)nNewOffset;
        if (nNewOffset < LONG_MIN || nNewOffset > LONG_MAX) {
            *m_plExtra = sizeof(ULONG) - 4;
        }
        break;
#if defined(DETOURS_X64)
      case 8:
        *(UNALIGNED LONGLONG*&)pvTargetAddr = nNewOffset;
        break;
#endif
    }
#ifdef DETOURS_X64
#error Feature not supported in this release.







#endif
    {
        ASSERT(pbDst + cbOp + nNewOffset == pbTarget);
    }
#endif
    return pbTarget;
}

PBYTE CDetourDis::Invalid(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    (void)pbDst;
    (void)pEntry;
    ASSERT(!"Invalid Instruction");
    return pbSrc + 1;
}

////////////////////////////////////////////////////// Individual Bytes Codes.
//
PBYTE CDetourDis::Copy0F(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    pbDst[0] = pbSrc[0];
    pEntry = &s_rceCopyTable0F[pbSrc[1]];
    return (this->*pEntry->pfCopy)(pEntry, pbDst + 1, pbSrc + 1);
}

PBYTE CDetourDis::Copy0F78(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
{
    // vmread, 66/extrq, F2/insertq

    static const COPYENTRY vmread = { 0x78, ENTRY_CopyBytes2Mod };
    static const COPYENTRY extrq_insertq = { 0x78, ENTRY_CopyBytes4 };

    ASSERT(!(m_bF2 && m_bOperandOverride));

    // For insertq and presumably despite documentation extrq, mode must be 11, not checked.
    // insertq/extrq/78 are followed by two immediate bytes, and given mode == 11, mod/rm byte is always one byte,
    // and the 0x78 makes 4 bytes (not counting the 66/F2/F which are accounted for elsewhere)

    REFCOPYENTRY const pEntry = ((m_bF2 || m_bOperandOverride) ? &extrq_insertq : &vmread);

    return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::Copy0F00(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
{
    // jmpe is 32bit x86 only
    // Notice that the sizes are the same either way, but jmpe is marked as "dynamic".

    static const COPYENTRY other = { 0xB8, ENTRY_CopyBytes2Mod }; // sldt/0 str/1 lldt/2 ltr/3 err/4 verw/5 jmpe/6 invalid/7
    static const COPYENTRY jmpe = { 0xB8, ENTRY_CopyBytes2ModDynamic }; // jmpe/6 x86-on-IA64 syscalls

    REFCOPYENTRY const pEntry = (((6 << 3) == ((7 << 3) & pbSrc[1])) ?  &jmpe : &other);
    return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::Copy0FB8(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
{
    // jmpe is 32bit x86 only

    static const COPYENTRY popcnt = { 0xB8, ENTRY_CopyBytes2Mod };
    static const COPYENTRY jmpe = { 0xB8, ENTRY_CopyBytes3Or5Dynamic }; // jmpe x86-on-IA64 syscalls
    REFCOPYENTRY const pEntry = m_bF3 ? &popcnt : &jmpe;
    return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::Copy66(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{   // Operand-size override prefix
    m_bOperandOverride = TRUE;
    return CopyBytesPrefix(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::Copy67(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{   // Address size override prefix
    m_bAddressOverride = TRUE;
    return CopyBytesPrefix(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyF2(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    m_bF2 = TRUE;
    return CopyBytesPrefix(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyF3(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{ // x86 only
    m_bF3 = TRUE;
    return CopyBytesPrefix(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyF6(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    (void)pEntry;

    // TEST BYTE /0
    if (0x00 == (0x38 & pbSrc[1])) {    // reg(bits 543) of ModR/M == 0
        static const COPYENTRY ce = { 0xf6, ENTRY_CopyBytes2Mod1 };
        return (this->*ce.pfCopy)(&ce, pbDst, pbSrc);
    }
    // DIV /6
    // IDIV /7
    // IMUL /5
    // MUL /4
    // NEG /3
    // NOT /2

    static const COPYENTRY ce = { 0xf6, ENTRY_CopyBytes2Mod };
    return (this->*ce.pfCopy)(&ce, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyF7(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{
    (void)pEntry;

    // TEST WORD /0
    if (0x00 == (0x38 & pbSrc[1])) {    // reg(bits 543) of ModR/M == 0
        static const COPYENTRY ce = { 0xf7, ENTRY_CopyBytes2ModOperand };
        return (this->*ce.pfCopy)(&ce, pbDst, pbSrc);
    }

    // DIV /6
    // IDIV /7
    // IMUL /5
    // MUL /4
    // NEG /3
    // NOT /2
    static const COPYENTRY ce = { 0xf7, ENTRY_CopyBytes2Mod };
    return (this->*ce.pfCopy)(&ce, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyFF(REFCOPYENTRY pEntry, PBYTE pbDst, PBYTE pbSrc)
{   // INC /0
    // DEC /1
    // CALL /2
    // CALL /3
    // JMP /4
    // JMP /5
    // PUSH /6
    // invalid/7
    (void)pEntry;

    static const COPYENTRY ce = { 0xff, ENTRY_CopyBytes2Mod };
    PBYTE pbOut = (this->*ce.pfCopy)(&ce, pbDst, pbSrc);

    BYTE const b1 = pbSrc[1];

    if (0x15 == b1 || 0x25 == b1) {         // CALL [], JMP []
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
        if (m_nSegmentOverride == 0 || m_nSegmentOverride == 0x2E)
#endif
        {
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
            PBYTE *ppbTarget = (PBYTE *)(SIZE_T)*(UNALIGNED ULONG*)&pbSrc[2];
#endif
            if (s_fLimitReferencesToModule &&
                (ppbTarget < (PVOID)s_pbModuleBeg || ppbTarget >= (PVOID)s_pbModuleEnd)) {

                *m_ppbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_DYNAMIC;
            }
            else {
                // This can access violate on random bytes. Use DetourSetCodeModule.
                *m_ppbTarget = *ppbTarget;
            }
        }
        else {
            *m_ppbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_DYNAMIC;
        }
    }
    else if (0x10 == (0x30 & b1) || // CALL /2 or /3  --> reg(bits 543) of ModR/M == 010 or 011
             0x20 == (0x30 & b1)) { // JMP /4 or /5 --> reg(bits 543) of ModR/M == 100 or 101
        *m_ppbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_DYNAMIC;
    }
    return pbOut;
}

PBYTE CDetourDis::CopyVexCommon(BYTE m, PBYTE pbDst, PBYTE pbSrc)
// m is first instead of last in the hopes of pbDst/pbSrc being
// passed along efficiently in the registers they were already in.
{
    static const COPYENTRY ceF38 = { 0x38, ENTRY_CopyBytes2Mod };
    static const COPYENTRY ceF3A = { 0x3A, ENTRY_CopyBytes2Mod1 };
    static const COPYENTRY Invalid = { 0xC4, ENTRY_Invalid };

    m_bVex = TRUE;
    REFCOPYENTRY pEntry;
    switch (m) {
    default: pEntry = &Invalid; break;
    case 1:  pEntry = &s_rceCopyTable0F[pbSrc[0]]; break;
    case 2:  pEntry = &ceF38; break;
    case 3:  pEntry = &ceF3A; break;
    }

    switch (pbSrc[-1] & 3) { // p in last byte
    case 0: break;
    case 1: m_bOperandOverride = TRUE; break;
    case 2: m_bF3 = TRUE; break;
    case 3: m_bF2 = TRUE; break;
    }

    return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
}

PBYTE CDetourDis::CopyVex3(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
// 3 byte VEX prefix 0xC4
{
#ifdef DETOURS_X86
    const static COPYENTRY ceLES = { 0xC4, ENTRY_CopyBytes2Mod };
    if ((pbSrc[1] & 0xC0) != 0xC0) {
        REFCOPYENTRY pEntry = &ceLES;
        return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
    }
#endif
    pbDst[0] = pbSrc[0];
    pbDst[1] = pbSrc[1];
    pbDst[2] = pbSrc[2];
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    //
    // TODO
    //
    // Usually the VEX.W bit changes the size of a general purpose register and is ignored for 32bit.
    // Sometimes it is an opcode extension.
    // Look in the Intel manual, in the instruction-by-instruction reference, for ".W1",
    // without nearby wording saying it is ignored for 32bit.
    // For example: "VFMADD132PD/VFMADD213PD/VFMADD231PD Fused Multiply-Add of Packed Double-Precision Floating-Point Values".
    //
    // Then, go through each such case and determine if W0 vs. W1 affect the size of the instruction. Probably not.
    // Look for the same encoding but with "W1" changed to "W0".
    // Here is one such pairing:
    // VFMADD132PD/VFMADD213PD/VFMADD231PD Fused Multiply-Add of Packed Double-Precision Floating-Point Values
    //
    // VEX.DDS.128.66.0F38.W1 98 /r A V/V FMA Multiply packed double-precision floating-point values
    // from xmm0 and xmm2/mem, add to xmm1 and
    // put result in xmm0.
    // VFMADD132PD xmm0, xmm1, xmm2/m128
    //
    // VFMADD132PS/VFMADD213PS/VFMADD231PS Fused Multiply-Add of Packed Single-Precision Floating-Point Values
    // VEX.DDS.128.66.0F38.W0 98 /r A V/V FMA Multiply packed single-precision floating-point values
    // from xmm0 and xmm2/mem, add to xmm1 and put
    // result in xmm0.
    // VFMADD132PS xmm0, xmm1, xmm2/m128
    //
#endif
    return CopyVexCommon(pbSrc[1] & 0x1F, pbDst + 3, pbSrc + 3);
}

PBYTE CDetourDis::CopyVex2(REFCOPYENTRY, PBYTE pbDst, PBYTE pbSrc)
// 2 byte VEX prefix 0xC5
{
#ifdef DETOURS_X86
    const static COPYENTRY ceLDS = { 0xC5, ENTRY_CopyBytes2Mod };
    if ((pbSrc[1] & 0xC0) != 0xC0) {
        REFCOPYENTRY pEntry = &ceLDS;
        return (this->*pEntry->pfCopy)(pEntry, pbDst, pbSrc);
    }
#endif
    pbDst[0] = pbSrc[0];
    pbDst[1] = pbSrc[1];
    return CopyVexCommon(1, pbDst + 2, pbSrc + 2);
}

//////////////////////////////////////////////////////////////////////////////
//
PBYTE CDetourDis::s_pbModuleBeg = NULL;
PBYTE CDetourDis::s_pbModuleEnd = (PBYTE)~(ULONG_PTR)0;
BOOL CDetourDis::s_fLimitReferencesToModule = FALSE;

BOOL CDetourDis::SetCodeModule(PBYTE pbBeg, PBYTE pbEnd, BOOL fLimitReferencesToModule)
{
    if (pbEnd < pbBeg) {
        return FALSE;
    }

    s_pbModuleBeg = pbBeg;
    s_pbModuleEnd = pbEnd;
    s_fLimitReferencesToModule = fLimitReferencesToModule;

    return TRUE;
}

///////////////////////////////////////////////////////// Disassembler Tables.
//
const BYTE CDetourDis::s_rbModRm[256] = {
    0,0,0,0, SIB|1,RIP|4,0,0, 0,0,0,0, SIB|1,RIP|4,0,0, // 0x
    0,0,0,0, SIB|1,RIP|4,0,0, 0,0,0,0, SIB|1,RIP|4,0,0, // 1x
    0,0,0,0, SIB|1,RIP|4,0,0, 0,0,0,0, SIB|1,RIP|4,0,0, // 2x
    0,0,0,0, SIB|1,RIP|4,0,0, 0,0,0,0, SIB|1,RIP|4,0,0, // 3x
    1,1,1,1, 2,1,1,1, 1,1,1,1, 2,1,1,1,                 // 4x
    1,1,1,1, 2,1,1,1, 1,1,1,1, 2,1,1,1,                 // 5x
    1,1,1,1, 2,1,1,1, 1,1,1,1, 2,1,1,1,                 // 6x
    1,1,1,1, 2,1,1,1, 1,1,1,1, 2,1,1,1,                 // 7x
    4,4,4,4, 5,4,4,4, 4,4,4,4, 5,4,4,4,                 // 8x
    4,4,4,4, 5,4,4,4, 4,4,4,4, 5,4,4,4,                 // 9x
    4,4,4,4, 5,4,4,4, 4,4,4,4, 5,4,4,4,                 // Ax
    4,4,4,4, 5,4,4,4, 4,4,4,4, 5,4,4,4,                 // Bx
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,                 // Cx
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,                 // Dx
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,                 // Ex
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0                  // Fx
};

const CDetourDis::COPYENTRY CDetourDis::s_rceCopyTable[257] =
{
    { 0x00, ENTRY_CopyBytes2Mod },                      // ADD /r
    { 0x01, ENTRY_CopyBytes2Mod },                      // ADD /r
    { 0x02, ENTRY_CopyBytes2Mod },                      // ADD /r
    { 0x03, ENTRY_CopyBytes2Mod },                      // ADD /r
    { 0x04, ENTRY_CopyBytes2 },                         // ADD ib
    { 0x05, ENTRY_CopyBytes3Or5 },                      // ADD iw
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
    { 0x06, ENTRY_CopyBytes1 },                         // PUSH
    { 0x07, ENTRY_CopyBytes1 },                         // POP
#endif
    { 0x08, ENTRY_CopyBytes2Mod },                      // OR /r
    { 0x09, ENTRY_CopyBytes2Mod },                      // OR /r
    { 0x0A, ENTRY_CopyBytes2Mod },                      // OR /r
    { 0x0B, ENTRY_CopyBytes2Mod },                      // OR /r
    { 0x0C, ENTRY_CopyBytes2 },                         // OR ib
    { 0x0D, ENTRY_CopyBytes3Or5 },                      // OR iw
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x0E, ENTRY_CopyBytes1 },                         // PUSH
#endif
    { 0x0F, ENTRY_Copy0F },                             // Extension Ops
    { 0x10, ENTRY_CopyBytes2Mod },                      // ADC /r
    { 0x11, ENTRY_CopyBytes2Mod },                      // ADC /r
    { 0x12, ENTRY_CopyBytes2Mod },                      // ADC /r
    { 0x13, ENTRY_CopyBytes2Mod },                      // ADC /r
    { 0x14, ENTRY_CopyBytes2 },                         // ADC ib
    { 0x15, ENTRY_CopyBytes3Or5 },                      // ADC id
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
    { 0x16, ENTRY_CopyBytes1 },                         // PUSH
    { 0x17, ENTRY_CopyBytes1 },                         // POP
#endif
    { 0x18, ENTRY_CopyBytes2Mod },                      // SBB /r
    { 0x19, ENTRY_CopyBytes2Mod },                      // SBB /r
    { 0x1A, ENTRY_CopyBytes2Mod },                      // SBB /r
    { 0x1B, ENTRY_CopyBytes2Mod },                      // SBB /r
    { 0x1C, ENTRY_CopyBytes2 },                         // SBB ib
    { 0x1D, ENTRY_CopyBytes3Or5 },                      // SBB id
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
    { 0x1E, ENTRY_CopyBytes1 },                         // PUSH
    { 0x1F, ENTRY_CopyBytes1 },                         // POP
#endif
    { 0x20, ENTRY_CopyBytes2Mod },                      // AND /r
    { 0x21, ENTRY_CopyBytes2Mod },                      // AND /r
    { 0x22, ENTRY_CopyBytes2Mod },                      // AND /r
    { 0x23, ENTRY_CopyBytes2Mod },                      // AND /r
    { 0x24, ENTRY_CopyBytes2 },                         // AND ib
    { 0x25, ENTRY_CopyBytes3Or5 },                      // AND id
    { 0x26, ENTRY_CopyBytesSegment },                   // ES prefix
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x27, ENTRY_CopyBytes1 },                         // DAA
#endif
    { 0x28, ENTRY_CopyBytes2Mod },                      // SUB /r
    { 0x29, ENTRY_CopyBytes2Mod },                      // SUB /r
    { 0x2A, ENTRY_CopyBytes2Mod },                      // SUB /r
    { 0x2B, ENTRY_CopyBytes2Mod },                      // SUB /r
    { 0x2C, ENTRY_CopyBytes2 },                         // SUB ib
    { 0x2D, ENTRY_CopyBytes3Or5 },                      // SUB id
    { 0x2E, ENTRY_CopyBytesSegment },                   // CS prefix
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x2F, ENTRY_CopyBytes1 },                         // DAS
#endif
    { 0x30, ENTRY_CopyBytes2Mod },                      // XOR /r
    { 0x31, ENTRY_CopyBytes2Mod },                      // XOR /r
    { 0x32, ENTRY_CopyBytes2Mod },                      // XOR /r
    { 0x33, ENTRY_CopyBytes2Mod },                      // XOR /r
    { 0x34, ENTRY_CopyBytes2 },                         // XOR ib
    { 0x35, ENTRY_CopyBytes3Or5 },                      // XOR id
    { 0x36, ENTRY_CopyBytesSegment },                   // SS prefix
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x37, ENTRY_CopyBytes1 },                         // AAA
#endif
    { 0x38, ENTRY_CopyBytes2Mod },                      // CMP /r
    { 0x39, ENTRY_CopyBytes2Mod },                      // CMP /r
    { 0x3A, ENTRY_CopyBytes2Mod },                      // CMP /r
    { 0x3B, ENTRY_CopyBytes2Mod },                      // CMP /r
    { 0x3C, ENTRY_CopyBytes2 },                         // CMP ib
    { 0x3D, ENTRY_CopyBytes3Or5 },                      // CMP id
    { 0x3E, ENTRY_CopyBytesSegment },                   // DS prefix
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x3F, ENTRY_CopyBytes1 },                         // AAS
#endif
#ifdef DETOURS_X64 // For Rax Prefix
#error Feature not supported in this release.















#else
    { 0x40, ENTRY_CopyBytes1 },                         // INC
    { 0x41, ENTRY_CopyBytes1 },                         // INC
    { 0x42, ENTRY_CopyBytes1 },                         // INC
    { 0x43, ENTRY_CopyBytes1 },                         // INC
    { 0x44, ENTRY_CopyBytes1 },                         // INC
    { 0x45, ENTRY_CopyBytes1 },                         // INC
    { 0x46, ENTRY_CopyBytes1 },                         // INC
    { 0x47, ENTRY_CopyBytes1 },                         // INC
    { 0x48, ENTRY_CopyBytes1 },                         // DEC
    { 0x49, ENTRY_CopyBytes1 },                         // DEC
    { 0x4A, ENTRY_CopyBytes1 },                         // DEC
    { 0x4B, ENTRY_CopyBytes1 },                         // DEC
    { 0x4C, ENTRY_CopyBytes1 },                         // DEC
    { 0x4D, ENTRY_CopyBytes1 },                         // DEC
    { 0x4E, ENTRY_CopyBytes1 },                         // DEC
    { 0x4F, ENTRY_CopyBytes1 },                         // DEC
#endif
    { 0x50, ENTRY_CopyBytes1 },                         // PUSH
    { 0x51, ENTRY_CopyBytes1 },                         // PUSH
    { 0x52, ENTRY_CopyBytes1 },                         // PUSH
    { 0x53, ENTRY_CopyBytes1 },                         // PUSH
    { 0x54, ENTRY_CopyBytes1 },                         // PUSH
    { 0x55, ENTRY_CopyBytes1 },                         // PUSH
    { 0x56, ENTRY_CopyBytes1 },                         // PUSH
    { 0x57, ENTRY_CopyBytes1 },                         // PUSH
    { 0x58, ENTRY_CopyBytes1 },                         // POP
    { 0x59, ENTRY_CopyBytes1 },                         // POP
    { 0x5A, ENTRY_CopyBytes1 },                         // POP
    { 0x5B, ENTRY_CopyBytes1 },                         // POP
    { 0x5C, ENTRY_CopyBytes1 },                         // POP
    { 0x5D, ENTRY_CopyBytes1 },                         // POP
    { 0x5E, ENTRY_CopyBytes1 },                         // POP
    { 0x5F, ENTRY_CopyBytes1 },                         // POP
#ifdef DETOURS_X64
#error Feature not supported in this release.


#else
    { 0x60, ENTRY_CopyBytes1 },                         // PUSHAD
    { 0x61, ENTRY_CopyBytes1 },                         // POPAD
    { 0x62, ENTRY_CopyBytes2Mod },                      // BOUND /r
#endif
    { 0x63, ENTRY_CopyBytes2Mod },                      // 32bit ARPL /r, 64bit MOVSXD
    { 0x64, ENTRY_CopyBytesSegment },                   // FS prefix
    { 0x65, ENTRY_CopyBytesSegment },                   // GS prefix
    { 0x66, ENTRY_Copy66 },                             // Operand Prefix
    { 0x67, ENTRY_Copy67 },                             // Address Prefix
    { 0x68, ENTRY_CopyBytes3Or5 },                      // PUSH
    { 0x69, ENTRY_CopyBytes2ModOperand },               // IMUL /r iz
    { 0x6A, ENTRY_CopyBytes2 },                         // PUSH
    { 0x6B, ENTRY_CopyBytes2Mod1 },                     // IMUL /r ib
    { 0x6C, ENTRY_CopyBytes1 },                         // INS
    { 0x6D, ENTRY_CopyBytes1 },                         // INS
    { 0x6E, ENTRY_CopyBytes1 },                         // OUTS/OUTSB
    { 0x6F, ENTRY_CopyBytes1 },                         // OUTS/OUTSW
    { 0x70, ENTRY_CopyBytes2Jump },                     // JO           // 0f80
    { 0x71, ENTRY_CopyBytes2Jump },                     // JNO          // 0f81
    { 0x72, ENTRY_CopyBytes2Jump },                     // JB/JC/JNAE   // 0f82
    { 0x73, ENTRY_CopyBytes2Jump },                     // JAE/JNB/JNC  // 0f83
    { 0x74, ENTRY_CopyBytes2Jump },                     // JE/JZ        // 0f84
    { 0x75, ENTRY_CopyBytes2Jump },                     // JNE/JNZ      // 0f85
    { 0x76, ENTRY_CopyBytes2Jump },                     // JBE/JNA      // 0f86
    { 0x77, ENTRY_CopyBytes2Jump },                     // JA/JNBE      // 0f87
    { 0x78, ENTRY_CopyBytes2Jump },                     // JS           // 0f88
    { 0x79, ENTRY_CopyBytes2Jump },                     // JNS          // 0f89
    { 0x7A, ENTRY_CopyBytes2Jump },                     // JP/JPE       // 0f8a
    { 0x7B, ENTRY_CopyBytes2Jump },                     // JNP/JPO      // 0f8b
    { 0x7C, ENTRY_CopyBytes2Jump },                     // JL/JNGE      // 0f8c
    { 0x7D, ENTRY_CopyBytes2Jump },                     // JGE/JNL      // 0f8d
    { 0x7E, ENTRY_CopyBytes2Jump },                     // JLE/JNG      // 0f8e
    { 0x7F, ENTRY_CopyBytes2Jump },                     // JG/JNLE      // 0f8f
    { 0x80, ENTRY_CopyBytes2Mod1 },                     // ADD/0 OR/1 ADC/2 SBB/3 AND/4 SUB/5 XOR/6 CMP/7 byte reg, immediate byte
    { 0x81, ENTRY_CopyBytes2ModOperand },               // ADD/0 OR/1 ADC/2 SBB/3 AND/4 SUB/5 XOR/6 CMP/7 byte reg, immediate word or dword
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x82, ENTRY_CopyBytes2Mod1 },                     // MOV al,x
#endif
    { 0x83, ENTRY_CopyBytes2Mod1 },                     // ADD/0 OR/1 ADC/2 SBB/3 AND/4 SUB/5 XOR/6 CMP/7 reg, immediate byte
    { 0x84, ENTRY_CopyBytes2Mod },                      // TEST /r
    { 0x85, ENTRY_CopyBytes2Mod },                      // TEST /r
    { 0x86, ENTRY_CopyBytes2Mod },                      // XCHG /r @todo
    { 0x87, ENTRY_CopyBytes2Mod },                      // XCHG /r @todo
    { 0x88, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x89, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x8A, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x8B, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x8C, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x8D, ENTRY_CopyBytes2Mod },                      // LEA /r
    { 0x8E, ENTRY_CopyBytes2Mod },                      // MOV /r
    { 0x8F, ENTRY_CopyBytes2Mod },                      // POP /0
    { 0x90, ENTRY_CopyBytes1 },                         // NOP
    { 0x91, ENTRY_CopyBytes1 },                         // XCHG
    { 0x92, ENTRY_CopyBytes1 },                         // XCHG
    { 0x93, ENTRY_CopyBytes1 },                         // XCHG
    { 0x94, ENTRY_CopyBytes1 },                         // XCHG
    { 0x95, ENTRY_CopyBytes1 },                         // XCHG
    { 0x96, ENTRY_CopyBytes1 },                         // XCHG
    { 0x97, ENTRY_CopyBytes1 },                         // XCHG
    { 0x98, ENTRY_CopyBytes1 },                         // CWDE
    { 0x99, ENTRY_CopyBytes1 },                         // CDQ
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x9A, ENTRY_CopyBytes5Or7Dynamic },               // CALL cp
#endif
    { 0x9B, ENTRY_CopyBytes1 },                         // WAIT/FWAIT
    { 0x9C, ENTRY_CopyBytes1 },                         // PUSHFD
    { 0x9D, ENTRY_CopyBytes1 },                         // POPFD
    { 0x9E, ENTRY_CopyBytes1 },                         // SAHF
    { 0x9F, ENTRY_CopyBytes1 },                         // LAHF
    { 0xA0, ENTRY_CopyBytes1Address },                  // MOV
    { 0xA1, ENTRY_CopyBytes1Address },                  // MOV
    { 0xA2, ENTRY_CopyBytes1Address },                  // MOV
    { 0xA3, ENTRY_CopyBytes1Address },                  // MOV
    { 0xA4, ENTRY_CopyBytes1 },                         // MOVS
    { 0xA5, ENTRY_CopyBytes1 },                         // MOVS/MOVSD
    { 0xA6, ENTRY_CopyBytes1 },                         // CMPS/CMPSB
    { 0xA7, ENTRY_CopyBytes1 },                         // CMPS/CMPSW
    { 0xA8, ENTRY_CopyBytes2 },                         // TEST
    { 0xA9, ENTRY_CopyBytes3Or5 },                      // TEST
    { 0xAA, ENTRY_CopyBytes1 },                         // STOS/STOSB
    { 0xAB, ENTRY_CopyBytes1 },                         // STOS/STOSW
    { 0xAC, ENTRY_CopyBytes1 },                         // LODS/LODSB
    { 0xAD, ENTRY_CopyBytes1 },                         // LODS/LODSW
    { 0xAE, ENTRY_CopyBytes1 },                         // SCAS/SCASB
    { 0xAF, ENTRY_CopyBytes1 },                         // SCAS/SCASD
    { 0xB0, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB1, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB2, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB3, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB4, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB5, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB6, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB7, ENTRY_CopyBytes2 },                         // MOV B0+rb
    { 0xB8, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xB9, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBA, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBB, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBC, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBD, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBE, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xBF, ENTRY_CopyBytes3Or5Rax },                   // MOV B8+rb
    { 0xC0, ENTRY_CopyBytes2Mod1 },                     // RCL/2 ib, etc.
    { 0xC1, ENTRY_CopyBytes2Mod1 },                     // RCL/2 ib, etc.
    { 0xC2, ENTRY_CopyBytes3 },                         // RET
    { 0xC3, ENTRY_CopyBytes1 },                         // RET
    { 0xC4, ENTRY_CopyVex3 },                           // LES, VEX 3-byte opcodes.
    { 0xC5, ENTRY_CopyVex2 },                           // LDS, VEX 2-byte opcodes.
    { 0xC6, ENTRY_CopyBytes2Mod1 },                     // MOV
    { 0xC7, ENTRY_CopyBytes2ModOperand },               // MOV/0 XBEGIN/7
    { 0xC8, ENTRY_CopyBytes4 },                         // ENTER
    { 0xC9, ENTRY_CopyBytes1 },                         // LEAVE
    { 0xCA, ENTRY_CopyBytes3Dynamic },                  // RET
    { 0xCB, ENTRY_CopyBytes1Dynamic },                  // RET
    { 0xCC, ENTRY_CopyBytes1Dynamic },                  // INT 3
    { 0xCD, ENTRY_CopyBytes2Dynamic },                  // INT ib
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0xCE, ENTRY_CopyBytes1Dynamic },                  // INTO
#endif
    { 0xCF, ENTRY_CopyBytes1Dynamic },                  // IRET
    { 0xD0, ENTRY_CopyBytes2Mod },                      // RCL/2, etc.
    { 0xD1, ENTRY_CopyBytes2Mod },                      // RCL/2, etc.
    { 0xD2, ENTRY_CopyBytes2Mod },                      // RCL/2, etc.
    { 0xD3, ENTRY_CopyBytes2Mod },                      // RCL/2, etc.
#ifdef DETOURS_X64
#error Feature not supported in this release.

#else
    { 0xD4, ENTRY_CopyBytes2 },                         // AAM
    { 0xD5, ENTRY_CopyBytes2 },                         // AAD
#endif
    { 0xD6, ENTRY_Invalid },                            // Invalid
    { 0xD7, ENTRY_CopyBytes1 },                         // XLAT/XLATB
    { 0xD8, ENTRY_CopyBytes2Mod },                      // FADD, etc.
    { 0xD9, ENTRY_CopyBytes2Mod },                      // F2XM1, etc.
    { 0xDA, ENTRY_CopyBytes2Mod },                      // FLADD, etc.
    { 0xDB, ENTRY_CopyBytes2Mod },                      // FCLEX, etc.
    { 0xDC, ENTRY_CopyBytes2Mod },                      // FADD/0, etc.
    { 0xDD, ENTRY_CopyBytes2Mod },                      // FFREE, etc.
    { 0xDE, ENTRY_CopyBytes2Mod },                      // FADDP, etc.
    { 0xDF, ENTRY_CopyBytes2Mod },                      // FBLD/4, etc.
    { 0xE0, ENTRY_CopyBytes2CantJump },                 // LOOPNE cb
    { 0xE1, ENTRY_CopyBytes2CantJump },                 // LOOPE cb
    { 0xE2, ENTRY_CopyBytes2CantJump },                 // LOOP cb
    { 0xE3, ENTRY_CopyBytes2CantJump },                 // JCXZ/JECXZ
    { 0xE4, ENTRY_CopyBytes2 },                         // IN ib
    { 0xE5, ENTRY_CopyBytes2 },                         // IN id
    { 0xE6, ENTRY_CopyBytes2 },                         // OUT ib
    { 0xE7, ENTRY_CopyBytes2 },                         // OUT ib
    { 0xE8, ENTRY_CopyBytes3Or5Target },                // CALL cd
    { 0xE9, ENTRY_CopyBytes3Or5Target },                // JMP cd
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0xEA, ENTRY_CopyBytes5Or7Dynamic },               // JMP cp
#endif
    { 0xEB, ENTRY_CopyBytes2Jump },                     // JMP cb
    { 0xEC, ENTRY_CopyBytes1 },                         // IN ib
    { 0xED, ENTRY_CopyBytes1 },                         // IN id
    { 0xEE, ENTRY_CopyBytes1 },                         // OUT
    { 0xEF, ENTRY_CopyBytes1 },                         // OUT
    { 0xF0, ENTRY_CopyBytesPrefix },                    // LOCK prefix
    { 0xF1, ENTRY_CopyBytes1Dynamic },                  // INT1 / ICEBP somewhat documented by AMD, not by Intel
    { 0xF2, ENTRY_CopyF2 },                             // REPNE prefix
//#ifdef DETOURS_X86
    { 0xF3, ENTRY_CopyF3 },                             // REPE prefix
//#else
// This does presently suffice for AMD64 but it requires tracing
// through a bunch of code to verify and seems not worth maintaining.
//  { 0xF3, ENTRY_CopyBytesPrefix },                    // REPE prefix
//#endif
    { 0xF4, ENTRY_CopyBytes1 },                         // HLT
    { 0xF5, ENTRY_CopyBytes1 },                         // CMC
    { 0xF6, ENTRY_CopyF6 },                             // TEST/0, DIV/6
    { 0xF7, ENTRY_CopyF7 },                             // TEST/0, DIV/6
    { 0xF8, ENTRY_CopyBytes1 },                         // CLC
    { 0xF9, ENTRY_CopyBytes1 },                         // STC
    { 0xFA, ENTRY_CopyBytes1 },                         // CLI
    { 0xFB, ENTRY_CopyBytes1 },                         // STI
    { 0xFC, ENTRY_CopyBytes1 },                         // CLD
    { 0xFD, ENTRY_CopyBytes1 },                         // STD
    { 0xFE, ENTRY_CopyBytes2Mod },                      // DEC/1,INC/0
    { 0xFF, ENTRY_CopyFF },                             // CALL/2
    { 0, ENTRY_End },
};

const CDetourDis::COPYENTRY CDetourDis::s_rceCopyTable0F[257] =
{
#ifdef DETOURS_X86
    { 0x00, ENTRY_Copy0F00 },                           // sldt/0 str/1 lldt/2 ltr/3 err/4 verw/5 jmpe/6/dynamic invalid/7
#else
    { 0x00, ENTRY_CopyBytes2Mod },                      // sldt/0 str/1 lldt/2 ltr/3 err/4 verw/5 jmpe/6/dynamic invalid/7
#endif
    { 0x01, ENTRY_CopyBytes2Mod },                      // INVLPG/7, etc.
    { 0x02, ENTRY_CopyBytes2Mod },                      // LAR/r
    { 0x03, ENTRY_CopyBytes2Mod },                      // LSL/r
    { 0x04, ENTRY_Invalid },                            // _04
    { 0x05, ENTRY_CopyBytes1 },                         // SYSCALL
    { 0x06, ENTRY_CopyBytes1 },                         // CLTS
    { 0x07, ENTRY_CopyBytes1 },                         // SYSRET
    { 0x08, ENTRY_CopyBytes1 },                         // INVD
    { 0x09, ENTRY_CopyBytes1 },                         // WBINVD
    { 0x0A, ENTRY_Invalid },                            // _0A
    { 0x0B, ENTRY_CopyBytes1 },                         // UD2
    { 0x0C, ENTRY_Invalid },                            // _0C
    { 0x0D, ENTRY_CopyBytes2Mod },                      // PREFETCH
    { 0x0E, ENTRY_CopyBytes1 },                         // FEMMS (3DNow -- not in Intel documentation)
    { 0x0F, ENTRY_CopyBytes2Mod1 },                     // 3DNow Opcodes
    { 0x10, ENTRY_CopyBytes2Mod },                      // MOVSS MOVUPD MOVSD
    { 0x11, ENTRY_CopyBytes2Mod },                      // MOVSS MOVUPD MOVSD
    { 0x12, ENTRY_CopyBytes2Mod },                      // MOVLPD
    { 0x13, ENTRY_CopyBytes2Mod },                      // MOVLPD
    { 0x14, ENTRY_CopyBytes2Mod },                      // UNPCKLPD
    { 0x15, ENTRY_CopyBytes2Mod },                      // UNPCKHPD
    { 0x16, ENTRY_CopyBytes2Mod },                      // MOVHPD
    { 0x17, ENTRY_CopyBytes2Mod },                      // MOVHPD
    { 0x18, ENTRY_CopyBytes2Mod },                      // PREFETCHINTA...
    { 0x19, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1A, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1B, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1C, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1D, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1E, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop, not documented by Intel, documented by AMD
    { 0x1F, ENTRY_CopyBytes2Mod },                      // NOP/r multi byte nop
    { 0x20, ENTRY_CopyBytes2Mod },                      // MOV/r
    { 0x21, ENTRY_CopyBytes2Mod },                      // MOV/r
    { 0x22, ENTRY_CopyBytes2Mod },                      // MOV/r
    { 0x23, ENTRY_CopyBytes2Mod },                      // MOV/r
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x24, ENTRY_CopyBytes2Mod },                      // MOV/r,TR TR is test register on 80386 and 80486, removed in Pentium
#endif
    { 0x25, ENTRY_Invalid },                            // _25
#ifdef DETOURS_X64
#error Feature not supported in this release.
#else
    { 0x26, ENTRY_CopyBytes2Mod },                      // MOV TR/r TR is test register on 80386 and 80486, removed in Pentium
#endif
    { 0x27, ENTRY_Invalid },                            // _27
    { 0x28, ENTRY_CopyBytes2Mod },                      // MOVAPS MOVAPD
    { 0x29, ENTRY_CopyBytes2Mod },                      // MOVAPS MOVAPD
    { 0x2A, ENTRY_CopyBytes2Mod },                      // CVPI2PS &
    { 0x2B, ENTRY_CopyBytes2Mod },                      // MOVNTPS MOVNTPD
    { 0x2C, ENTRY_CopyBytes2Mod },                      // CVTTPS2PI &
    { 0x2D, ENTRY_CopyBytes2Mod },                      // CVTPS2PI &
    { 0x2E, ENTRY_CopyBytes2Mod },                      // UCOMISS UCOMISD
    { 0x2F, ENTRY_CopyBytes2Mod },                      // COMISS COMISD
    { 0x30, ENTRY_CopyBytes1 },                         // WRMSR
    { 0x31, ENTRY_CopyBytes1 },                         // RDTSC
    { 0x32, ENTRY_CopyBytes1 },                         // RDMSR
    { 0x33, ENTRY_CopyBytes1 },                         // RDPMC
    { 0x34, ENTRY_CopyBytes1 },                         // SYSENTER
    { 0x35, ENTRY_CopyBytes1 },                         // SYSEXIT
    { 0x36, ENTRY_Invalid },                            // _36
    { 0x37, ENTRY_CopyBytes1 },                         // GETSEC
    { 0x38, ENTRY_CopyBytes3Mod },                      // SSE3 Opcodes
    { 0x39, ENTRY_Invalid },                            // _39
    { 0x3A, ENTRY_CopyBytes3Mod1 },                      // SSE3 Opcodes
    { 0x3B, ENTRY_Invalid },                            // _3B
    { 0x3C, ENTRY_Invalid },                            // _3C
    { 0x3D, ENTRY_Invalid },                            // _3D
    { 0x3E, ENTRY_Invalid },                            // _3E
    { 0x3F, ENTRY_Invalid },                            // _3F
    { 0x40, ENTRY_CopyBytes2Mod },                      // CMOVO (0F 40)
    { 0x41, ENTRY_CopyBytes2Mod },                      // CMOVNO (0F 41)
    { 0x42, ENTRY_CopyBytes2Mod },                      // CMOVB & CMOVNE (0F 42)
    { 0x43, ENTRY_CopyBytes2Mod },                      // CMOVAE & CMOVNB (0F 43)
    { 0x44, ENTRY_CopyBytes2Mod },                      // CMOVE & CMOVZ (0F 44)
    { 0x45, ENTRY_CopyBytes2Mod },                      // CMOVNE & CMOVNZ (0F 45)
    { 0x46, ENTRY_CopyBytes2Mod },                      // CMOVBE & CMOVNA (0F 46)
    { 0x47, ENTRY_CopyBytes2Mod },                      // CMOVA & CMOVNBE (0F 47)
    { 0x48, ENTRY_CopyBytes2Mod },                      // CMOVS (0F 48)
    { 0x49, ENTRY_CopyBytes2Mod },                      // CMOVNS (0F 49)
    { 0x4A, ENTRY_CopyBytes2Mod },                      // CMOVP & CMOVPE (0F 4A)
    { 0x4B, ENTRY_CopyBytes2Mod },                      // CMOVNP & CMOVPO (0F 4B)
    { 0x4C, ENTRY_CopyBytes2Mod },                      // CMOVL & CMOVNGE (0F 4C)
    { 0x4D, ENTRY_CopyBytes2Mod },                      // CMOVGE & CMOVNL (0F 4D)
    { 0x4E, ENTRY_CopyBytes2Mod },                      // CMOVLE & CMOVNG (0F 4E)
    { 0x4F, ENTRY_CopyBytes2Mod },                      // CMOVG & CMOVNLE (0F 4F)
    { 0x50, ENTRY_CopyBytes2Mod },                      // MOVMSKPD MOVMSKPD
    { 0x51, ENTRY_CopyBytes2Mod },                      // SQRTPS &
    { 0x52, ENTRY_CopyBytes2Mod },                      // RSQRTTS RSQRTPS
    { 0x53, ENTRY_CopyBytes2Mod },                      // RCPPS RCPSS
    { 0x54, ENTRY_CopyBytes2Mod },                      // ANDPS ANDPD
    { 0x55, ENTRY_CopyBytes2Mod },                      // ANDNPS ANDNPD
    { 0x56, ENTRY_CopyBytes2Mod },                      // ORPS ORPD
    { 0x57, ENTRY_CopyBytes2Mod },                      // XORPS XORPD
    { 0x58, ENTRY_CopyBytes2Mod },                      // ADDPS &
    { 0x59, ENTRY_CopyBytes2Mod },                      // MULPS &
    { 0x5A, ENTRY_CopyBytes2Mod },                      // CVTPS2PD &
    { 0x5B, ENTRY_CopyBytes2Mod },                      // CVTDQ2PS &
    { 0x5C, ENTRY_CopyBytes2Mod },                      // SUBPS &
    { 0x5D, ENTRY_CopyBytes2Mod },                      // MINPS &
    { 0x5E, ENTRY_CopyBytes2Mod },                      // DIVPS &
    { 0x5F, ENTRY_CopyBytes2Mod },                      // MASPS &
    { 0x60, ENTRY_CopyBytes2Mod },                      // PUNPCKLBW/r
    { 0x61, ENTRY_CopyBytes2Mod },                      // PUNPCKLWD/r
    { 0x62, ENTRY_CopyBytes2Mod },                      // PUNPCKLWD/r
    { 0x63, ENTRY_CopyBytes2Mod },                      // PACKSSWB/r
    { 0x64, ENTRY_CopyBytes2Mod },                      // PCMPGTB/r
    { 0x65, ENTRY_CopyBytes2Mod },                      // PCMPGTW/r
    { 0x66, ENTRY_CopyBytes2Mod },                      // PCMPGTD/r
    { 0x67, ENTRY_CopyBytes2Mod },                      // PACKUSWB/r
    { 0x68, ENTRY_CopyBytes2Mod },                      // PUNPCKHBW/r
    { 0x69, ENTRY_CopyBytes2Mod },                      // PUNPCKHWD/r
    { 0x6A, ENTRY_CopyBytes2Mod },                      // PUNPCKHDQ/r
    { 0x6B, ENTRY_CopyBytes2Mod },                      // PACKSSDW/r
    { 0x6C, ENTRY_CopyBytes2Mod },                      // PUNPCKLQDQ
    { 0x6D, ENTRY_CopyBytes2Mod },                      // PUNPCKHQDQ
    { 0x6E, ENTRY_CopyBytes2Mod },                      // MOVD/r
    { 0x6F, ENTRY_CopyBytes2Mod },                      // MOV/r
    { 0x70, ENTRY_CopyBytes2Mod1 },                     // PSHUFW/r ib
    { 0x71, ENTRY_CopyBytes2Mod1 },                     // PSLLW/6 ib,PSRAW/4 ib,PSRLW/2 ib
    { 0x72, ENTRY_CopyBytes2Mod1 },                     // PSLLD/6 ib,PSRAD/4 ib,PSRLD/2 ib
    { 0x73, ENTRY_CopyBytes2Mod1 },                     // PSLLQ/6 ib,PSRLQ/2 ib
    { 0x74, ENTRY_CopyBytes2Mod },                      // PCMPEQB/r
    { 0x75, ENTRY_CopyBytes2Mod },                      // PCMPEQW/r
    { 0x76, ENTRY_CopyBytes2Mod },                      // PCMPEQD/r
    { 0x77, ENTRY_CopyBytes1 },                         // EMMS
    // extrq/insertq require mode=3 and are followed by two immediate bytes
    { 0x78, ENTRY_Copy0F78 },                           // VMREAD/r, 66/EXTRQ/r/ib/ib, F2/INSERTQ/r/ib/ib
    // extrq/insertq require mod=3, therefore ENTRY_CopyBytes2, but it ends up the same
    { 0x79, ENTRY_CopyBytes2Mod },                      // VMWRITE/r, 66/EXTRQ/r, F2/INSERTQ/r
    { 0x7A, ENTRY_Invalid },                            // _7A
    { 0x7B, ENTRY_Invalid },                            // _7B
    { 0x7C, ENTRY_CopyBytes2Mod },                      // HADDPS
    { 0x7D, ENTRY_CopyBytes2Mod },                      // HSUBPS
    { 0x7E, ENTRY_CopyBytes2Mod },                      // MOVD/r
    { 0x7F, ENTRY_CopyBytes2Mod },                      // MOV/r
    { 0x80, ENTRY_CopyBytes3Or5Target },                // JO
    { 0x81, ENTRY_CopyBytes3Or5Target },                // JNO
    { 0x82, ENTRY_CopyBytes3Or5Target },                // JB,JC,JNAE
    { 0x83, ENTRY_CopyBytes3Or5Target },                // JAE,JNB,JNC
    { 0x84, ENTRY_CopyBytes3Or5Target },                // JE,JZ,JZ
    { 0x85, ENTRY_CopyBytes3Or5Target },                // JNE,JNZ
    { 0x86, ENTRY_CopyBytes3Or5Target },                // JBE,JNA
    { 0x87, ENTRY_CopyBytes3Or5Target },                // JA,JNBE
    { 0x88, ENTRY_CopyBytes3Or5Target },                // JS
    { 0x89, ENTRY_CopyBytes3Or5Target },                // JNS
    { 0x8A, ENTRY_CopyBytes3Or5Target },                // JP,JPE
    { 0x8B, ENTRY_CopyBytes3Or5Target },                // JNP,JPO
    { 0x8C, ENTRY_CopyBytes3Or5Target },                // JL,NGE
    { 0x8D, ENTRY_CopyBytes3Or5Target },                // JGE,JNL
    { 0x8E, ENTRY_CopyBytes3Or5Target },                // JLE,JNG
    { 0x8F, ENTRY_CopyBytes3Or5Target },                // JG,JNLE
    { 0x90, ENTRY_CopyBytes2Mod },                      // CMOVO (0F 40)
    { 0x91, ENTRY_CopyBytes2Mod },                      // CMOVNO (0F 41)
    { 0x92, ENTRY_CopyBytes2Mod },                      // CMOVB & CMOVC & CMOVNAE (0F 42)
    { 0x93, ENTRY_CopyBytes2Mod },                      // CMOVAE & CMOVNB & CMOVNC (0F 43)
    { 0x94, ENTRY_CopyBytes2Mod },                      // CMOVE & CMOVZ (0F 44)
    { 0x95, ENTRY_CopyBytes2Mod },                      // CMOVNE & CMOVNZ (0F 45)
    { 0x96, ENTRY_CopyBytes2Mod },                      // CMOVBE & CMOVNA (0F 46)
    { 0x97, ENTRY_CopyBytes2Mod },                      // CMOVA & CMOVNBE (0F 47)
    { 0x98, ENTRY_CopyBytes2Mod },                      // CMOVS (0F 48)
    { 0x99, ENTRY_CopyBytes2Mod },                      // CMOVNS (0F 49)
    { 0x9A, ENTRY_CopyBytes2Mod },                      // CMOVP & CMOVPE (0F 4A)
    { 0x9B, ENTRY_CopyBytes2Mod },                      // CMOVNP & CMOVPO (0F 4B)
    { 0x9C, ENTRY_CopyBytes2Mod },                      // CMOVL & CMOVNGE (0F 4C)
    { 0x9D, ENTRY_CopyBytes2Mod },                      // CMOVGE & CMOVNL (0F 4D)
    { 0x9E, ENTRY_CopyBytes2Mod },                      // CMOVLE & CMOVNG (0F 4E)
    { 0x9F, ENTRY_CopyBytes2Mod },                      // CMOVG & CMOVNLE (0F 4F)
    { 0xA0, ENTRY_CopyBytes1 },                         // PUSH
    { 0xA1, ENTRY_CopyBytes1 },                         // POP
    { 0xA2, ENTRY_CopyBytes1 },                         // CPUID
    { 0xA3, ENTRY_CopyBytes2Mod },                      // BT  (0F A3)
    { 0xA4, ENTRY_CopyBytes2Mod1 },                     // SHLD
    { 0xA5, ENTRY_CopyBytes2Mod },                      // SHLD
    { 0xA6, ENTRY_CopyBytes2Mod },                      // XBTS
    { 0xA7, ENTRY_CopyBytes2Mod },                      // IBTS
    { 0xA8, ENTRY_CopyBytes1 },                         // PUSH
    { 0xA9, ENTRY_CopyBytes1 },                         // POP
    { 0xAA, ENTRY_CopyBytes1 },                         // RSM
    { 0xAB, ENTRY_CopyBytes2Mod },                      // BTS (0F AB)
    { 0xAC, ENTRY_CopyBytes2Mod1 },                     // SHRD
    { 0xAD, ENTRY_CopyBytes2Mod },                      // SHRD

    // 0F AE mod76=mem mod543=0 fxsave
    // 0F AE mod76=mem mod543=1 fxrstor
    // 0F AE mod76=mem mod543=2 ldmxcsr
    // 0F AE mod76=mem mod543=3 stmxcsr
    // 0F AE mod76=mem mod543=4 xsave
    // 0F AE mod76=mem mod543=5 xrstor
    // 0F AE mod76=mem mod543=6 saveopt
    // 0F AE mod76=mem mod543=7 clflush
    // 0F AE mod76=11b mod543=5 lfence
    // 0F AE mod76=11b mod543=6 mfence
    // 0F AE mod76=11b mod543=7 sfence
    // F3 0F AE mod76=11b mod543=0 rdfsbase
    // F3 0F AE mod76=11b mod543=1 rdgsbase
    // F3 0F AE mod76=11b mod543=2 wrfsbase
    // F3 0F AE mod76=11b mod543=3 wrgsbase
    { 0xAE, ENTRY_CopyBytes2Mod },                      // fxsave fxrstor ldmxcsr stmxcsr xsave xrstor saveopt clflush lfence mfence sfence rdfsbase rdgsbase wrfsbase wrgsbase
    { 0xAF, ENTRY_CopyBytes2Mod },                      // IMUL (0F AF)
    { 0xB0, ENTRY_CopyBytes2Mod },                      // CMPXCHG (0F B0)
    { 0xB1, ENTRY_CopyBytes2Mod },                      // CMPXCHG (0F B1)
    { 0xB2, ENTRY_CopyBytes2Mod },                      // LSS/r
    { 0xB3, ENTRY_CopyBytes2Mod },                      // BTR (0F B3)
    { 0xB4, ENTRY_CopyBytes2Mod },                      // LFS/r
    { 0xB5, ENTRY_CopyBytes2Mod },                      // LGS/r
    { 0xB6, ENTRY_CopyBytes2Mod },                      // MOVZX/r
    { 0xB7, ENTRY_CopyBytes2Mod },                      // MOVZX/r
#ifdef DETOURS_X86
    { 0xB8, ENTRY_Copy0FB8 },                           // jmpe f3/popcnt
#else
    { 0xB8, ENTRY_CopyBytes2Mod },                      // f3/popcnt
#endif
    { 0xB9, ENTRY_Invalid },                            // _B9
    { 0xBA, ENTRY_CopyBytes2Mod1 },                     // BT & BTC & BTR & BTS (0F BA)
    { 0xBB, ENTRY_CopyBytes2Mod },                      // BTC (0F BB)
    { 0xBC, ENTRY_CopyBytes2Mod },                      // BSF (0F BC)
    { 0xBD, ENTRY_CopyBytes2Mod },                      // BSR (0F BD)
    { 0xBE, ENTRY_CopyBytes2Mod },                      // MOVSX/r
    { 0xBF, ENTRY_CopyBytes2Mod },                      // MOVSX/r
    { 0xC0, ENTRY_CopyBytes2Mod },                      // XADD/r
    { 0xC1, ENTRY_CopyBytes2Mod },                      // XADD/r
    { 0xC2, ENTRY_CopyBytes2Mod1 },                     // CMPPS &
    { 0xC3, ENTRY_CopyBytes2Mod },                      // MOVNTI
    { 0xC4, ENTRY_CopyBytes2Mod1 },                     // PINSRW /r ib
    { 0xC5, ENTRY_CopyBytes2Mod1 },                     // PEXTRW /r ib
    { 0xC6, ENTRY_CopyBytes2Mod1 },                     // SHUFPS & SHUFPD
    { 0xC7, ENTRY_CopyBytes2Mod },                      // CMPXCHG8B (0F C7)
    { 0xC8, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xC9, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xCA, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xCB, ENTRY_CopyBytes1 },                         // CVTPD2PI BSWAP 0F C8 + rd
    { 0xCC, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xCD, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xCE, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xCF, ENTRY_CopyBytes1 },                         // BSWAP 0F C8 + rd
    { 0xD0, ENTRY_CopyBytes2Mod },                      // ADDSUBPS (untestd)
    { 0xD1, ENTRY_CopyBytes2Mod },                      // PSRLW/r
    { 0xD2, ENTRY_CopyBytes2Mod },                      // PSRLD/r
    { 0xD3, ENTRY_CopyBytes2Mod },                      // PSRLQ/r
    { 0xD4, ENTRY_CopyBytes2Mod },                      // PADDQ
    { 0xD5, ENTRY_CopyBytes2Mod },                      // PMULLW/r
    { 0xD6, ENTRY_CopyBytes2Mod },                      // MOVDQ2Q / MOVQ2DQ
    { 0xD7, ENTRY_CopyBytes2Mod },                      // PMOVMSKB/r
    { 0xD8, ENTRY_CopyBytes2Mod },                      // PSUBUSB/r
    { 0xD9, ENTRY_CopyBytes2Mod },                      // PSUBUSW/r
    { 0xDA, ENTRY_CopyBytes2Mod },                      // PMINUB/r
    { 0xDB, ENTRY_CopyBytes2Mod },                      // PAND/r
    { 0xDC, ENTRY_CopyBytes2Mod },                      // PADDUSB/r
    { 0xDD, ENTRY_CopyBytes2Mod },                      // PADDUSW/r
    { 0xDE, ENTRY_CopyBytes2Mod },                      // PMAXUB/r
    { 0xDF, ENTRY_CopyBytes2Mod },                      // PANDN/r
    { 0xE0, ENTRY_CopyBytes2Mod  },                     // PAVGB
    { 0xE1, ENTRY_CopyBytes2Mod },                      // PSRAW/r
    { 0xE2, ENTRY_CopyBytes2Mod },                      // PSRAD/r
    { 0xE3, ENTRY_CopyBytes2Mod },                      // PAVGW
    { 0xE4, ENTRY_CopyBytes2Mod },                      // PMULHUW/r
    { 0xE5, ENTRY_CopyBytes2Mod },                      // PMULHW/r
    { 0xE6, ENTRY_CopyBytes2Mod },                      // CTDQ2PD &
    { 0xE7, ENTRY_CopyBytes2Mod },                      // MOVNTQ
    { 0xE8, ENTRY_CopyBytes2Mod },                      // PSUBB/r
    { 0xE9, ENTRY_CopyBytes2Mod },                      // PSUBW/r
    { 0xEA, ENTRY_CopyBytes2Mod },                      // PMINSW/r
    { 0xEB, ENTRY_CopyBytes2Mod },                      // POR/r
    { 0xEC, ENTRY_CopyBytes2Mod },                      // PADDSB/r
    { 0xED, ENTRY_CopyBytes2Mod },                      // PADDSW/r
    { 0xEE, ENTRY_CopyBytes2Mod },                      // PMAXSW /r
    { 0xEF, ENTRY_CopyBytes2Mod },                      // PXOR/r
    { 0xF0, ENTRY_CopyBytes2Mod },                      // LDDQU
    { 0xF1, ENTRY_CopyBytes2Mod },                      // PSLLW/r
    { 0xF2, ENTRY_CopyBytes2Mod },                      // PSLLD/r
    { 0xF3, ENTRY_CopyBytes2Mod },                      // PSLLQ/r
    { 0xF4, ENTRY_CopyBytes2Mod },                      // PMULUDQ/r
    { 0xF5, ENTRY_CopyBytes2Mod },                      // PMADDWD/r
    { 0xF6, ENTRY_CopyBytes2Mod },                      // PSADBW/r
    { 0xF7, ENTRY_CopyBytes2Mod },                      // MASKMOVQ
    { 0xF8, ENTRY_CopyBytes2Mod },                      // PSUBB/r
    { 0xF9, ENTRY_CopyBytes2Mod },                      // PSUBW/r
    { 0xFA, ENTRY_CopyBytes2Mod },                      // PSUBD/r
    { 0xFB, ENTRY_CopyBytes2Mod },                      // FSUBQ/r
    { 0xFC, ENTRY_CopyBytes2Mod },                      // PADDB/r
    { 0xFD, ENTRY_CopyBytes2Mod },                      // PADDW/r
    { 0xFE, ENTRY_CopyBytes2Mod },                      // PADDD/r
    { 0xFF, ENTRY_Invalid },                            // _FF
    { 0, ENTRY_End },
};

BOOL CDetourDis::SanityCheckSystem()
{
    ULONG n = 0;
    for (; n < 256; n++) {
        REFCOPYENTRY pEntry = &s_rceCopyTable[n];

        if (n != pEntry->nOpcode) {
            ASSERT(n == pEntry->nOpcode);
            return FALSE;
        }
    }
    if (s_rceCopyTable[256].pfCopy != NULL) {
        ASSERT(!"Missing end marker.");
        return FALSE;
    }

    for (n = 0; n < 256; n++) {
        REFCOPYENTRY pEntry = &s_rceCopyTable0F[n];

        if (n != pEntry->nOpcode) {
            ASSERT(n == pEntry->nOpcode);
            return FALSE;
        }
    }
    if (s_rceCopyTable0F[256].pfCopy != NULL) {
        ASSERT(!"Missing end marker.");
        return FALSE;
    }

    return TRUE;
}
#endif // defined(DETOURS_X64) || defined(DETOURS_X86)

/////////////////////////////////////////////////////////// IA64 Disassembler.
//
#ifdef DETOURS_IA64
#error Feature not supported in this release.

























































































































































































































































































































































































































































































































































































































































































#endif // DETOURS_IA64

#ifdef DETOURS_ARM
#error Feature not supported in this release.





























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































#endif // DETOURS_ARM

#ifdef DETOURS_ARM64

#define c_LR        30          // The register number for the Link Register
#define c_SP        31          // The register number for the Stack Pointer
#define c_NOP       0xd503201f  // A nop instruction
#define c_BREAK     (0xd4200000 | (0xf000 << 5)) // A break instruction

//
// Problematic instructions:
//
// ADR     0ll10000 hhhhhhhh hhhhhhhh hhhddddd  & 0x9f000000 == 0x10000000  (l = low, h = high, d = Rd)
// ADRP    1ll10000 hhhhhhhh hhhhhhhh hhhddddd  & 0x9f000000 == 0x90000000  (l = low, h = high, d = Rd)
//
// B.cond  01010100 iiiiiiii iiiiiiii iii0cccc  & 0xff000010 == 0x54000000  (i = delta = SignExtend(imm19:00, 64), c = cond)
//
// B       000101ii iiiiiiii iiiiiiii iiiiiiii  & 0xfc000000 == 0x14000000  (i = delta = SignExtend(imm26:00, 64))
// BL      100101ii iiiiiiii iiiiiiii iiiiiiii  & 0xfc000000 == 0x94000000  (i = delta = SignExtend(imm26:00, 64))
//
// CBNZ    z0110101 iiiiiiii iiiiiiii iiittttt  & 0x7f000000 == 0x35000000  (z = size, i = delta = SignExtend(imm19:00, 64), t = Rt)
// CBZ     z0110100 iiiiiiii iiiiiiii iiittttt  & 0x7f000000 == 0x34000000  (z = size, i = delta = SignExtend(imm19:00, 64), t = Rt)
//
// LDR Wt  00011000 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x18000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDR Xt  01011000 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x58000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDRSW   10011000 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x98000000  (i = SignExtend(imm19:00, 64), t = Rt)
// PRFM    11011000 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0xd8000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDR St  00011100 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x1c000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDR Dt  01011100 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x5c000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDR Qt  10011100 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0x9c000000  (i = SignExtend(imm19:00, 64), t = Rt)
// LDR inv 11011100 iiiiiiii iiiiiiii iiittttt  & 0xff000000 == 0xdc000000  (i = SignExtend(imm19:00, 64), t = Rt)
//
// TBNZ    z0110111 bbbbbiii iiiiiiii iiittttt  & 0x7f000000 == 0x37000000  (z = size, b = bitnum, i = SignExtend(imm14:00, 64), t = Rt)
// TBZ     z0110110 bbbbbiii iiiiiiii iiittttt  & 0x7f000000 == 0x36000000  (z = size, b = bitnum, i = SignExtend(imm14:00, 64), t = Rt)
//

class CDetourDis
{
  public:
    CDetourDis();

    PBYTE   CopyInstruction(PBYTE pDst,
                            PBYTE pSrc,
                            PBYTE *ppTarget,
                            LONG *plExtra);

  public:
    typedef BYTE (CDetourDis::* COPYFUNC)(PBYTE pbDst, PBYTE pbSrc);

    union AddImm12
    {
        DWORD Assembled;
        struct
        {
            DWORD Rd : 5;           // Destination register
            DWORD Rn : 5;           // Source register
            DWORD Imm12 : 12;       // 12-bit immediate
            DWORD Shift : 2;        // shift (must be 0 or 1)
            DWORD Opcode1 : 7;      // Must be 0010001 == 0x11
            DWORD Size : 1;         // 0 = 32-bit, 1 = 64-bit
        } s;
        static DWORD Assemble(DWORD size, DWORD rd, DWORD rn, ULONG imm, DWORD shift)
        {
            AddImm12 temp;
            temp.s.Rd = rd;
            temp.s.Rn = rn;
            temp.s.Imm12 = imm & 0xfff;
            temp.s.Shift = shift;
            temp.s.Opcode1 = 0x11;
            temp.s.Size = size;
            return temp.Assembled;
        }
        static DWORD AssembleAdd32(DWORD rd, DWORD rn, ULONG imm, DWORD shift) { return Assemble(0, rd, rn, imm, shift); }
        static DWORD AssembleAdd64(DWORD rd, DWORD rn, ULONG imm, DWORD shift) { return Assemble(1, rd, rn, imm, shift); }
    };

    union Adr19
    {
        DWORD Assembled;
        struct
        {
            DWORD Rd : 5;           // Destination register
            DWORD Imm19 : 19;       // 19-bit upper immediate
            DWORD Opcode1 : 5;      // Must be 10000 == 0x10
            DWORD Imm2 : 2;         // 2-bit lower immediate
            DWORD Type : 1;         // 0 = ADR, 1 = ADRP
        } s;
        inline LONG Imm() const { DWORD Imm = (s.Imm19 << 2) | s.Imm2; return (LONG)(Imm << 11) >> 11; }
        static DWORD Assemble(DWORD type, DWORD rd, LONG delta)
        {
            Adr19 temp;
            temp.s.Rd = rd;
            temp.s.Imm19 = (delta >> 2) & 0x7ffff;
            temp.s.Opcode1 = 0x10;
            temp.s.Imm2 = delta & 3;
            temp.s.Type = type;
            return temp.Assembled;
        }
        static DWORD AssembleAdr(DWORD rd, LONG delta) { return Assemble(0, rd, delta); }
        static DWORD AssembleAdrp(DWORD rd, LONG delta) { return Assemble(1, rd, delta); }
    };

    union Bcc19
    {
        DWORD Assembled;
        struct
        {
            DWORD Condition : 4;    // Condition
            DWORD Opcode1 : 1;      // Must be 0
            DWORD Imm19 : 19;       // 19-bit immediate
            DWORD Opcode2 : 8;      // Must be 01010100 == 0x54
        } s;
        inline LONG Imm() const { return (LONG)(s.Imm19 << 13) >> 11; }
        static DWORD AssembleBcc(DWORD condition, LONG delta)
        {
            Bcc19 temp;
            temp.s.Condition = condition;
            temp.s.Opcode1 = 0;
            temp.s.Imm19 = delta >> 2;
            temp.s.Opcode2 = 0x54;
            return temp.Assembled;
        }
    };

    union Branch26
    {
        DWORD Assembled;
        struct
        {
            DWORD Imm26 : 26;       // 26-bit immediate
            DWORD Opcode1 : 5;      // Must be 00101 == 0x5
            DWORD Link : 1;         // 0 = B, 1 = BL
        } s;
        inline LONG Imm() const { return (LONG)(s.Imm26 << 6) >> 4; }
        static DWORD Assemble(DWORD link, LONG delta)
        {
            Branch26 temp;
            temp.s.Imm26 = delta >> 2;
            temp.s.Opcode1 = 0x5;
            temp.s.Link = link;
            return temp.Assembled;
        }
        static DWORD AssembleB(LONG delta) { return Assemble(0, delta); }
        static DWORD AssembleBl(LONG delta) { return Assemble(1, delta); }
    };

    union Br
    {
        DWORD Assembled;
        struct
        {
            DWORD Opcode1 : 5;      // Must be 00000 == 0
            DWORD Rn : 5;           // Register number
            DWORD Opcode2 : 22;     // Must be 1101011000011111000000 == 0x3587c0
        } s;
        static DWORD AssembleBr(DWORD rn)
        {
            Br temp;
            temp.s.Opcode1 = 0;
            temp.s.Rn = rn;
            temp.s.Opcode2 = 0x3587c0;
            return temp.Assembled;
        }
    };

    union Cbz19
    {
        DWORD Assembled;
        struct
        {
            DWORD Rt : 5;           // Register to test
            DWORD Imm19 : 19;       // 19-bit immediate
            DWORD Nz : 1;           // 0 = CBZ, 1 = CBNZ
            DWORD Opcode1 : 6;      // Must be 011010 == 0x1a
            DWORD Size : 1;         // 0 = 32-bit, 1 = 64-bit
        } s;
        inline LONG Imm() const { return (LONG)(s.Imm19 << 13) >> 11; }
        static DWORD Assemble(DWORD size, DWORD nz, DWORD rt, LONG delta)
        {
            Cbz19 temp;
            temp.s.Rt = rt;
            temp.s.Imm19 = delta >> 2;
            temp.s.Nz = nz;
            temp.s.Opcode1 = 0x1a;
            temp.s.Size = size;
            return temp.Assembled;
        }
    };

    union LdrLit19
    {
        DWORD Assembled;
        struct
        {
            DWORD Rt : 5;           // Destination register
            DWORD Imm19 : 19;       // 19-bit immediate
            DWORD Opcode1 : 2;      // Must be 0
            DWORD FpNeon : 1;       // 0 = LDR Wt/LDR Xt/LDRSW/PRFM, 1 = LDR St/LDR Dt/LDR Qt
            DWORD Opcode2 : 3;      // Must be 011 = 3
            DWORD Size : 2;         // 00 = LDR Wt/LDR St, 01 = LDR Xt/LDR Dt, 10 = LDRSW/LDR Qt, 11 = PRFM/invalid
        } s;
        inline LONG Imm() const { return (LONG)(s.Imm19 << 13) >> 11; }
        static DWORD Assemble(DWORD size, DWORD fpneon, DWORD rt, LONG delta)
        {
            LdrLit19 temp;
            temp.s.Rt = rt;
            temp.s.Imm19 = delta >> 2;
            temp.s.Opcode1 = 0;
            temp.s.FpNeon = fpneon;
            temp.s.Opcode2 = 3;
            temp.s.Size = size;
            return temp.Assembled;
        }
    };

    union LdrFpNeonImm9
    {
        DWORD Assembled;
        struct
        {
            DWORD Rt : 5;           // Destination register
            DWORD Rn : 5;           // Base register
            DWORD Imm12 : 12;       // 12-bit immediate
            DWORD Opcode1 : 1;      // Must be 1 == 1
            DWORD Opc : 1;          // Part of size
            DWORD Opcode2 : 6;      // Must be 111101 == 0x3d
            DWORD Size : 2;         // Size (0=8-bit, 1=16-bit, 2=32-bit, 3=64-bit, 4=128-bit)
        } s;
        static DWORD Assemble(DWORD size, DWORD rt, DWORD rn, ULONG imm)
        {
            LdrFpNeonImm9 temp;
            temp.s.Rt = rt;
            temp.s.Rn = rn;
            temp.s.Imm12 = imm;
            temp.s.Opcode1 = 1;
            temp.s.Opc = size >> 2;
            temp.s.Opcode2 = 0x3d;
            temp.s.Size = size & 3;
            return temp.Assembled;
        }
    };

    union Mov16
    {
        DWORD Assembled;
        struct
        {
            DWORD Rd : 5;           // Destination register
            DWORD Imm16 : 16;       // Immediate
            DWORD Shift : 2;        // Shift amount (0=0, 1=16, 2=32, 3=48)
            DWORD Opcode : 6;       // Must be 100101 == 0x25
            DWORD Type : 2;         // 0 = MOVN, 1 = reserved, 2 = MOVZ, 3 = MOVK
            DWORD Size : 1;         // 0 = 32-bit, 1 = 64-bit
        } s;
        static DWORD Assemble(DWORD size, DWORD type, DWORD rd, DWORD imm, DWORD shift)
        {
            Mov16 temp;
            temp.s.Rd = rd;
            temp.s.Imm16 = imm;
            temp.s.Shift = shift;
            temp.s.Opcode = 0x25;
            temp.s.Type = type;
            temp.s.Size = size;
            return temp.Assembled;
        }
        static DWORD AssembleMovn32(DWORD rd, DWORD imm, DWORD shift) { return Assemble(0, 0, rd, imm, shift); }
        static DWORD AssembleMovn64(DWORD rd, DWORD imm, DWORD shift) { return Assemble(1, 0, rd, imm, shift); }
        static DWORD AssembleMovz32(DWORD rd, DWORD imm, DWORD shift) { return Assemble(0, 2, rd, imm, shift); }
        static DWORD AssembleMovz64(DWORD rd, DWORD imm, DWORD shift) { return Assemble(1, 2, rd, imm, shift); }
        static DWORD AssembleMovk32(DWORD rd, DWORD imm, DWORD shift) { return Assemble(0, 3, rd, imm, shift); }
        static DWORD AssembleMovk64(DWORD rd, DWORD imm, DWORD shift) { return Assemble(1, 3, rd, imm, shift); }
    };

    union Tbz14
    {
        DWORD Assembled;
        struct
        {
            DWORD Rt : 5;           // Register to test
            DWORD Imm14 : 14;       // 14-bit immediate
            DWORD Bit : 5;          // 5-bit index
            DWORD Nz : 1;           // 0 = TBZ, 1 = TBNZ
            DWORD Opcode1 : 6;      // Must be 011011 == 0x1b
            DWORD Size : 1;         // 0 = 32-bit, 1 = 64-bit
        } s;
        inline LONG Imm() const { return (LONG)(s.Imm14 << 18) >> 16; }
        static DWORD Assemble(DWORD size, DWORD nz, DWORD rt, DWORD bit, LONG delta)
        {
            Tbz14 temp;
            temp.s.Rt = rt;
            temp.s.Imm14 = delta >> 2;
            temp.s.Bit = bit;
            temp.s.Nz = nz;
            temp.s.Opcode1 = 0x1b;
            temp.s.Size = size;
            return temp.Assembled;
        }
    };


  protected:
    BYTE    PureCopy32(BYTE* pSource, BYTE* pDest);
    BYTE    EmitMovImmediate(PULONG& pDstInst, BYTE rd, UINT64 immediate);
    BYTE    CopyAdr(BYTE* pSource, BYTE* pDest, ULONG instruction);
    BYTE    CopyBcc(BYTE* pSource, BYTE* pDest, ULONG instruction);
    BYTE    CopyB(BYTE* pSource, BYTE* pDest, ULONG instruction);
    BYTE    CopyCbz(BYTE* pSource, BYTE* pDest, ULONG instruction);
    BYTE    CopyTbz(BYTE* pSource, BYTE* pDest, ULONG instruction);
    BYTE    CopyLdrLiteral(BYTE* pSource, BYTE* pDest, ULONG instruction);

  protected:
    ULONG GetInstruction(BYTE* pSource)
    {
        return ((PULONG)pSource)[0];
    }

    BYTE EmitInstruction(PULONG& pDstInst, ULONG instruction)
    {
        *pDstInst++ = instruction;
        return sizeof(ULONG);
    }

  protected:
    PBYTE   m_pbTarget;
    BYTE    m_rbScratchDst[64];
};

BYTE CDetourDis::PureCopy32(BYTE* pSource, BYTE* pDest)
{
    *(ULONG *)pDest = *(ULONG*)pSource;
    return sizeof(DWORD);
}

/////////////////////////////////////////////////////////// Disassembler Code.
//
CDetourDis::CDetourDis()
{
    m_pbTarget = (PBYTE)DETOUR_INSTRUCTION_TARGET_NONE;
}

PBYTE CDetourDis::CopyInstruction(PBYTE pDst,
                                  PBYTE pSrc,
                                  PBYTE *ppTarget,
                                  LONG *plExtra)
{
    if (pDst == NULL) {
        pDst = m_rbScratchDst;
    }

    DWORD Instruction = GetInstruction(pSrc);

    ULONG CopiedSize;
    if ((Instruction & 0x1f000000) == 0x10000000) {
        CopiedSize = CopyAdr(pSrc, pDst, Instruction);
    } else if ((Instruction & 0xff000010) == 0x54000000) {
        CopiedSize = CopyBcc(pSrc, pDst, Instruction);
    } else if ((Instruction & 0x7c000000) == 0x14000000) {
        CopiedSize = CopyB(pSrc, pDst, Instruction);
    } else if ((Instruction & 0x7e000000) == 0x34000000) {
        CopiedSize = CopyCbz(pSrc, pDst, Instruction);
    } else if ((Instruction & 0x7e000000) == 0x36000000) {
        CopiedSize = CopyTbz(pSrc, pDst, Instruction);
    } else if ((Instruction & 0x3b000000) == 0x18000000) {
        CopiedSize = CopyLdrLiteral(pSrc, pDst, Instruction);
    } else {
        CopiedSize = PureCopy32(pSrc, pDst);
    }

    // If the target is needed, store our target
    if (ppTarget) {
        *ppTarget = m_pbTarget;
    }
    if (plExtra) {
        *plExtra = CopiedSize - sizeof(DWORD);
    }

    return pSrc + 4;
}

BYTE CDetourDis::EmitMovImmediate(PULONG& pDstInst, BYTE rd, UINT64 immediate)
{
    DWORD piece[4];
    piece[3] = (DWORD)((immediate >> 48) & 0xffff);
    piece[2] = (DWORD)((immediate >> 32) & 0xffff);
    piece[1] = (DWORD)((immediate >> 16) & 0xffff);
    piece[0] = (DWORD)((immediate >> 0) & 0xffff);
    int count = 0;

    // special case: MOVN with 32-bit dest
    if (piece[3] == 0 && piece[2] == 0 && piece[1] == 0xffff)
    {
        EmitInstruction(pDstInst, Mov16::AssembleMovn32(rd, piece[0] ^ 0xffff, 0));
        count++;
    }

    // MOVN/MOVZ with 64-bit dest
    else
    {
        int zero_pieces = (piece[3] == 0x0000) + (piece[2] == 0x0000) + (piece[1] == 0x0000) + (piece[0] == 0x0000);
        int ffff_pieces = (piece[3] == 0xffff) + (piece[2] == 0xffff) + (piece[1] == 0xffff) + (piece[0] == 0xffff);
        DWORD defaultPiece = (ffff_pieces > zero_pieces) ? 0xffff : 0x0000;
        bool first = true;
        for (int pieceNum = 3; pieceNum >= 0; pieceNum--)
        {
            DWORD curPiece = piece[pieceNum];
            if (curPiece != defaultPiece || (pieceNum == 0 && first))
            {
                count++;
                if (first)
                {
                    if (defaultPiece == 0xffff)
                    {
                        EmitInstruction(pDstInst, Mov16::AssembleMovn64(rd, curPiece ^ 0xffff, pieceNum));
                    }
                    else
                    {
                        EmitInstruction(pDstInst, Mov16::AssembleMovz64(rd, curPiece, pieceNum));
                    }
                    first = false;
                }
                else
                {
                    EmitInstruction(pDstInst, Mov16::AssembleMovk64(rd, curPiece, pieceNum));
                }
            }
        }
    }
    return (BYTE)(count * sizeof(DWORD));
}

BYTE CDetourDis::CopyAdr(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    Adr19& decoded = (Adr19&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    // ADR case
    if (decoded.s.Type == 0)
    {
        BYTE* pTarget = pSource + decoded.Imm();
        LONG64 delta = pTarget - pDest;
        LONG64 deltaPage = ((ULONG_PTR)pTarget >> 12) - ((ULONG_PTR)pDest >> 12);

        // output as ADR
        if (delta >= -(1 << 20) && delta < (1 << 20))
        {
            EmitInstruction(pDstInst, Adr19::AssembleAdr(decoded.s.Rd, (LONG)delta));
        }

        // output as ADRP; ADD
        else if (deltaPage >= -(1 << 20) && (deltaPage < (1 << 20)))
        {
            EmitInstruction(pDstInst, Adr19::AssembleAdrp(decoded.s.Rd, (LONG)deltaPage));
            EmitInstruction(pDstInst, AddImm12::AssembleAdd32(decoded.s.Rd, decoded.s.Rd, ((ULONG)(ULONG_PTR)pTarget) & 0xfff, 0));
        }

        // output as immediate move
        else
        {
            EmitMovImmediate(pDstInst, decoded.s.Rd, (ULONG_PTR)pTarget);
        }
    }

    // ADRP case
    else
    {
        BYTE* pTarget = (BYTE*)((((ULONG_PTR)pSource >> 12) + decoded.Imm()) << 12);
        LONG64 deltaPage = ((ULONG_PTR)pTarget >> 12) - ((ULONG_PTR)pDest >> 12);

        // output as ADRP
        if (deltaPage >= -(1 << 20) && (deltaPage < (1 << 20)))
        {
            EmitInstruction(pDstInst, Adr19::AssembleAdrp(decoded.s.Rd, (LONG)deltaPage));
        }

        // output as immediate move
        else
        {
            EmitMovImmediate(pDstInst, decoded.s.Rd, (ULONG_PTR)pTarget);
        }
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}

BYTE CDetourDis::CopyBcc(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    Bcc19& decoded = (Bcc19&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    BYTE* pTarget = pSource + decoded.Imm();
    m_pbTarget = pTarget;
    LONG64 delta = pTarget - pDest;
    LONG64 delta4 = pTarget - (pDest + 4);

    // output as BCC
    if (delta >= -(1 << 20) && delta < (1 << 20))
    {
        EmitInstruction(pDstInst, Bcc19::AssembleBcc(decoded.s.Condition, (LONG)delta));
    }

    // output as BCC <skip>; B
    else if (delta4 >= -(1 << 27) && (delta4 < (1 << 27)))
    {
        EmitInstruction(pDstInst, Bcc19::AssembleBcc(decoded.s.Condition ^ 1, 8));
        EmitInstruction(pDstInst, Branch26::AssembleB((LONG)delta4));
    }

    // output as MOV x17, Target; BCC <skip>; BR x17 (BIG assumption that x17 isn't being used for anything!!)
    else
    {
        EmitMovImmediate(pDstInst, 17, (ULONG_PTR)pTarget);
        EmitInstruction(pDstInst, Bcc19::AssembleBcc(decoded.s.Condition ^ 1, 8));
        EmitInstruction(pDstInst, Br::AssembleBr(17));
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}

BYTE CDetourDis::CopyB(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    Branch26& decoded = (Branch26&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    BYTE* pTarget = pSource + decoded.Imm();
    m_pbTarget = pTarget;
    LONG64 delta = pTarget - pDest;

    // output as B
    if (delta >= -(1 << 27) && (delta < (1 << 27)))
    {
        EmitInstruction(pDstInst, Branch26::AssembleB((LONG)delta));
    }

    // output as MOV x17, Target; BR x17 (BIG assumption that x17 isn't being used for anything!!)
    else
    {
        EmitMovImmediate(pDstInst, 17, (ULONG_PTR)pTarget);
        EmitInstruction(pDstInst, Br::AssembleBr(17));
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}

BYTE CDetourDis::CopyCbz(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    Cbz19& decoded = (Cbz19&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    BYTE* pTarget = pSource + decoded.Imm();
    m_pbTarget = pTarget;
    LONG64 delta = pTarget - pDest;
    LONG64 delta4 = pTarget - (pDest + 4);

    // output as CBZ/NZ
    if (delta >= -(1 << 20) && delta < (1 << 20))
    {
        EmitInstruction(pDstInst, Cbz19::Assemble(decoded.s.Size, decoded.s.Nz, decoded.s.Rt, (LONG)delta));
    }

    // output as CBNZ/Z <skip>; B
    else if (delta4 >= -(1 << 27) && (delta4 < (1 << 27)))
    {
        EmitInstruction(pDstInst, Cbz19::Assemble(decoded.s.Size, decoded.s.Nz ^ 1, decoded.s.Rt, 8));
        EmitInstruction(pDstInst, Branch26::AssembleB((LONG)delta4));
    }

    // output as MOV x17, Target; CBNZ/Z <skip>; BR x17 (BIG assumption that x17 isn't being used for anything!!)
    else
    {
        EmitMovImmediate(pDstInst, 17, (ULONG_PTR)pTarget);
        EmitInstruction(pDstInst, Cbz19::Assemble(decoded.s.Size, decoded.s.Nz ^ 1, decoded.s.Rt, 8));
        EmitInstruction(pDstInst, Br::AssembleBr(17));
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}

BYTE CDetourDis::CopyTbz(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    Tbz14& decoded = (Tbz14&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    BYTE* pTarget = pSource + decoded.Imm();
    m_pbTarget = pTarget;
    LONG64 delta = pTarget - pDest;
    LONG64 delta4 = pTarget - (pDest + 4);

    // output as TBZ/NZ
    if (delta >= -(1 << 13) && delta < (1 << 13))
    {
        EmitInstruction(pDstInst, Tbz14::Assemble(decoded.s.Size, decoded.s.Nz, decoded.s.Rt, decoded.s.Bit, (LONG)delta));
    }

    // output as TBNZ/Z <skip>; B
    else if (delta4 >= -(1 << 27) && (delta4 < (1 << 27)))
    {
        EmitInstruction(pDstInst, Tbz14::Assemble(decoded.s.Size, decoded.s.Nz ^ 1, decoded.s.Rt, decoded.s.Bit, 8));
        EmitInstruction(pDstInst, Branch26::AssembleB((LONG)delta4));
    }

    // output as MOV x17, Target; TBNZ/Z <skip>; BR x17 (BIG assumption that x17 isn't being used for anything!!)
    else
    {
        EmitMovImmediate(pDstInst, 17, (ULONG_PTR)pTarget);
        EmitInstruction(pDstInst, Tbz14::Assemble(decoded.s.Size, decoded.s.Nz ^ 1, decoded.s.Rt, decoded.s.Bit, 8));
        EmitInstruction(pDstInst, Br::AssembleBr(17));
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}

BYTE CDetourDis::CopyLdrLiteral(BYTE* pSource, BYTE* pDest, ULONG instruction)
{
    LdrLit19& decoded = (LdrLit19&)(instruction);
    PULONG pDstInst = (PULONG)(pDest);

    BYTE* pTarget = pSource + decoded.Imm();
    LONG64 delta = pTarget - pDest;

    // output as LDR
    if (delta >= -(1 << 21) && delta < (1 << 21))
    {
        EmitInstruction(pDstInst, LdrLit19::Assemble(decoded.s.Size, decoded.s.FpNeon, decoded.s.Rt, (LONG)delta));
    }

    // output as move immediate
    else if (decoded.s.FpNeon == 0)
    {
        UINT64 value = 0;
        switch (decoded.s.Size)
        {
            case 0: value = *(ULONG*)pTarget;       break;
            case 1: value = *(UINT64*)pTarget;   break;
            case 2: value = *(LONG*)pTarget;        break;
        }
        EmitMovImmediate(pDstInst, decoded.s.Rt, value);
    }

    // FP/NEON register: compute address in x17 and load from there (BIG assumption that x17 isn't being used for anything!!)
    else
    {
        EmitMovImmediate(pDstInst, 17, (ULONG_PTR)pTarget);
        EmitInstruction(pDstInst, LdrFpNeonImm9::Assemble(2 + decoded.s.Size, decoded.s.Rt, 17, 0));
    }

    return (BYTE)((BYTE*)pDstInst - pDest);
}


PVOID WINAPI DetourCopyInstruction(_In_opt_ PVOID pDst,
                                   _Inout_opt_ PVOID *ppDstPool,
                                   _In_ PVOID pSrc,
                                   _Out_opt_ PVOID *ppTarget,
                                   _Out_opt_ LONG *plExtra)
{
    UNREFERENCED_PARAMETER(ppDstPool);

    CDetourDis state;
    return (PVOID)state.CopyInstruction((PBYTE)pDst,
                                        (PBYTE)pSrc,
                                        (PBYTE*)ppTarget,
                                        plExtra);
}

#endif // DETOURS_ARM64

BOOL WINAPI DetourSetCodeModule(_In_ HMODULE hModule,
                                _In_ BOOL fLimitReferencesToModule)
{
#if defined(DETOURS_X64) || defined(DETOURS_X86)
    PBYTE pbBeg = NULL;
    PBYTE pbEnd = (PBYTE)~(ULONG_PTR)0;

    if (hModule != NULL) {
        ULONG cbModule = DetourGetModuleSize(hModule);

        pbBeg = (PBYTE)hModule;
        pbEnd = (PBYTE)hModule + cbModule;
    }

    return CDetourDis::SetCodeModule(pbBeg, pbEnd, fLimitReferencesToModule);
#elif defined(DETOURS_ARM) || defined(DETOURS_ARM64) || defined(DETOURS_IA64)
    (void)hModule;
    (void)fLimitReferencesToModule;
    return TRUE;
#else
#error unknown architecture (x86, x64, arm, arm64, ia64)
#endif
}

//
///////////////////////////////////////////////////////////////// End of File.
