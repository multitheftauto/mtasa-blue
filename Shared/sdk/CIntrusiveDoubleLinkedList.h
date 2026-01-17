/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/CIntrusiveDoubleLinkedList.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <initializer_list>

// Double-linked list node base class.
struct CIntrusiveDoubleLinkedListNode
{
    // Pointer to previous node.
    CIntrusiveDoubleLinkedListNode* m_previous{};

    // Pointer to next node.
    CIntrusiveDoubleLinkedListNode* m_next{};
};

// Double-linked list template class. Elements must inherit from CIntrusiveDoubleLinkedListNode.
template <class T> class CIntrusiveDoubleLinkedList
{
public:
    struct SIterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        SIterator(T* ptr) :
            m_ptr(ptr)
        {
        }

        // Point to the node value.
        reference operator*() const { return *m_ptr; }

        // Dereference the node value.
        pointer operator->() const { return m_ptr; }

        // Prefix increment
        SIterator& operator++()
        {
            assert(m_ptr);
            m_ptr = static_cast<T*>(m_ptr->m_next);
            return *this;
        }

        // Prefix decrement
        SIterator& operator--()
        {
            assert(m_ptr);
            m_ptr = static_cast<T*>(m_ptr->m_previous);
            return *this;
        }

        // Postfix increment
        SIterator operator++(int)
        {
            SIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // Postfix decrement
        SIterator operator--(int)
        {
            SIterator tmp = *this;
            --(*this);
            return tmp;
        }

        // Test for equality with another iterator.
        friend bool operator== (const SIterator& a, const SIterator& b) { return a.m_ptr == b.m_ptr; };

        // Test for inequality with another iterator.
        friend bool operator!= (const SIterator& a, const SIterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        T* m_ptr;
    };

    struct SConstIterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        SConstIterator(T* ptr) :
            m_ptr(ptr)
        {
        }

        // Point to the node value.
        const reference operator*() const { return *m_ptr; }

        // Dereference the node value.
        const pointer operator->() const { return m_ptr; }

        // Prefix increment
        SConstIterator& operator++()
        {
            assert(m_ptr);
            m_ptr = static_cast<T*>(m_ptr->m_next);
            return *this;
        }

        // Prefix decrement
        SConstIterator& operator--()
        {
            assert(m_ptr);
            m_ptr = static_cast<T*>(m_ptr->m_previous);
            return *this;
        }

        // Postfix increment
        SConstIterator operator++(int)
        {
            SConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // Postfix decrement
        SConstIterator operator--(int)
        {
            SConstIterator tmp = *this;
            --(*this);
            return tmp;
        }

        // Test for equality with another iterator.
        friend bool operator== (const SConstIterator& a, const SConstIterator& b) { return a.m_ptr == b.m_ptr; };

        // Test for inequality with another iterator.
        friend bool operator!= (const SConstIterator& a, const SConstIterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        T* m_ptr;
    };

    // Construct empty.
    CIntrusiveDoubleLinkedList() :
        m_head(nullptr)
    {
    }

    // Non-copyable.
    CIntrusiveDoubleLinkedList(const CIntrusiveDoubleLinkedList<T>& list) = delete;

    // Aggregate initialization constructor.
    CIntrusiveDoubleLinkedList(const std::initializer_list<T>& list) : CIntrusiveDoubleLinkedList()
    {
        for (auto it = list.begin(); it != list.end(); it++)
            Insert(*it);
    }

    // Non-assignable.
    CIntrusiveDoubleLinkedList<T>& operator =(const CIntrusiveDoubleLinkedList<T>& list) = delete;

    // Destruct.
    ~CIntrusiveDoubleLinkedList()
    {
        Clear();
    }

    // Remove all elements.
    void Clear()
    {
        T* element = m_head;
        while (element)
        {
            T* next = Next(element);
            delete element;
            element = next;
        }
        m_head = nullptr;
    }

    // Insert an element at the beginning.
    void InsertFront(T* element)
    {
        if (element)
        {
            element->m_next = m_head;

            if (m_head)
                m_head->m_previous = element;

            m_head = element;
        }
    }

    // Insert an element at the end.
    void Insert(T* element)
    {
        if (m_head)
        {
            T* tail = Last();
            element->m_previous = tail;
            tail->m_next = element;
        }
        else
            m_head = element;
    }

    // Erase an element. Return true if successful.
    void Erase(T* element)
    {
        Detach(element);
        delete element;
    }

    void Detach(T* element)
    {
        assert(element);
        
        T* previous = Previous(element);
        T* next = Next(element);

        if (previous)
            previous->m_next = next;

        if (next)
            next->m_previous = previous;

        if (element == m_head)
            m_head = next;

        // Invalidate element's references
        element->m_previous = nullptr;
        element->m_next = nullptr;
    }    

    // Return first element, or null if empty.
    T* First() const { return m_head; }

    // Return last element, or null if empty.
    T* Last() const
    {
        T* element = m_head;
        if (element)
        {
            while (element->m_next)
                element = Next(element);
        }
        return element;
    }

    // Return previous element, or null if no more elements.
    T* Previous(T* element) const { return element ? static_cast<T*>(element->m_previous) : nullptr; }

    // Return next element, or null if no more elements.
    T* Next(T* element) const { return element ? static_cast<T*>(element->m_next) : nullptr; }   

    // Return whether is empty.
    bool Empty() const { return m_head == nullptr; }

    // Return iterator to the first element.
    SIterator begin() { return SIterator(m_head); }

    // Return iterator to the end.
    SIterator end() { return SIterator(nullptr); }

    // Return iterator to the first element.
    SConstIterator begin() const { return SConstIterator(m_head); }

    // Return iterator to the end.
    SConstIterator end() const { return SConstIterator(nullptr); }

private:
    // First element.
    T* m_head;
};
