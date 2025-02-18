/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CScriptFile.cpp
 *  PURPOSE:     Script file class
 *
 *****************************************************************************/

#include <StdInc.h>

CScriptFile::CScriptFile(uint uiScriptId, const char* szFilename, unsigned long ulMaxSize, eAccessType accessType)
    : ClassInit(this), CClientEntity(INVALID_ELEMENT_ID)
{
    // Init
    SetTypeName("file");
    m_pFile = NULL;
    m_strFilename = szFilename ? szFilename : "";
    m_ulMaxSize = ulMaxSize;
    m_uiScriptId = uiScriptId;
    m_accessType = accessType;
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
        m_bDoneResourceFileCheck = false;
        m_pFile = g_pNet->AllocateBinaryFile();
        bool bOk = false;
        m_strAbsPath = pResourceForFilePath->GetResourceDirectoryPath(m_accessType, m_strFilename);
        switch (Mode)
        {
            // Open file in read only binary mode
            case MODE_READ:
                bOk = m_pFile->FOpen(m_strAbsPath, "rb", true);
                break;

            // Open file in read write binary mode.
            case MODE_READWRITE:
                // Try to load the file in rw mode. Use existing content.
                bOk = m_pFile->FOpen(m_strAbsPath, "rb+", true);
                break;

            // Open file in read write binary mode. Truncate size to 0.
            case MODE_CREATE:
                bOk = m_pFile->FOpen(m_strAbsPath, "wb+", true);
                break;
        }

        if (!bOk)
        {
            SAFE_DELETE(m_pFile);
        }

        m_pResource = pResourceForFilePath;

        // Return whether we successfully opened it or not
        return m_pFile != NULL;
    }

    // Failed
    return false;
}

void CScriptFile::Unload()
{
    // Close the file if required
    SAFE_DELETE(m_pFile);
}

bool CScriptFile::IsEOF()
{
    if (!m_pFile)
        return true;

    // Reached end of file?
    return m_pFile->FEof();
}

long CScriptFile::GetPointer()
{
    if (!m_pFile)
        return -1;

    return m_pFile->FTell();
}

long CScriptFile::GetSize()
{
    if (!m_pFile)
        return -1;

    // Remember current position and seek to the end
    long lCurrentPos = m_pFile->FTell();
    m_pFile->FSeek(0, SEEK_END);

    // Retrieve size of file
    long lSize = m_pFile->FTell();

    // Seek back to where the pointer was
    m_pFile->FSeek(lCurrentPos, SEEK_SET);

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
    m_pFile->FSeek(ulPosition, SEEK_SET);

    // Bigger than file size? Tell the script how far we were able to move it
    long lSize = GetSize();
    if (ulPosition > static_cast<unsigned long>(lSize))
    {
        ulPosition = static_cast<unsigned long>(lSize);
    }

    // Return the new position
    return ulPosition;
}

void CScriptFile::Flush()
{
    if (!m_pFile)
        return;

    m_pFile->FFlush();
}

long CScriptFile::Read(unsigned long ulSize, SString& outBuffer)
{
    if (!m_pFile)
        return -1;

    DoResourceFileCheck();

    // If read size is large, limit it to how many bytes can be read (avoid memory problems with over allocation)
    if (ulSize > 10000)
    {
        long lCurrentPos = m_pFile->FTell();
        m_pFile->FSeek(0, SEEK_END);
        long lFileSize = m_pFile->FTell();
        m_pFile->FSeek(lCurrentPos, SEEK_SET);
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

    return m_pFile->FRead(outBuffer.data(), ulSize);
}

long CScriptFile::Write(unsigned long ulSize, const char* pData)
{
    if (!m_pFile)
        return -1;

    // Write the data into the given block. Return number of bytes we wrote.
    return m_pFile->FWrite(pData, ulSize);
}

long CScriptFile::GetContents(std::string& buffer)
{
    if (!m_pFile)
        return -1;

    // Store the current position to restore it later.
    const int currentPos = m_pFile->FTell();

    // Move to the end of the file to determine the size.
    m_pFile->FSeek(0, SEEK_END);
    const int fileSize = m_pFile->FTell();

    try
    {
        buffer.resize(fileSize);
    }
    catch (const std::bad_alloc&)
    {
        m_pFile->FSeek(currentPos, SEEK_SET);
        return -2;
    }

    // Move to the start of the file to read the entire file.
    m_pFile->FSeek(0, SEEK_SET);
    const int bytesRead = m_pFile->FRead(buffer.data(), fileSize);
    m_pFile->FSeek(currentPos, SEEK_SET);
    buffer.resize(bytesRead);
    return bytesRead;
}

// If file was downloaded with a resource, validate checksum
void CScriptFile::DoResourceFileCheck()
{
    if (!m_pFile || m_bDoneResourceFileCheck)
        return;
    m_bDoneResourceFileCheck = true;

    if (g_pClientGame->GetResourceManager()->IsResourceFile(m_strFilename))
    {
        // Remember current position and seek to the end
        long lCurrentPos = m_pFile->FTell();
        m_pFile->FSeek(0, SEEK_END);

        // Retrieve size of file
        long lSize = m_pFile->FTell();

        // Read data
        CBuffer buffer;
        buffer.SetSize(lSize);
        m_pFile->FSeek(0, SEEK_SET);
        m_pFile->FRead(buffer.GetData(), buffer.GetSize());

        // Seek back to where the pointer was
        m_pFile->FSeek(lCurrentPos, SEEK_SET);

        // Check file content
        g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strFilename, buffer.GetData(), buffer.GetSize());
    }
}

CResource* CScriptFile::GetResource()
{
    return m_pResource;
}

CResourceFile* CScriptFile::GetResourceFile() const
{
    return m_pResource->GetResourceFile(m_strFilename);
}
