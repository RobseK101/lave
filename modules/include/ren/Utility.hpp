#pragma once
#include <cstdint>

/// @file
/// @brief ren namespace utility functions

namespace ren
{
    inline constexpr size_t calculateBlockCount(size_t _packedSize, size_t _blockSize)
    {
	    return (_packedSize % _blockSize) ? _packedSize / _blockSize + 1 : _packedSize / _blockSize;
    }

    template <size_t blockSize>
    inline constexpr size_t calculateBlockCount(size_t _packedSize)
    {
        return (_packedSize % blockSize) ? _packedSize / blockSize + 1 : _packedSize / blockSize;
    }

    template <size_t alignment>
    inline constexpr size_t align(size_t _packedSize)
    {
        return calculateBlockCount<alignment>(_packedSize) * alignment;
    }

    typedef uint32_t FourCC;

    /// @brief Strictly assumes little-endian format!
    inline constexpr FourCC makeFourCC(const char _chars[5])
    {
	    return (static_cast<uint32_t>(_chars[0]))
	    	| (static_cast<uint32_t>(_chars[1]) << 8)
	    	| (static_cast<uint32_t>(_chars[2]) << 16)
	    	| (static_cast<uint32_t>(_chars[3]) << 24);
    }

    /// @brief This template requires that C::operator[] returns a reference to an object of type T.
    template<class C, class T>
    inline intmax_t findUnsorted(const C& _container, const T& _value, size_t _startIndex = 0) 
    {
        for (size_t i = 0; i < _container.size(); i++) {
            if (_container[i] == _value) {
                return i;
            }
        }
        return -1;
    }

    /// @brief This template requires that C::operator[] returns a reference to an object of type T.
    /// @param   Comp follows the syntax of: 
    ///          lhs < rhs == true -> find the smallest value;
    ///          rhs > lhs == true -> find the largest value.
    ///          In other words: in each iteration the function promotes the currently checked value (lhs) to the current extreme (rhs) if the 
    ///          conditions of Comp are satisfied.
    /// @returns The index of the first value satisfying the criterion accross all values.
    template<class C, class Compare>
    inline intmax_t findExtremeUnsorted(const C& _container, Compare _comp)
    {
        if (_container.size() == 0) {
            return -1;
        }

        size_t currentExtreme = 0;
        for (size_t i = 1; i < _container.size(); i++) {
            if (_comp(_container[i], currentExtreme)) {
                currentExtreme = i;
            }
        }
        return currentExtreme;
    }
}