#include "StdInc.h"
/*
#include "FileIDLimits.h"

static void* ptrCGenericGameStorage__SaveDataToWorkBuffer = (void* )0x5D0F80;

// Writes to save file
int CGenericGameStorage_SaveDataToWorkBuffer(const void* pSource, int Size)
{
    return ((int(*) (const void* pSource, int Size))ptrCGenericGameStorage__SaveDataToWorkBuffer)(pSource, Size);
}

static void* ptrCGenericGameStorage__LoadDataFromWorkBuffer = (void* )0x5D1300;

// Reads to save file
bool CGenericGameStorage_LoadDataFromWorkBuffer(void* ptr, unsigned int numOfBytes)
{
    return ((bool(*)(void* ptr, unsigned int numOfBytes))ptrCGenericGameStorage__LoadDataFromWorkBuffer)(ptr, numOfBytes);
}


// GTA San Andreas
namespace Game_GTASA
{
    // 32-bit indices
    namespace ModelInfoExtension
    {
        // Returns pointer to extended value
        void* realGetPointerToExtendedValue(DWORD InfoForModelOffset, DWORD memberOffset)
        {
#if TRUE
            DWORD fileID = InfoForModelOffset / sizeof(CStreamingInfo);

            CStreamingInfoExtension* pExtension = &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[fileID];

            switch (memberOffset)
            {
                case 0:
                    return &pExtension->exUsNext;
                case 2:
                    return &pExtension->exUsPrev;
                case 4:
                    return &pExtension->exNextOnCd;
            }
#else
            CStreamingInfo* pFileInfo = (CStreamingInfo*)((char*)(g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa) + InfoForModelOffset);

            switch (memberOffset)
            {
                case 0:
                    return &pFileInfo->usNext;
                case 2:
                    return &pFileInfo->usPrev;
                case 4:
                    return &pFileInfo->usNextOnCd;
            }
#endif

            return NULL;
        }

        int32_t __cdecl realGetExtendedMemberByStreamingInfoMemOffset(DWORD InfoForModelOffset, DWORD memberOffset)
        {
#if TRUE
            return *(int32_t*)realGetPointerToExtendedValue(InfoForModelOffset, memberOffset);
#else

            // DWORD result =  *((WORD*)((char*)FileIDlimit::CStreaming__ms_aInfoForModel + InfoForModelOffset + memberOffset));

            DWORD result = *(WORD*)realGetPointerToExtendedValue(InfoForModelOffset, memberOffset);

            if (result == 0xFFFF)
                result = UNDEFINED_FILE_ID_INT32;

            return result;
#endif
        }

        void __cdecl realSetExtendedMemberByStreamingInfoMemOffset(DWORD InfoForModelOffset, DWORD memberOffset, DWORD newValue)
        {
#if TRUE
            *(int32_t*)realGetPointerToExtendedValue(InfoForModelOffset, memberOffset) = newValue;
#else
            // *((WORD*)((char*)FileIDlimit::CStreaming__ms_aInfoForModel + InfoForModelOffset + memberOffset)) = (WORD)newValue;
            *(WORD*)realGetPointerToExtendedValue(InfoForModelOffset, memberOffset) = newValue;
#endif
        }

        NAKED void GetExtendedMemberByStreamingInfoPtr(CStreamingInfo* pFileInfo, DWORD memberOffset, DWORD returnValue)
        {
            // stack:
            // 0x0: return address
            // 0x4: CStreamingInfo* pFileInfo
            // 0x8: DWORD memberOffset
            // 0xC: DWORD returnValue

            __asm
            {
				pushfd;
				pushad;
				
				mov eax, [esp + 0x24 + 0x8];
				push eax;            // memberOffset

				mov eax, [esp + 0x28 + 0x4];            // pFileInfo
				sub eax, ASM_ACCESS_LIMIT_VAR_32_BIT(m_fileIDlimits, FileIDlimit, CStreaming__ms_aInfoForModel);
				push eax;            // InfoForModelOffset

				call realGetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0x8;

				mov[esp + 0x24 + 0xC], eax;

				popad;
				popfd;
				retn;
            }
        }

        NAKED void GetExtendedMemberByStreamingInfoMemOffset(DWORD InfoForModelOffset, DWORD memberOffset, DWORD returnValue)
        {
            // stack:
            // 0x0: return address
            // 0x4: DWORD InfoForModelOffset
            // 0x8: DWORD memberOffset
            // 0xC: DWORD returnValue

            __asm
            {
				pushfd;
				pushad;

				mov eax, [esp + 0x24 + 0x8];
				push eax;            // memberOffset

				mov eax, [esp + 0x28 + 0x4];            // pFileInfo
				push eax;            // InfoForModelOffset

				call realGetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0x8;

				mov[esp + 0x24 + 0xC], eax;

				popad;
				popfd;
				retn;
            }
        }

        NAKED void SetExtendedMemberByStreamingInfoPtr(CStreamingInfo* pFileInfo, DWORD memberOffset, DWORD newValue)
        {
            // stack:
            // 0x0: return address
            // 0x4: CStreamingInfo* pFileInfo
            // 0x8: DWORD memberOffset
            // 0xC: DWORD newValue

            __asm
            {
				pushfd;
				pushad;

				mov eax, [esp + 0x24 + 0xC];            // newValue
				push eax;

				mov eax, [esp + 0x28 + 0x8];
				push eax;            // memberOffset

				mov eax, [esp + 0x2C + 0x4];            // pFileInfo
				sub eax, ASM_ACCESS_LIMIT_VAR_32_BIT(m_fileIDlimits, FileIDlimit, CStreaming__ms_aInfoForModel);
				push eax;            // InfoForModelOffset

				call realSetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				popad;
				popfd;
				retn;
            }
        }

        NAKED void SetExtendedMemberByStreamingInfoMemOffset(DWORD InfoForModelOffset, DWORD memberOffset, DWORD returnValue)
        {
            // stack:
            // 0x0: return address
            // 0x4: DWORD InfoForModelOffset
            // 0x8: DWORD memberOffset
            // 0xC: DWORD newValue

            __asm
            {
				pushfd;
				pushad;

				mov eax, [esp + 0x24 + 0xC];            // newValue
				push eax;

				mov eax, [esp + 0x28 + 0x8];
				push eax;            // memberOffset

				mov eax, [esp + 0x2C + 0x4];
				push eax;            // InfoForModelOffset

				call realSetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				popad;
				popfd;
				retn;
            }
        }
    }            // namespace ModelInfoExtension

    // 32-bit indices
    namespace ModelInfoExtension
    {
        // Original code:	mov ax, [ecx]
        NAKED void patch_407520()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_407523()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [ecx+2]
        NAKED void patch_407540()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_407544()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_407562()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [eax]
        NAKED void patch_407AD5()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_407AD8()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov si, [esi]
        NAKED void patch_407B35()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov esi, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp si, 0FFFFh
        NAKED void patch_407B38()
        {
            __asm
            {
				cmp esi, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [esi], 0FFFFh
        NAKED void patch_40884E()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [esi]
        NAKED void patch_40885E()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov cx, [esi+2]
        NAKED void patch_408861()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov ecx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+eax*4+2], cx
        NAKED void patch_40886E()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + eax * 4];

				push ecx;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [esi+2]
        NAKED void patch_408873()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov cx, [esi]
        NAKED void patch_408877()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov ecx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+eax*4], cx
        NAKED void patch_408883()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + eax * 4];

				push ecx;            // new value
				push 0;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	mov word ptr [esi], 0FFFFh
        NAKED void patch_408887()
        {
            __asm
            {
				push UNDEFINED_FILE_ID_INT32;            // new value
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov word ptr [esi+2], 0FFFFh
        NAKED void patch_40888C()
        {
            __asm
            {
				push UNDEFINED_FILE_ID_INT32;            // new value
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	cmp word ptr ds:ms_aInfoForModel.usNext[edi], bp
        NAKED void patch_408ADA()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], ebp;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr ds:ms_aInfoForModel.usNext[edi]
        NAKED void patch_408B22()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov ax, ds:ms_aInfoForModel.usPrev[edi]
        NAKED void patch_408B32()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx+edx*4+2], ax
        NAKED void patch_408B39()
        {
            __asm
            {
				push ebx;
				lea ebx, [ecx + edx * 4];

				push eax;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	movsx eax, ds:ms_aInfoForModel.usPrev[edi]
        NAKED void patch_408B3E()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov ax, word ptr ds:ms_aInfoForModel.usNext[edi]
        NAKED void patch_408B4E()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx+edx*4], ax
        NAKED void patch_408B55()
        {
            __asm
            {
				push ebx;
				lea ebx, [ecx + edx * 4];

				push eax;            // new value
				push 0;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	mov word ptr ds:ms_aInfoForModel.usNext[edi], bp
        NAKED void patch_408B59()
        {
            __asm
            {
				push ebp;            // new value
				push 0;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call SetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov ds:ms_aInfoForModel.usPrev[edi], bp
        NAKED void patch_408B60()
        {
            __asm
            {
				push ebp;            // new value
				push 2;            // member offset
				push edi;            // CStreaming::ms_aInfoForModel offset
				call SetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov ax, [eax+CStreamingInfo.usNext]
        NAKED void patch_408E28()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, cx
        NAKED void patch_408E2E()
        {
            __asm
            {
				cmp eax, ecx;
				retn;
            }
        }

        // Original code:	mov ax, [esi]
        NAKED void patch_408E72()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_408E7F()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr ds:ms_aInfoForModel.usNext[eax], 0FFFFh
        NAKED void patch_4098C8()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp ds:ms_aInfoForModel.usNext[ecx*4], 0FFFFh
        NAKED void patch_409978()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreaming::ms_aInfoForModel offset
				shl [esp], 2;
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp ds:ms_aInfoForModel.usNext[ecx*4], 0FFFFh
        NAKED void patch_409A39()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreaming::ms_aInfoForModel offset
				shl [esp], 2;
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_409C41()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_409CCC()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ds:ms_aInfoForModel.usNextOnCd[eax], 0FFFFh
        NAKED void patch_409FCA()
        {
            __asm
            {
				push UNDEFINED_FILE_ID_INT32;            // new value
				push 4;            // member offset
				push eax;            // CStreaming::ms_aInfoForModel offset
				call SetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40A862()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_40A88B()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_40A88E()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_40A8AA()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_40A8AE()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_40A8B1()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_40A8C1()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40A926()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_40A953()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_40A956()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_40A972()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_40A976()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_40A979()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_40A989()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40ABEB()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [esi], 0FFFFh
        NAKED void patch_40AC59()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40AE1F()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_40AE48()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_40AE4B()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_40AE63()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_40AE67()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_40AE6A()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_40AE76()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40AEC8()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_40AEF1()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_40AEF4()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_40AF0C()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_40AF10()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_40AF13()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_40AF1F()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40B952()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40BAFA()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [esi], 0FFFFh
        NAKED void patch_40BB5E()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_40BEB9()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [esi], 0FFFFh
        NAKED void patch_40BF19()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [eax+2]
        NAKED void patch_40C1E5()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40C1E9()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [esi+2]
        NAKED void patch_40C229()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40C22D()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	movsx esi, ds:ms_aInfoForModel.usNextOnCd[ebx]
        NAKED void patch_40CEDB()
        {
            __asm
            {
				push 0;            // space for result
				push 4;            // member offset
				push ebx;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				mov esi, [esp - 4];
				retn;
            }
        }

        // Original code:	mov ax, [eax+2]
        NAKED void patch_40CFD5()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40CFD9()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov di, [edi+2]
        NAKED void patch_40D030()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edi, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp di, 0FFFFh
        NAKED void patch_40D034()
        {
            __asm
            {
				cmp edi, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov di, [edi+2]
        NAKED void patch_40D11B()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edi, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp di, 0FFFFh
        NAKED void patch_40D11F()
        {
            __asm
            {
				cmp edi, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [eax+2]
        NAKED void patch_40D2F5()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40D2F9()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [esi+2]
        NAKED void patch_40D349()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40D34D()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [eax]
        NAKED void patch_40E4E5()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40E4E8()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [ecx]
        NAKED void patch_40E510()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40E513()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [eax+2]
        NAKED void patch_40EA46()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40EA4A()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [ecx+2]
        NAKED void patch_40EBD8()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40EBDC()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [esi+2]
        NAKED void patch_40EC0F()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_40EC1D()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_1562A12()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [esi], 0FFFFh
        NAKED void patch_1562A72()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_1562B15()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_1562B3E()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_1562B41()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_1562B59()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_1562B5D()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_1562B60()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_1562B70()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	cmp word ptr [ecx], 0FFFFh
        NAKED void patch_1562BD1()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				cmp dword ptr[esp - 4], UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov dx, [edi]
        NAKED void patch_1562BFA()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_1562BFD()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_1562C15()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_1562C19()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_1562C1C()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_1562C2C()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	mov ax, [eax]
        NAKED void patch_15664B5()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp ax, 0FFFFh
        NAKED void patch_15664B8()
        {
            __asm
            {
				cmp eax, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov si, [esi]
        NAKED void patch_1566552()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push esi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov esi, [esp - 4];
				retn;
            }
        }

        // Original code:	cmp si, 0FFFFh
        NAKED void patch_1566555()
        {
            __asm
            {
				cmp esi, UNDEFINED_FILE_ID_INT32;
				retn;
            }
        }

        // Original code:	mov ax, [edi]
        NAKED void patch_15674F9()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [ecx], ax
        NAKED void patch_15674FC()
        {
            __asm
            {
				push eax;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_15674FF()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [edi], si
        NAKED void patch_1567503()
        {
            __asm
            {
				push esi;            // new value
				push 0;            // member offset
				push edi;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_1567506()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [edx+ecx*4+2], si
        NAKED void patch_1567513()
        {
            __asm
            {
				push ebx;
				lea ebx, [edx + ecx * 4];

				push esi;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	mov [ecx], dx
        NAKED void patch_1567528()
        {
            __asm
            {
				push edx;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+2], dx
        NAKED void patch_156752B()
        {
            __asm
            {
				push edx;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov [ecx+4], dx
        NAKED void patch_156752F()
        {
            __asm
            {
				push edx;            // new value
				push 4;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	cmp word ptr ds:ms_aInfoForModel.usNext[eax], 0
        NAKED void patch_1567B73()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push eax;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], 0;
				retn;
            }
        }

        // Original code:	cmp ds:ms_aInfoForModel.usPrev[eax], 0
        NAKED void patch_1567B7D()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push eax;            // CStreaming::ms_aInfoForModel offset
				call GetExtendedMemberByStreamingInfoMemOffset;
				add esp, 0xC;

				cmp dword ptr[esp - 4], 0;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx]
        NAKED void patch_156CE90()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov dx, [ecx+2]
        NAKED void patch_156CE93()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [esi+eax*4+2], dx
        NAKED void patch_156CEA1()
        {
            __asm
            {
				push ebx;
				lea ebx, [esi + eax * 4];

				push edx;            // new value
				push 2;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	movsx eax, word ptr [ecx+2]
        NAKED void patch_156CEA6()
        {
            __asm
            {
				push 0;            // space for result
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov eax, [esp - 4];
				retn;
            }
        }

        // Original code:	mov dx, [ecx]
        NAKED void patch_156CEB0()
        {
            __asm
            {
				push 0;            // space for result
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call GetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				mov edx, [esp - 4];
				retn;
            }
        }

        // Original code:	mov [esi+eax*4], dx
        NAKED void patch_156CEB6()
        {
            __asm
            {
				push ebx;
				lea ebx, [esi + eax * 4];

				push edx;            // new value
				push 0;            // member offset
				push ebx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;

				pop ebx;
				retn;
            }
        }

        // Original code:	mov word ptr [ecx], 0FFFFh
        NAKED void patch_156CEBA()
        {
            __asm
            {
				push UNDEFINED_FILE_ID_INT32;            // new value
				push 0;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        // Original code:	mov word ptr [ecx+2], 0FFFFh
        NAKED void patch_156CEBF()
        {
            __asm
            {
				push UNDEFINED_FILE_ID_INT32;            // new value
				push 2;            // member offset
				push ecx;            // CStreamingInfo*
				call SetExtendedMemberByStreamingInfoPtr;
				add esp, 0xC;
				retn;
            }
        }

        static const tArrayItem ptrArray[] = {
            tArrayItem(0x407520, &patch_407520),   tArrayItem(0x407523, &patch_407523),   tArrayItem(0x407540, &patch_407540),
            tArrayItem(0x407544, &patch_407544),   tArrayItem(0x407562, &patch_407562),   tArrayItem(0x407AD5, &patch_407AD5),
            tArrayItem(0x407AD8, &patch_407AD8),   tArrayItem(0x407B35, &patch_407B35),   tArrayItem(0x407B38, &patch_407B38),
            tArrayItem(0x40884E, &patch_40884E),   tArrayItem(0x40885E, &patch_40885E),   tArrayItem(0x408861, &patch_408861),
            tArrayItem(0x40886E, &patch_40886E),   tArrayItem(0x408873, &patch_408873),   tArrayItem(0x408877, &patch_408877),
            tArrayItem(0x408883, &patch_408883),   tArrayItem(0x408887, &patch_408887),   tArrayItem(0x40888C, &patch_40888C),
            tArrayItem(0x408ADA, &patch_408ADA),   tArrayItem(0x408B22, &patch_408B22),   tArrayItem(0x408B32, &patch_408B32),
            tArrayItem(0x408B39, &patch_408B39),   tArrayItem(0x408B3E, &patch_408B3E),   tArrayItem(0x408B4E, &patch_408B4E),
            tArrayItem(0x408B55, &patch_408B55),   tArrayItem(0x408B59, &patch_408B59),   tArrayItem(0x408B60, &patch_408B60),
            tArrayItem(0x408E28, &patch_408E28),   tArrayItem(0x408E2E, &patch_408E2E),   tArrayItem(0x408E72, &patch_408E72),
            tArrayItem(0x408E7F, &patch_408E7F),   tArrayItem(0x4098C8, &patch_4098C8),   tArrayItem(0x409978, &patch_409978),
            tArrayItem(0x409A39, &patch_409A39),   tArrayItem(0x409C41, &patch_409C41),   tArrayItem(0x409CCC, &patch_409CCC),
            tArrayItem(0x409FCA, &patch_409FCA),   tArrayItem(0x40A862, &patch_40A862),   tArrayItem(0x40A88B, &patch_40A88B),
            tArrayItem(0x40A88E, &patch_40A88E),   tArrayItem(0x40A8AA, &patch_40A8AA),   tArrayItem(0x40A8AE, &patch_40A8AE),
            tArrayItem(0x40A8B1, &patch_40A8B1),   tArrayItem(0x40A8C1, &patch_40A8C1),   tArrayItem(0x40A926, &patch_40A926),
            tArrayItem(0x40A953, &patch_40A953),   tArrayItem(0x40A956, &patch_40A956),   tArrayItem(0x40A972, &patch_40A972),
            tArrayItem(0x40A976, &patch_40A976),   tArrayItem(0x40A979, &patch_40A979),   tArrayItem(0x40A989, &patch_40A989),
            tArrayItem(0x40ABEB, &patch_40ABEB),   tArrayItem(0x40AC59, &patch_40AC59),   tArrayItem(0x40AE1F, &patch_40AE1F),
            tArrayItem(0x40AE48, &patch_40AE48),   tArrayItem(0x40AE4B, &patch_40AE4B),   tArrayItem(0x40AE63, &patch_40AE63),
            tArrayItem(0x40AE67, &patch_40AE67),   tArrayItem(0x40AE6A, &patch_40AE6A),   tArrayItem(0x40AE76, &patch_40AE76),
            tArrayItem(0x40AEC8, &patch_40AEC8),   tArrayItem(0x40AEF1, &patch_40AEF1),   tArrayItem(0x40AEF4, &patch_40AEF4),
            tArrayItem(0x40AF0C, &patch_40AF0C),   tArrayItem(0x40AF10, &patch_40AF10),   tArrayItem(0x40AF13, &patch_40AF13),
            tArrayItem(0x40AF1F, &patch_40AF1F),   tArrayItem(0x40B952, &patch_40B952),   tArrayItem(0x40BAFA, &patch_40BAFA),
            tArrayItem(0x40BB5E, &patch_40BB5E),   tArrayItem(0x40BEB9, &patch_40BEB9),   tArrayItem(0x40BF19, &patch_40BF19),
            tArrayItem(0x40C1E5, &patch_40C1E5),   tArrayItem(0x40C1E9, &patch_40C1E9),   tArrayItem(0x40C229, &patch_40C229),
            tArrayItem(0x40C22D, &patch_40C22D),   tArrayItem(0x40CEDB, &patch_40CEDB),   tArrayItem(0x40CFD5, &patch_40CFD5),
            tArrayItem(0x40CFD9, &patch_40CFD9),   tArrayItem(0x40D030, &patch_40D030),   tArrayItem(0x40D034, &patch_40D034),
            tArrayItem(0x40D11B, &patch_40D11B),   tArrayItem(0x40D11F, &patch_40D11F),   tArrayItem(0x40D2F5, &patch_40D2F5),
            tArrayItem(0x40D2F9, &patch_40D2F9),   tArrayItem(0x40D349, &patch_40D349),   tArrayItem(0x40D34D, &patch_40D34D),
            tArrayItem(0x40E4E5, &patch_40E4E5),   tArrayItem(0x40E4E8, &patch_40E4E8),   tArrayItem(0x40E510, &patch_40E510),
            tArrayItem(0x40E513, &patch_40E513),   tArrayItem(0x40EA46, &patch_40EA46),   tArrayItem(0x40EA4A, &patch_40EA4A),
            tArrayItem(0x40EBD8, &patch_40EBD8),   tArrayItem(0x40EBDC, &patch_40EBDC),   tArrayItem(0x40EC0F, &patch_40EC0F),
            tArrayItem(0x40EC1D, &patch_40EC1D),   tArrayItem(0x1562A12, &patch_1562A12), tArrayItem(0x1562A72, &patch_1562A72),
            tArrayItem(0x1562B15, &patch_1562B15), tArrayItem(0x1562B3E, &patch_1562B3E), tArrayItem(0x1562B41, &patch_1562B41),
            tArrayItem(0x1562B59, &patch_1562B59), tArrayItem(0x1562B5D, &patch_1562B5D), tArrayItem(0x1562B60, &patch_1562B60),
            tArrayItem(0x1562B70, &patch_1562B70), tArrayItem(0x1562BD1, &patch_1562BD1), tArrayItem(0x1562BFA, &patch_1562BFA),
            tArrayItem(0x1562BFD, &patch_1562BFD), tArrayItem(0x1562C15, &patch_1562C15), tArrayItem(0x1562C19, &patch_1562C19),
            tArrayItem(0x1562C1C, &patch_1562C1C), tArrayItem(0x1562C2C, &patch_1562C2C), tArrayItem(0x15664B5, &patch_15664B5),
            tArrayItem(0x15664B8, &patch_15664B8), tArrayItem(0x1566552, &patch_1566552), tArrayItem(0x1566555, &patch_1566555),
            tArrayItem(0x15674F9, &patch_15674F9), tArrayItem(0x15674FC, &patch_15674FC), tArrayItem(0x15674FF, &patch_15674FF),
            tArrayItem(0x1567503, &patch_1567503), tArrayItem(0x1567506, &patch_1567506), tArrayItem(0x1567513, &patch_1567513),
            tArrayItem(0x1567528, &patch_1567528), tArrayItem(0x156752B, &patch_156752B), tArrayItem(0x156752F, &patch_156752F),
            tArrayItem(0x1567B73, &patch_1567B73), tArrayItem(0x1567B7D, &patch_1567B7D), tArrayItem(0x156CE90, &patch_156CE90),
            tArrayItem(0x156CE93, &patch_156CE93), tArrayItem(0x156CEA1, &patch_156CEA1), tArrayItem(0x156CEA6, &patch_156CEA6),
            tArrayItem(0x156CEB0, &patch_156CEB0), tArrayItem(0x156CEB6, &patch_156CEB6), tArrayItem(0x156CEBA, &patch_156CEBA),
            tArrayItem(0x156CEBF, &patch_156CEBF),
        };
    }            // namespace ModelInfoExtension

    // Count of killable model IDs
    unsigned int CDarkel::CountOfKillableModelIDs = 800;

    // Registered kills
    int16_t* CDarkel::RegisteredKills = (int16_t*)0x969A50;

    // Finds total number of peds killed by player.
    int __cdecl CDarkel::FindTotalPedsKilledByPlayer(int playerID)
    {
        int16_t* pRegisteredKillsForThisPlayer = CDarkel::RegisteredKills + playerID;
        int16_t  sum = 0;

        for (unsigned int i = 0; i < CDarkel::CountOfKillableModelIDs; i++)
            sum += *pRegisteredKillsForThisPlayer;

        return sum;
    }

    // Returns position and size of file
    bool CStreamingInfo::GetCdPosnAndSize(unsigned int& outBlockOffsetWithStreamHandle, unsigned int& outBlockCount)
    {
        return ((bool(__thiscall*)(CStreamingInfo*, unsigned int&, unsigned int&))0x4075A0)(this, outBlockOffsetWithStreamHandle, outBlockCount);
    }

    // Sets position and size of file
    void CStreamingInfo::SetCdPosnAndSize(unsigned int BlockOffset, unsigned int BlockCount)
    {
        return ((void(__thiscall*)(CStreamingInfo*, unsigned int, unsigned int))0x4075E0)(this, BlockOffset, BlockCount);
    }

    // readIPLsFromSave patch for ID limit
    NAKED void patch_readIPLsFromSave_5D5514()
    {
        __asm {
			mov eax, [esp + 10h];            // numberOfIplFiles

			cmp ebx, eax;

            // go to loop
			jae returnFromFunc;

			mov eax, 0x5D54C0;
			jmp eax;

		returnFromFunc:
			pop edi;
			pop esi;
			mov al, 1;
			pop ebx;
			add esp, 8;
			retn;
        }
    }

    static const char ENHANCED_FLAG_IDENTIFIER[] = "fastman92_modelFlags";
    static const int  ENHANCED_FLAG_IDENTIFIER_LEN = sizeof(ENHANCED_FLAG_IDENTIFIER) - 1;

    static const eFileType arrayOfFileTypes[] = {
        FILE_TYPE_DFF, FILE_TYPE_TXD,

        FILE_TYPE_COL, FILE_TYPE_IPL, FILE_TYPE_DAT, FILE_TYPE_IFP, FILE_TYPE_RRR, FILE_TYPE_SCM, FILE_TYPE_LOADED_START, FILE_TYPE_REQUESTED_START,
    };

    // Patch for readModelFlagsFromSave
    bool __cdecl readModelFlagsFromSave_patch()
    {
        char buffer[ENHANCED_FLAG_IDENTIFIER_LEN + 1];

        CGenericGameStorage_LoadDataFromWorkBuffer(buffer, ENHANCED_FLAG_IDENTIFIER_LEN);

        if (strncmp(buffer, ENHANCED_FLAG_IDENTIFIER, ENHANCED_FLAG_IDENTIFIER_LEN))            // is standard version?
        {
            WORD defaultCount = g_LimitAdjuster.m_fileIDlimits.GetDefaultCountOfAllFileIDs();

            BYTE* pFlags = new BYTE[defaultCount];

            memcpy(pFlags, buffer, ENHANCED_FLAG_IDENTIFIER_LEN);

            // read the rest of data
            CGenericGameStorage_LoadDataFromWorkBuffer(pFlags + ENHANCED_FLAG_IDENTIFIER_LEN, defaultCount - ENHANCED_FLAG_IDENTIFIER_LEN);

            for (eFileType type : arrayOfFileTypes)
            {
                WORD numberOfFiles = g_LimitAdjuster.m_fileIDlimits.GetFileIDdefaultLimit(type);

                if (g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type) < numberOfFiles)
                    numberOfFiles = g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type);

                BYTE*           iFlags = pFlags + g_LimitAdjuster.m_fileIDlimits.GetDefaultBaseID(type);
                CStreamingInfo* oInfoForModel =
                    &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[g_LimitAdjuster.m_fileIDlimits.GetBaseID(type)];

                for (WORD i = 0; i < numberOfFiles; i++)
                {
                    CStreamingInfo* file = oInfoForModel + i;

                    if (file->uiLoadFlag == 1 && iFlags[i] != 0xFF)
                        file->ucFlags |= iFlags[i];
                }

               // 
               // printf_MessageBox("File type: %d Number of source IDs: %d Input ID: %d Output ID: %d",
               // type,
                //numberOfFiles,
                //CLimitAdjusterMain::CStreaming__defaultBaseID[type],
                //CLimitAdjusterMain::GetBaseID(type)
                //);
                //
            }

            delete[] pFlags;
        }
        else
        {
            char format;

            // enhanced version
            CGenericGameStorage_LoadDataFromWorkBuffer(&format, 1);

            if (format == 1)
            {
                DWORD numberOfBytesFollowing;
                CGenericGameStorage_LoadDataFromWorkBuffer(&numberOfBytesFollowing, sizeof(numberOfBytesFollowing));

                for (eFileType type : arrayOfFileTypes)
                {
                    WORD numberOfFilesToProcess;
                    WORD numberOfFilesToSkip = 0;

                    CGenericGameStorage_LoadDataFromWorkBuffer(&numberOfFilesToProcess, sizeof(WORD));

                    WORD numberOfFilesInGame = g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type);

                    CStreamingInfo* oInfoForModel =
                        &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[g_LimitAdjuster.m_fileIDlimits.GetBaseID(type)];

                    if (numberOfFilesInGame < numberOfFilesToProcess)
                    {
                        numberOfFilesToSkip = numberOfFilesToProcess - numberOfFilesInGame;
                        numberOfFilesToProcess = numberOfFilesInGame;
                    }

                  //
                   // printf_MessageBox("File type: %d\nNumber of files to process: %d\nNumber of files to skip: %d\nOutput ID: %d",
                   // type,
                   // numberOfFilesToProcess,
                   // numberOfFilesToSkip,
                    //CLimitAdjusterMain::GetBaseID(type)
                   // );
                    //

                    BYTE flags;

                    // Process file flags
                    for (WORD i = 0; i < numberOfFilesToProcess; i++)
                    {
                        CGenericGameStorage_LoadDataFromWorkBuffer(&flags, 1);

                        CStreamingInfo* file = oInfoForModel + i;

                        if (file->uiLoadFlag == 1 && flags != 0xFF)
                            file->ucFlags |= flags;
                    }

                    // Skip file flags
                    while (numberOfFilesToSkip-- > 0)
                        CGenericGameStorage_LoadDataFromWorkBuffer(&flags, 1);
                }
            }
        }

        return 1;
    }

    bool __cdecl saveModelFlagsInSave_patch_standard()
    {
        for (eFileType type : arrayOfFileTypes)
        {
            WORD defaultNumberOfFiles = g_LimitAdjuster.m_fileIDlimits.GetFileIDdefaultLimit(type);

            WORD numberOfFilesToProcess = g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type);
            WORD numberOfFilesToSkip = defaultNumberOfFiles - numberOfFilesToProcess;

            CStreamingInfo* iInfoForModel = &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[g_LimitAdjuster.m_fileIDlimits.GetBaseID(type)];

          
           // printf_MessageBox("File type: %d\nNumber of files to process: %d\nNumber of files to skip: %d\nInput ID: %d Output ID: %d",
           // type,
           // numberOfFilesToProcess,
           // numberOfFilesToSkip,
           /// CLimitAdjusterMain::GetBaseID(type),
           // CLimitAdjusterMain::CStreaming__defaultBaseID[type]
           // );
            //

            BYTE flags;

            for (WORD i = 0; i < numberOfFilesToProcess; i++)
            {
                CStreamingInfo* file = iInfoForModel + i;

                flags = -1;

                if (file->uiLoadFlag == 1)
                    flags = file->ucFlags;

                CGenericGameStorage_SaveDataToWorkBuffer(&flags, 1);
            }

            // Skip files
            flags = -1;

            while (numberOfFilesToSkip-- > 0)
                CGenericGameStorage_SaveDataToWorkBuffer(&flags, 1);
        }

        return true;
    }

    bool __cdecl saveModelFlagsInSave_patch_enhanced()
    {
        CGenericGameStorage_SaveDataToWorkBuffer(ENHANCED_FLAG_IDENTIFIER, ENHANCED_FLAG_IDENTIFIER_LEN);

        char format = 1;
        CGenericGameStorage_SaveDataToWorkBuffer(&format, 1);

        DWORD numberOfBytesFollowing = _countof(arrayOfFileTypes) * sizeof(uint16_t);

        for (eFileType type : arrayOfFileTypes)
            numberOfBytesFollowing += g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type);

        CGenericGameStorage_SaveDataToWorkBuffer(&numberOfBytesFollowing, sizeof(numberOfBytesFollowing));

        for (eFileType type : arrayOfFileTypes)
        {
            WORD numberOfFiles = g_LimitAdjuster.m_fileIDlimits.GetFileIDcurrentLimit(type);

            CGenericGameStorage_SaveDataToWorkBuffer(&numberOfFiles, sizeof(numberOfFiles));

            CStreamingInfo* FileInfoBase = &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[g_LimitAdjuster.m_fileIDlimits.GetBaseID(type)];

            for (WORD i = 0; i < numberOfFiles; i++)
            {
                CStreamingInfo* file = FileInfoBase + i;

                BYTE flags = -1;

                if (file->uiLoadFlag == 1)
                    flags = file->ucFlags;

                CGenericGameStorage_SaveDataToWorkBuffer(&flags, 1);
            }
        }

        return true;
    }

    // Initializes prev, next for info for model.
    void InitPrevNextForInfoForModel(tManagerBaseID firstID, tManagerBaseID lastID, CStreamingInfo** ppStart, CStreamingInfo** ppEnd)
    {
        *ppStart = &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[firstID];
        *ppEnd = &g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[lastID];

        tManagerBaseID curID = firstID;

        if (g_LimitAdjuster.m_fileIDlimits.bIsIDbaseInt32)
        {
            do
            {
                if (curID == firstID)
                    g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[curID].exUsPrev = -1;
                else
                    g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[curID].exUsPrev = curID - 1;

                if (curID == lastID)
                    g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[curID].exUsNext = -1;
                else
                    g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[curID].exUsNext = curID + 1;

                curID++;

            } while (curID <= lastID);
        }

        do
        {
            if (curID == firstID)
                g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[curID].usPrev = -1;
            else
                g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[curID].usPrev = curID - 1;

            if (curID == lastID)
                g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[curID].usNext = -1;
            else
                g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModel.gta_sa[curID].usNext = curID + 1;

            curID++;

        } while (curID <= lastID);
    }

    // Initializes loaded & requested.
    void __cdecl patch_CStreaming__Init2_initializeLoadedAndRequested()
    {
        // Loaded
        InitPrevNextForInfoForModel(g_LimitAdjuster.m_fileIDlimits.GetBaseID(FILE_TYPE_LOADED_START),
                                    g_LimitAdjuster.m_fileIDlimits.GetBaseID(FILE_TYPE_REQUESTED_START) - 1, (CStreamingInfo**)0x8E4C60,
                                    (CStreamingInfo**)0x8E4C5C);

        // Requested
        InitPrevNextForInfoForModel(g_LimitAdjuster.m_fileIDlimits.GetBaseID(FILE_TYPE_REQUESTED_START),
                                    g_LimitAdjuster.m_fileIDlimits.GetCountOfAllFileIDs() - 1, (CStreamingInfo**)0x8E4C58, (CStreamingInfo**)0x8E4C54);
    }

    void __cdecl patch_CStreaming__Init2_initalizeExtensionArray()
    {
        tManagerBaseID countOfFiles = g_LimitAdjuster.m_fileIDlimits.GetCountOfAllFileIDs();

        for (tManagerBaseID i = 0; i < countOfFiles; i++)
            g_LimitAdjuster.m_fileIDlimits.CStreaming__ms_aInfoForModelExtension[i].Init();
    }

    // Initializes CStreaming::ms_aInfoForModelExtension
    NAKED void patch_5B8AE7()
    {
        __asm
        {
			call patch_CStreaming__Init2_initalizeExtensionArray;

          // go back
			mov esi, ASM_ACCESS_LIMIT_VAR_32_BIT(m_fileIDlimits, FileIDlimit, CStreaming__ms_aInfoForModel);

			mov eax, 0x5B8AF0;
			jmp eax;
        }
    }

    // Initializes loaded & requested.
    NAKED void patch_CStreaming__Init2_5B8B12()
    {
        __asm
        {
			call patch_CStreaming__Init2_initializeLoadedAndRequested;

			mov eax, 0x5B8B7A;
			jmp eax;
        }
    }

    void __cdecl SetCollisionFileIDinCollisionModel(CColModelSAInterface* pColModel, int COL_ID)
    {
        pColModel->level = COL_ID; // ->COL_file_ID = COL_ID

        CColModel_extension* pCOLextData = g_LimitAdjuster.m_dynamicLimits.GetPointerToCOLmodelExtensionData(pColModel);
        pCOLextData->exCOL_file_ID = COL_ID;
    }

    int __cdecl GetCollisionFileIDfromCollisionModel(CColModel* pColModel)
    {
        CColModel_extension* pCOLextData = g_LimitAdjuster.m_dynamicLimits.GetPointerToCOLmodelExtensionData(pColModel);

        return pCOLextData->exCOL_file_ID;
    }

    int __cdecl GetCollisionFileIDfromCollisionModelSafe(CColModel* pColModel)
    {
        CColModel_extension* pCOLextData = g_LimitAdjuster.m_dynamicLimits.GetPointerToCOLmodelExtensionData(pColModel);

        //
       // printf_MessageBox(
       //     "COL model: 0x%X"
        //    "standard: %d ext: %d",
         //   pColModel,
         //   pColModel->COL_file_ID,
         //   pCOLextData->exCOL_file_ID
         //   );
        //

        if (pCOLextData)
            return pCOLextData->exCOL_file_ID;
        else
            return NULL;
    }

    NAKED void patch_CFileLoader__LoadCollisionFile_5B4FBD()
    {
        __asm
        {
			mov     edx, [esp + 40h + 8];

			push edx;
			push edi;
			call SetCollisionFileIDinCollisionModel;
			add esp, 8;			

			push 0x5B4FC4;
			retn;
        }
    }

    NAKED void patch_CObject__Init_59F8BE()
    {
        __asm
        {
			push edi;
			call GetCollisionFileIDfromCollisionModel;
			add esp, 4;

			push eax;

			push 0x59F8C3;
			retn;
        }
    }

    NAKED void patch_CFileLoader__LoadObjectInstance_5383D5()
    {
        __asm
        {
			push eax;
			call GetCollisionFileIDfromCollisionModelSafe;
			add esp, 4;
			mov bx, ax;
			test bx, bx;

			push 0x5383DA;
			retn;
        }
    }

    NAKED void patch_CFileLoader__LoadCollisionFile_538620()
    {
        __asm
        {
			mov eax, [esp + 44h + 0xC];

			push eax;
			push esi;
			call SetCollisionFileIDinCollisionModel;
			add esp, 8;

			push 0x538627;
			retn;
        }
    }

    NAKED void patch_CFileLoader__LoadCollisionFileFirstTime_5B5189()
    {
        __asm
        {
			mov     edx, [esp + 48h + 0xC];
			push edx;
			push esi;
			call SetCollisionFileIDinCollisionModel;
			add esp, 8;

			push 1;
			push esi;
			mov     ecx, edi;

			push 0x5B5195;
			retn;
        }
    }

    NAKED void patch_CPedModelInfo__CreateHitColModelSkinned_4C6F48()
    {
        __asm
        {
			push ecx;

			push 0;
			push esi;
			call SetCollisionFileIDinCollisionModel;
			add esp, 8;

			pop ecx;

			pop edi;

			push 0x4C6F4D;
			retn;
        }
    }

    NAKED void patch_CColStore__RemoveCol_1564EDE()
    {
        __asm
        {
			push ecx;


			push ecx;
			call GetCollisionFileIDfromCollisionModelSafe;
			add esp, 4;

			cmp     eax, ebx;

			pop ecx;
			push 0x1564EE4;
			retn;
        }
    }

    // Validates ped or vehicle model ID
    void __cdecl ValidatePedOrVehicleID(int ID)
    {
        if (ID >= (int)CDarkel::CountOfKillableModelIDs)
        {
            printf_MessageBox(
                "Attempt to define a ped/vehicle ID %d exceeding the limit of killable model IDs which is %d\n"
                "Please increase this limit.",
                ID, CDarkel::CountOfKillableModelIDs);
        }
    }

    NAKED void patch_5B6FCC()
    {
        __asm
        {
			mov     ecx, [esp + 0F8h - 0x94];
			push ecx;
			call ValidatePedOrVehicleID;
			add esp, 4;

			mov     ecx, [esp + 0F8h - 0x94];
			push ecx;

			push 0x5B6FD1;
			retn;
        }
    }

    NAKED void patch_5B74A2()
    {
        __asm
        {
			mov     eax, [esp + 178h - 0x128];
			push eax;
			call ValidatePedOrVehicleID;
			add esp, 4;

			mov     eax, [esp + 178h - 0x128];
			push eax;

			push 0x5B74A7;
			retn;
        }
    }
}            // namespace Game_GTASA

*/