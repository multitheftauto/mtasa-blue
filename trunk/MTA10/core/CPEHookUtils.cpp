/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CPEHookUtils.cpp
*  PURPOSE:     PE executable hooking utilities
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

DWORD CPEHookUtils::GetIATAddress   ( const string& ModuleName,
                                      const string& FunctionName, 
                                      HANDLE hImgBase )
{
    PIMAGE_NT_HEADERS image;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    PIMAGE_THUNK_DATA pThunkData;

    image = xImageNTHeader( hImgBase );

    if (!image) return 0;

    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((image->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) + (DWORD)hImgBase);

    for (int i=0; ;i++) 
    {
        if (pImportDesc->Name == NULL) break;

        if (!strcmpi((char*)((DWORD)pImportDesc->Name + (DWORD)hImgBase),ModuleName.c_str())) 
        {
            if ((pImportDesc->OriginalFirstThunk) != 0) {
                for (int c=0; ; c++) 
                {
                    pThunkData = (PIMAGE_THUNK_DATA)((DWORD)pImportDesc->OriginalFirstThunk + c*sizeof(IMAGE_THUNK_DATA) + (DWORD)hImgBase);
                    
                    if (pThunkData->u1.AddressOfData == NULL) break;

                    if (!strcmpi((char*)(((PIMAGE_IMPORT_BY_NAME)((DWORD)pThunkData->u1.AddressOfData + (DWORD)hImgBase))->Name),FunctionName.c_str())) 
                    {
                        return (DWORD)(((PIMAGE_THUNK_DATA)((DWORD)pImportDesc->FirstThunk + c*sizeof(IMAGE_THUNK_DATA) + (DWORD)hImgBase))->u1.Function);
                    }
                }
            }
        }
        pImportDesc++;
    }
    return 0;
}

DWORD CPEHookUtils::WriteIATAddress ( const string& ModuleName,
                                      const string& FunctionName,
                                      HANDLE hImgBase,
                                      PVOID  pvNewProc )
{
    PIMAGE_NT_HEADERS image;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    PIMAGE_THUNK_DATA pThunkData;
    DWORD oldvalue;

    image = xImageNTHeader(hImgBase);

    if (!image) return 0;

    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((image->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) + (DWORD)hImgBase);

    for (int i=0; ;i++) 
    {
        if (pImportDesc->Name == NULL) break;

        if (!strcmpi((char*)((DWORD)pImportDesc->Name + (DWORD)hImgBase),ModuleName.c_str())) 
        {
            if ((pImportDesc->OriginalFirstThunk) != 0) 
            {
                for (int c=0; ; c++) 
                {
                    pThunkData = (PIMAGE_THUNK_DATA)((DWORD)pImportDesc->OriginalFirstThunk + c*sizeof(IMAGE_THUNK_DATA) + (DWORD)hImgBase);

                    if (pThunkData->u1.AddressOfData == NULL) break;

                    if (!strcmpi((char*)(((PIMAGE_IMPORT_BY_NAME)((DWORD)pThunkData->u1.AddressOfData + (DWORD)hImgBase))->Name),FunctionName.c_str())) 
                    {
                         oldvalue = (DWORD)(((PIMAGE_THUNK_DATA)((DWORD)pImportDesc->FirstThunk + c*sizeof(IMAGE_THUNK_DATA) + (DWORD)hImgBase))->u1.Function);
                        ((PIMAGE_THUNK_DATA)((DWORD)pImportDesc->FirstThunk + c*sizeof(IMAGE_THUNK_DATA) + (DWORD)hImgBase))->u1.Function = (DWORD)pvNewProc;
                        return oldvalue;
                    }
                }
            }
        }
        pImportDesc++;
    }
    return 0;
}

PVOID CPEHookUtils::HookVTableFunc  ( PVOID  pvInterface,
                                      int    nOffset,
                                      PVOID  pvHookFunc,
                                      int    nVtableSlot )
{
    // first, get the vtable ptr
    void* vPtr = reinterpret_cast<void*>(static_cast<char**>(pvInterface)[nOffset]);

    // now, get the address of the vtable slot
    void** pVtableSlot = reinterpret_cast<void**>(&static_cast<DWORD*>(vPtr)[nVtableSlot]);

    // now copy the existing function pointer
    void* oldFunction  = *pVtableSlot;

    DWORD oldProtect;
    VirtualProtect(pVtableSlot, 4, PAGE_READWRITE, &oldProtect);

    // and set the new one
    *pVtableSlot = pvHookFunc;

    VirtualProtect(pVtableSlot, 4, oldProtect, &oldProtect);

    // and return the old one
    return oldFunction;
}

VOID  CPEHookUtils::GetSectionHeader( const string& SectName,
                                      const HANDLE hImgBase,
                                      PSECTIONINFO pSectionInfo )
{
    PIMAGE_NT_HEADERS image;
    PIMAGE_SECTION_HEADER section;
    DWORD hProcBase;

    hProcBase = (DWORD)hImgBase;

    image = xImageNTHeader(hImgBase);

    if (!image) return;

    hProcBase = (DWORD)image + sizeof(IMAGE_NT_HEADERS);
    section = (PIMAGE_SECTION_HEADER)hProcBase;

    for (int i=0; i<image->FileHeader.NumberOfSections; i++) 
    {
        if (strstr((char*)section->Name,SectName.c_str())) 
        {
            pSectionInfo->Address = (DWORD)hImgBase + section->VirtualAddress;
            pSectionInfo->Size = section->Misc.VirtualSize;
            return;
        }
        section++;
    }
    return;
}

PIMAGE_NT_HEADERS CPEHookUtils::xImageNTHeader  ( const HANDLE hImgBase )
{
    PIMAGE_DOS_HEADER pDosHeader;

    pDosHeader = (PIMAGE_DOS_HEADER)hImgBase;

    if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE) 
    {
        PIMAGE_NT_HEADERS pImage;

        pImage = reinterpret_cast< PIMAGE_NT_HEADERS > ( pDosHeader->e_lfanew + reinterpret_cast< DWORD >( hImgBase ) );
        if (pImage->Signature == IMAGE_NT_SIGNATURE)
        {
            return (PIMAGE_NT_HEADERS)(pDosHeader->e_lfanew + (DWORD)hImgBase);
        }
    }
    return 0;
}
