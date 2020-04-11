/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CDataQueue.h
 *  PURPOSE:     
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define DEFAULT_CAPACITY 4096

class CDataQueue
{
public:
    CDataQueue();
    ~CDataQueue();

    void   UpgradeToLargerCapacity();
    size_t Enqueue(const void* data, size_t dataLength);
    size_t Peek(void* data, size_t dataLength);
    size_t Dequeue(void* data, size_t dataLength);
    size_t GetLength() { return m_queue->activeLength; }
    void   Clear();

private:
    struct Entry
    {
        void* dataStartPointer;
        void* dataEndPointer;
        void* activeStartPointer;
        void* activeEndPointer;

        size_t capacity;
        size_t activeLength;
    };
    Entry* m_queue;
};
