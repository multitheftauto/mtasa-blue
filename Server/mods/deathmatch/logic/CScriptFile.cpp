/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CScriptFile.cpp
 *  PURPOSE:     Script file element class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CScriptFile.h"
#include "CGame.h"
#include "CResourceManager.h"

CScriptFile::CScriptFile(uint uiScriptId, const char* szFilename, unsigned long ulMaxSize) : CElement(NULL)
{
    // Init
    m_iType = CElement::SCRIPTFILE;
    SetTypeName("file");
    m_uiScriptId = uiScriptId;
    m_pFile = NULL;
    m_strFilename = szFilename ? szFilename : "";
    m_ulMaxSize = ulMaxSize;
    m_pResource = nullptr;
}

CScriptFile::~CScriptFile()
{
    // Close the file
    Unload();
}

bool CScriptFile::Load(CResource* pResourceForFilePath, eMode Mode)
{
    // If we haven't already got a file
    if (!m_pFile)
    {
        string strFilePath;

        switch (Mode)
        {
            // Open file in read only binary mode
            case MODE_READ:
                if (pResourceForFilePath->GetFilePath(m_strFilename.c_str(), strFilePath))
                    m_pFile = File::Fopen(strFilePath.c_str(), "rb");
                break;

            // Open file in read write binary mode.
            case MODE_READWRITE:
                // Try to load the file in rw mode. Use existing content.
                if (pResourceForFilePath->GetFilePath(m_strFilename.c_str(), strFilePath))
                    m_pFile = File::Fopen(strFilePath.c_str(), "rb+");
                break;

            // Open file in read write binary mode. Truncate size to 0.
            case MODE_CREATE:
                strFilePath = pResourceForFilePath->GetResourceDirectoryPath() + m_strFilename;
                MakeSureDirExists(strFilePath.c_str());
                m_pFile = File::Fopen(strFilePath.c_str(), "wb+");
                break;
        }

        // Return whether we successfully opened it or not
        if (m_pFile)
        {
            CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromScriptID(m_uiScriptId);
            if (pResource && pResource->GetVirtualMachine())
                pResource->GetVirtualMachine()->OnOpenFile(m_strFilename);

            m_pResource = pResourceForFilePath;
        }
        return m_pFile != NULL;
    }

    // Failed
    return false;
}

void CScriptFile::Unload()
{
    // Loaded?
    if (m_pFile)
    {
        // Close the file
        fclose(m_pFile);
        m_pFile = NULL;
        CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromScriptID(m_uiScriptId);
        if (pResource && pResource->GetVirtualMachine())
            pResource->GetVirtualMachine()->OnCloseFile(m_strFilename);
    }
}

bool CScriptFile::IsEOF()
{
    if (!m_pFile)
        return true;

    // Reached end of file?
    return feof(m_pFile) != 0;
}

long CScriptFile::GetPointer()
{
    if (!m_pFile)
        return -1;

    return ftell(m_pFile);
}

long CScriptFile::GetSize()
{
    if (!m_pFile)
        return -1;

    // Remember current position and seek to the end
    long lCurrentPos = ftell(m_pFile);
    fseek(m_pFile, 0, SEEK_END);

    // Retrieve size of file
    long lSize = ftell(m_pFile);

    // Seek back to where the pointer was
    fseek(m_pFile, lCurrentPos, SEEK_SET);

    // Return the size
    return lSize;
}

long CScriptFile::SetPointer(unsigned long ulPosition)
{
    if (!m_pFile)
        return -1;

    // Is the new position bigger than the file?
    if (GetSize() < static_cast<long>(ulPosition))
    {
        // Don't make it bigger than our limit
        if (ulPosition > m_ulMaxSize)
        {
            ulPosition = m_ulMaxSize;
        }
    }

    // Move the pointer
    fseek(m_pFile, ulPosition, SEEK_SET);

    // Bigger than file size? Tell the script how far we were able to move it
    long lSize = GetSize();
    if (ulPosition > static_cast<unsigned long>(lSize))
    {
        ulPosition = static_cast<unsigned long>(lSize);
    }

    // Return the new position
    return ulPosition;
}

void CScriptFile::SetSize(unsigned long ulNewSize)
{
    // TODO: A way to truncate a file
    if (!m_pFile)
        return;
}

void CScriptFile::Flush()
{
    if (!m_pFile)
        return;

    fflush(m_pFile);
}

long CScriptFile::Read(unsigned long ulSize, SString& outBuffer)
{
    if (!m_pFile)
        return -1;

    // If read size is large, limit it to how many bytes can be read (avoid memory problems with over allocation)
    if (ulSize > 10000)
    {
        long lCurrentPos = ftell(m_pFile);
        fseek(m_pFile, 0, SEEK_END);
        long lFileSize = ftell(m_pFile);
        fseek(m_pFile, lCurrentPos, SEEK_SET);
        ulSize = std::min<unsigned long>(1 + lFileSize - lCurrentPos, ulSize);
        // Note: Read extra byte at end so EOF indicator gets set
    }

    try
    {
        outBuffer.resize(ulSize);
    }
    catch (const std::bad_alloc&)
    {
        return -2;
    }

    return fread(outBuffer.data(), 1, ulSize, m_pFile);
}

long CScriptFile::Write(unsigned long ulSize, const char* pData)
{
    if (!m_pFile)
        return -1;

    // Write the data into the given block. Return number of bytes we wrote.
    return fwrite(pData, 1, ulSize, m_pFile);
}

long CScriptFile::GetContents(std::string& buffer)
{
    if (!m_pFile)
        return -1;

    // Store the current position to restore it later.
    const long currentPos = ftell(m_pFile);

    // Move to the end of the file to determine the size.
    fseek(m_pFile, 0, SEEK_END);
    const long fileSize = ftell(m_pFile);

    try
    {
        buffer.resize(fileSize);
    }
    catch (const std::bad_alloc&)
    {
        fseek(m_pFile, currentPos, SEEK_SET);
        return -2;
    }

    // Move to the start of the file to read the entire file.
    fseek(m_pFile, 0, SEEK_SET);
    const long bytesRead = fread(buffer.data(), 1, fileSize, m_pFile);
    fseek(m_pFile, currentPos, SEEK_SET);
    buffer.resize(bytesRead);
    return bytesRead;
}

CResource* CScriptFile::GetResource()
{
    return m_pResource;
}

CResourceFile* CScriptFile::GetResourceFile() const
{
    return m_pResource->GetResourceFile(m_strFilename);
}
