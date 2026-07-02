#pragma once
#include <type_traits>
#include <cmath>

/// @file 
/// @brief Collection of recurring mathematical functions.

namespace ren
{
    namespace math
    {
        /// @brief Restricts a value to the range _min <= _value <= _max. 
        /// @tparam T Value type
        /// @param _value The value to be modified.
        /// @param _min Result in case of underflow. 
        /// @param _max Result in case of overflow.
        template<typename T>
        inline void clamp(T* _value, T _min, T _max)
        {
            if (*_value < _min) {
                *_value = _min;
            }
            else if (*_value > _max) {
                *_value = _max;
            }
        }

        /// @brief Restricts a value to the range _min <= _value <= _max. 
        /// @tparam T Value type
        /// @param _value 
        /// @param _min Result in case of underflow. 
        /// @param _max Result in case of overflow.
        /// @return 
        template<typename T>
        inline constexpr T clamp(T _value, T _min, T _max)
        {
            if (_value < _min) {
                return _min;
            }
            else if (_value > _max) {
                return _max;
            }
            else {
                return _value;
            }
        }

        /// @brief Restricts a value to the range _Min <= _value <= _Max. 
        /// @tparam T Value type
        /// @tparam _Min Result in case of underflow. 
        /// @tparam _Max Result in case of overflow.
        /// @param _value 
        /// @return 
        template <typename T, T _Min, T _Max>
        inline constexpr T clamp(T _value)
        {
            if (_value < _Min) {
                return _Min;
            }
            else if (_value > _Max) {
                return _Max;
            }
            else {
                return _value;
            }
        }

        /// @brief A clamped value. Uses ren::math::clamp<T,_Min,_Max>() internally. 
        /// @tparam T Value type
        /// @tparam _Min Minimum value
        /// @tparam _Max Maximum value
        template <typename T, T _Min, T _Max>
        class ClampedValue
        {
        public:
            ClampedValue() = default;
            ClampedValue(const ClampedValue&) = default;
            ClampedValue(T _value) : 
                p_value(clamp<T, _Min, _Max>(_value)) {}

            ClampedValue& operator=(T _value) {
                p_value = clamp<T, _Min, _Max>(_value);
                return *this;
            }

            operator T() const {
                return p_value;
            }

            ClampedValue& operator+=(T _rhs) {
                if (_rhs < 0) {
                    subtractClean(-_rhs);
                }
                else {
                    addClean(_rhs);
                }
                return *this;
            }

            ClampedValue& operator-=(T _rhs) {
                if (_rhs < 0) {
                    addClean(-_rhs);
                }
                else {
                    subtractClean(_rhs);
                }
                return *this;
            }

        private:
            void addClean(T _value) {
                if (_value >= _Max - p_value) {
                    p_value = _Max;
                }
                else {
                    p_value += _value;
                }
            }

            void subtractClean(T _value) {
                if (_value >= p_value - _Min) {
                    p_value = _Min;
                }
                else {
                    p_value -= _value;
                }
            }

            T p_value;
        };

        /// @brief Restricts a value to _floor <= _value < _ceiling, but the value "rolls over" 
        ///        like an integer of insufficient width.
        /// @tparam Int The value type; this must be an integral type.
        /// @param _value 
        /// @param _floor The minimum of the allowable range.
        /// @param _ceiling The value just above the allowable range. 
        /// @return 
        template<typename Int>
        inline constexpr Int roll(Int _value, Int _floor, Int _ceiling)
        {
            static_assert(std::is_integral_v<Int> == true, "Integral required.");

            if (_value < _floor) {
                Int interval = _ceiling - _floor;
                Int distance = _floor - _value;
                Int remainder = distance % interval;
                return _ceiling - remainder;
            }
            else if (_value >= _ceiling) {
                Int interval = _ceiling - _floor;
                Int distance = _value - _ceiling;
                Int remainder = distance % interval;
                return _floor + remainder;
            }
            else return _value;
        }

        /// @brief Restricts a value to _Floor <= _value < _Ceiling, but the value "rolls over" 
        ///        like an integer of insufficient width.
        /// @tparam Int The value type; this must be an integral type.
        /// @tparam _Floor The minimum of the allowable range.
        /// @tparam _Ceiling The value just above the allowable range. 
        /// @param _value 
        /// @return 
        template <typename Int, Int _Floor, Int _Ceiling>
        inline constexpr Int roll(Int _value)
        {
            static_assert(std::is_integral_v<Int> == true, "Integral required.");
            static_assert(_Floor < _Ceiling, "The floor must be less than the ceiling.");

            constexpr Int interval = _Ceiling - _Floor;

            if (_value < _Floor) {
                Int distance = _Floor - _value;
                Int remainder = distance / interval;
                return _Ceiling - remainder;
            }
            else if (_value >= _Ceiling) {
                Int distance = _value - _Ceiling;
                Int remainder = distance / interval;
                return _Floor + remainder;
            }
            else {
                return _value;
            }
        }

        /// @brief A "rollover value" in the sense of ren::math::roll<Int,_Floor,_Ceiling>().
        /// @tparam Int Int The value type; this must be an integral type.
        /// @tparam _Floor The minimum of the allowable range.
        /// @tparam _Ceiling The value just above the allowable range. 
        template <typename Int, Int _Floor, Int _Ceiling>
        class RestrictedInteger
        {
        public:
            RestrictedInteger() = default;
            RestrictedInteger(const RestrictedInteger&) = default;
            RestrictedInteger(Int _value) : 
                p_value(roll<Int, _Floor, _Ceiling>(_value)) {}

            RestrictedInteger& operator=(Int _value) {
                p_value = roll<Int, _Floor, _Ceiling>(_value);
                return *this;
            }

            operator Int() const {
                return p_value;
            }

            RestrictedInteger& operator+=(Int _rhs) {
                if (_rhs < 0) {
                    subtractClean(-_rhs);
                }
                else {
                    addClean(_rhs);
                }
                return *this;
            }

            RestrictedInteger& operator-=(Int _rhs) {
                if (_rhs < 0) {
                    addClean(-_rhs);
                }
                else {
                    subtractClean(_rhs);
                }
                return *this;
            }

            RestrictedInteger& operator++() {
                if (p_value < _Ceiling - 1) {
                    p_value++;
                }
                else {
                    p_value = _Floor;
                }
                return *this;
            }

            RestrictedInteger& operator++(int) {
                operator++();
                return *this;
            }

            RestrictedInteger& operator--() {
                if (p_value > _Floor) {
                    p_value--;
                }
                else {
                    p_value = _Ceiling - 1;
                }
                return *this;
            }

            RestrictedInteger& operator--(int) {
                operator--();
                return this;
            }

        private:
            void addClean(Int _value) {
                Int remainingRange = _Ceiling - p_value;
                if (_value >= remainingRange) {
                    _value -= remainingRange;
                    p_value = roll<Int, _Floor, _Ceiling>(_value);
                }
                else {
                    p_value += _value;
                }
            }

            void subtractClean(Int _value) {
                if (_value > p_value) {
                    _value -= p_value;
                    p_value = _Ceiling - roll<Int, _Floor, _Ceiling>(_value);
                }
                else {
                    p_value -= _value;
                }
            }

            Int p_value;
        };

        template<typename T>
        inline constexpr T max(T _lhs, T _rhs) 
        {
            return (_lhs > _rhs) ? _lhs : _rhs;
        }

        template<typename T>
        inline constexpr T min(T _lhs, T _rhs) 
        {
            return (_lhs < _rhs) ? _lhs : _rhs;
        }

        
    }
}