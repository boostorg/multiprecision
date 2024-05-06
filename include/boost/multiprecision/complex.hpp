///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMPLEX_HPP
#define BOOST_MP_COMPLEX_HPP

#include <boost/multiprecision/detail/assert.hpp>
#include <complex>
#include <cmath>

namespace boost {
namespace multiprecision {

template <typename T>
class complex
{
private:
    T real_;
    T imag_;

public:
    constexpr complex() noexcept = default;
    explicit constexpr complex(T real) noexcept : real_ {real}, imag_ {T{0}} {}
    constexpr complex(T real, T imag) noexcept : real_ {real}, imag_ {imag} {}
    constexpr complex& operator=(const complex<T>& rhs) = default;

    constexpr T real() noexcept { return real_; }
    constexpr T imag() noexcept { return imag_; }

    friend constexpr complex<T> operator+(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend constexpr complex<T> operator-(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend constexpr complex<T> operator*(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend constexpr complex<T> operator/(const complex<T>& lhs, const complex<T>& rhs) noexcept;

    constexpr complex& operator+=(const complex<T>& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    constexpr complex& operator-=(const complex<T>& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    constexpr complex& operator*=(const complex<T>& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    constexpr complex& operator/=(const complex<T>& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }

    friend inline complex<T> polar(const T& rho, const T& theta) noexcept;
};

template <typename T>
constexpr complex<T> operator+(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ + rhs.real_, lhs.imag_ + rhs.imag_};
}

template <typename T>
constexpr complex<T> operator-(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ - rhs.real_, lhs.imag_ - rhs.imag_};
}

template <typename T>
constexpr complex<T> operator*(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ * rhs.real_ - lhs.imag_ * rhs.imag_, lhs.imag_ * rhs.real_ + lhs.real_ * rhs.imag_};
}

template <typename T>
constexpr complex<T> operator/(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
    const T real_part = (lhs.real_ * rhs.real_ + lhs.imag_ * rhs.imag_) / divisor;
    const T imag_part = (lhs.imag_ * rhs.real_ - lhs.real_ * rhs.imag_) / divisor;
    return {real_part, imag_part};
}

template <typename T>
inline complex<T> polar(const T& rho, const T& theta) noexcept
{
    using std::sin;
    using std::cos;

    BOOST_MP_ASSERT_MSG(rho >= T{0}, "Rho must be positive");

    return {rho * cos(theta), rho * sin(theta)};
}

} // Namespace multiprecision
} // Namespace boost

#endif //BOOST_MP_COMPLEX_HPP
