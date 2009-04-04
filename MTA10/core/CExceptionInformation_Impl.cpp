/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CExceptionInformation_Impl.cpp
*  PURPOSE:     Exception information parser
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CExceptionInformation_Impl::CExceptionInformation_Impl ( void )
{
    m_uiCode = 0;
    m_pOffset = NULL;
    m_pReferencingOffset = NULL;
    m_ulEAX = 0;
    m_ulEBX = 0;
    m_ulECX = 0;
    m_ulEDX = 0;
    m_ulESI = 0;
    m_ulEDI = 0;
    m_ulEBP = 0;
    m_ulESP = 0;
    m_ulEIP = 0;
    m_ulCS = 0;
    m_ulDS = 0;
    m_ulES = 0;
    m_ulFS = 0;
    m_ulGS = 0;
    m_ulSS = 0;
    m_ulEFlags = 0;
}


void CExceptionInformation_Impl::Set ( unsigned int iCode, _EXCEPTION_POINTERS* pException )
{
    m_uiCode = iCode;
    m_pOffset = pException->ExceptionRecord->ExceptionAddress;
    m_pReferencingOffset = reinterpret_cast < void* > ( pException->ContextRecord->Eax );
    m_ulEAX = pException->ContextRecord->Eax;
    m_ulEBX = pException->ContextRecord->Ebx;
    m_ulECX = pException->ContextRecord->Ecx;
    m_ulEDX = pException->ContextRecord->Edx;
    m_ulESI = pException->ContextRecord->Esi;
    m_ulEDI = pException->ContextRecord->Edi;
    m_ulEBP = pException->ContextRecord->Ebp;
    m_ulESP = pException->ContextRecord->Esp;
    m_ulEIP = pException->ContextRecord->Eip;
    m_ulCS = pException->ContextRecord->SegCs;
    m_ulDS = pException->ContextRecord->SegDs;
    m_ulES = pException->ContextRecord->SegEs;
    m_ulFS = pException->ContextRecord->SegFs;
    m_ulGS = pException->ContextRecord->SegGs;
    m_ulSS = pException->ContextRecord->SegSs;
    m_ulEFlags = pException->ContextRecord->EFlags;
}

/**
 * Returns the name of the module in which the exception occured, or
 * an emtpy string if the module could not be found.
 *
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 */
bool CExceptionInformation_Impl::GetModule ( char * szOutputBuffer, int nOutputNameLength )
{
   HMODULE hModule;

   if (szOutputBuffer == NULL)
      return false;

   /*
    * NUL out the first char in the output buffer.
    */
   szOutputBuffer[0] = 0;

   if ( nOutputNameLength == 0 )
      return false;

   /*
    * GetModuleHandleExA isn't supported under Windows 2000.
    */
   typedef BOOL (WINAPI * _pfnGetModuleHandleExA) ( DWORD, LPCSTR, HMODULE * );

   /*
    * Get kernel32.dll's HMODULE.
    */
   HMODULE hKern32 = GetModuleHandle("kernel32.dll");
   if ( NULL == hKern32 )  
      return false;

   /*
    * See if we're able to use GetModuleHandleExA.  According to Microsoft,
    * this API is only available on Windows XP and Vista.
    */
   _pfnGetModuleHandleExA pfnGetModuleHandleExA = 
      (_pfnGetModuleHandleExA)GetProcAddress(hKern32, "GetModuleHandleExA");

   /*
    * TODO:  Possibly use our own code to do this for other systems.
    * It is possible to enumerate all modules and get their starting/ending
    * offsets, so it would just be a simple comparison of addresses to
    * do this...
    */
   if (NULL == pfnGetModuleHandleExA)
      return false;
   
   if ( 0 == pfnGetModuleHandleExA ( 
               0x00000004 /*GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS*/, 
               (LPCSTR)m_pOffset,
               &hModule ) )
   {
      return false;
   }
   
   if ( 0 != GetModuleFileNameA ( 
                hModule, 
                szOutputBuffer,
                nOutputNameLength ) )
   {
      /*
       * GetModuleFileNameA will return nOutputNameLength to us
       * if the buffer is too small.  NUL term the buffer.
       *
       * TODO:  Check GetLastError() and actually grow the buffer
       * and retry if it is too small.
       */ 
      if (nOutputNameLength)
         szOutputBuffer[nOutputNameLength-1] = 0;

      return true;
   }

   return false;
}
