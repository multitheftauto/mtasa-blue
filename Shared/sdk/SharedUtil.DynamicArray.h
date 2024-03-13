/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.DynamicArray.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#ifndef _MTA_DYNAMIC_ARRAY_H_
#define _MTA_DYNAMIC_ARRAY_H_

#include <memory>

namespace SharedUtil
{
    template <typename Type>
    struct ArrTest
    {
        Type* buffer;
        size_t size;
    };

    template <typename Type>
    class DynamicArray
    {
    public:
        using element_type = Type;
        using pointer = element_type*;
        using reference = element_type&;

        DynamicArray() : _value(nullptr), _size(0) {}
        DynamicArray(std::initializer_list<element_type> list)
            : _size(list.size()) {
            _value = new element_type[_size];
            for (auto i = 0; i < _size; i++)
                _value[i] = list[i];
        }

        ~DynamicArray() noexcept { delete[] _value; }

        DynamicArray(const DynamicArray& other) {
            _value = new element_type[other._size];
            _size = other._size;
            std::copy(other._value, other._value + _size, _value);
        }
        DynamicArray(DynamicArray&& other) noexcept {
            _value = std::exchange(other._value, nullptr);
            _size = std::exchange(other._size, 0);
        }

        DynamicArray& operator=(DynamicArray other) noexcept
        {
            other.swap(*this);
            return *this;
        }

        void swap(DynamicArray& other) noexcept
        {
            std::swap(_value, other._value);
            std::swap(_size, other._size);
        }

        friend void swap(DynamicArray& a, DynamicArray& b) noexcept
        {
            a.swap(b);
        }

        void append(element_type value) noexcept
        {
            pointer copy = new element_type[_size + 1];
            std::copy(_value, _value + _size, copy);
            delete[] _value;
            _value = copy;
            _value[_size] = value;
            _size++;
        }

        const size_t size() const noexcept { return this->_size; }

    private:
        pointer _value;
        size_t  _size;
    };
}

#endif
