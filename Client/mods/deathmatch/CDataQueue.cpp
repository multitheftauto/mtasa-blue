/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CDataQueue.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDataQueue.h"

CDataQueue::CDataQueue()
{
    m_queue = (Entry*)malloc(sizeof(struct Entry));

    size_t capacity = DEFAULT_CAPACITY;
    void*  data = malloc(capacity);

    m_queue->dataStartPointer = data;
    m_queue->capacity = capacity;
    m_queue->dataEndPointer = &data + capacity;
    m_queue->activeStartPointer = data;
    m_queue->activeEndPointer = data;
    m_queue->activeLength = 0;
}

CDataQueue::~CDataQueue()
{
    free(m_queue->dataStartPointer);
    free(m_queue);
}

void CDataQueue::UpgradeToLargerCapacity()
{
    size_t newCapacity = m_queue->capacity * 2;
    void*  newData = malloc(newCapacity);
    Peek(newData, m_queue->activeLength);

    void* oldData = m_queue->dataStartPointer;
    m_queue->dataStartPointer = newData;
    m_queue->capacity = newCapacity;
    m_queue->dataEndPointer = &newData + newCapacity;
    m_queue->activeStartPointer = newData;
    m_queue->activeEndPointer = &newData + m_queue->activeLength;

    free(oldData);
}

size_t CDataQueue::Enqueue(const void* data, size_t dataLength)
{
    if (data == nullptr)
        return -1;

    if (dataLength == 0)
        return 0;

    size_t newLength = m_queue->activeLength + dataLength;
    while (newLength > m_queue->capacity)
        UpgradeToLargerCapacity();

    void* newEndPointer = &m_queue->activeEndPointer + dataLength;

    if (newEndPointer <= m_queue->dataEndPointer)
    {
        memcpy(m_queue->activeEndPointer, data, dataLength);
        m_queue->activeEndPointer = &m_queue->activeEndPointer + dataLength;
    }
    else
    {
        size_t sizeToEnd = &m_queue->dataEndPointer - &m_queue->activeEndPointer;
        memcpy(m_queue->activeEndPointer, data, sizeToEnd);
        const void* dataLeft = &data + sizeToEnd;
        size_t      dataLeftLength = dataLength - sizeToEnd;
        memcpy(m_queue->dataStartPointer, dataLeft, dataLeftLength);
        m_queue->activeEndPointer = &m_queue->dataStartPointer + dataLeftLength;
    }

    m_queue->activeLength += dataLength;
    return dataLength;
}

size_t CDataQueue::Peek(void* data, size_t dataLength)
{
    size_t readLength = m_queue->activeLength > dataLength ? dataLength : m_queue->activeLength;
    void*  readEndPointer = &m_queue->activeStartPointer + readLength;

    if (readEndPointer <= m_queue->dataEndPointer)
        memcpy(data, m_queue->activeStartPointer, readLength);
    else
    {
        size_t sizeToEnd = &m_queue->dataEndPointer - &m_queue->activeStartPointer;
        memcpy(data, m_queue->activeStartPointer, sizeToEnd);
        void*  secondReadPointer = &data + sizeToEnd;
        size_t dataLeft = readLength - sizeToEnd;
        memcpy(secondReadPointer, m_queue->dataStartPointer, dataLeft);
    }

    return readLength;
}

size_t CDataQueue::Dequeue(void* data, size_t dataLength)
{
    size_t readLength = Peek(data, dataLength);
    void*  newStartPointer = &m_queue->activeStartPointer + readLength;
    if (newStartPointer <= m_queue->dataEndPointer)
        m_queue->activeStartPointer = newStartPointer;
    else
    {
        size_t sizeToEnd = &m_queue->dataEndPointer - &m_queue->activeStartPointer;
        size_t dataLeft = readLength - sizeToEnd;
        m_queue->activeStartPointer = &m_queue->dataStartPointer + dataLeft;
    }

    m_queue->activeLength -= readLength;
    return readLength;
}

void CDataQueue::Clear()
{
    m_queue->activeStartPointer = &m_queue->dataStartPointer;
    m_queue->activeEndPointer = &m_queue->dataStartPointer;
    m_queue->activeLength = 0;
}
