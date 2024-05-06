///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMPLEX_HPP
#define BOOST_MP_COMPLEX_HPP

#include <boost/multiprecision/detail/assert.hpp>
#include <boost/multiprecision/traits/is_backend.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <complex>
#include <cmath>

namespace boost {
namespace multiprecision {
/*
template <typename T>
class complex
{
private:
    T real_;
    T imag_;

public:
    complex() noexcept = default;
    complex(T real) noexcept : real_ {real}, imag_ {T{0}} {}
    complex(T real, T imag) noexcept : real_ {real}, imag_ {imag} {}
    complex& operator=(const complex<T>& rhs)
    {
        real_ = rhs.real_;
        imag_ = rhs.imag_;
    }

    T real() noexcept { return real_; }
    T imag() noexcept { return imag_; }

    complex<T> operator+() { return *this; }
    complex<T> operator-() { return {-real_, -imag_}; }

    friend complex<T> operator+(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend complex<T> operator-(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend complex<T> operator*(const complex<T>& lhs, const complex<T>& rhs) noexcept;
    friend complex<T> operator/(const complex<T>& lhs, const complex<T>& rhs) noexcept;

    complex& operator+=(const complex<T>& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    complex& operator-=(const complex<T>& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    complex& operator*=(const complex<T>& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    complex& operator/=(const complex<T>& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }
};

template <typename T>
complex<T> operator+(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ + rhs.real_, lhs.imag_ + rhs.imag_};
}

template <typename T>
complex<T> operator-(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ - rhs.real_, lhs.imag_ - rhs.imag_};
}

template <typename T>
complex<T> operator*(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    return {lhs.real_ * rhs.real_ - lhs.imag_ * rhs.imag_, lhs.imag_ * rhs.real_ + lhs.real_ * rhs.imag_};
}

template <typename T>
complex<T> operator/(const complex<T>& lhs, const complex<T>& rhs) noexcept
{
    const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
    const T real_part = (lhs.real_ * rhs.real_ + lhs.imag_ * rhs.imag_) / divisor;
    const T imag_part = (lhs.imag_ * rhs.real_ - lhs.real_ * rhs.imag_) / divisor;
    return {real_part, imag_part};
}

template <typename T, std::enable_if_t<detail::is_backend<T>::value, bool> = true>
inline complex<T> polar(const T& rho, const T& theta) noexcept
{
    using std::sin;
    using std::cos;

    BOOST_MP_ASSERT_MSG(rho >= T{0}, "Rho must be positive");

    return {rho * cos(theta), rho * sin(theta)};
}
*/
} // Namespace multiprecision
} // Namespace boost

namespace std {

template <>
class complex<boost::multiprecision::cpp_bin_float_50>
{
private:
    using T = boost::multiprecision::cpp_bin_float_50;

    T real_;
    T imag_;

public:
    complex() noexcept = default;
    complex(T real) noexcept : real_ {real}, imag_ {T{0}} {}
    complex(T real, T imag) noexcept : real_ {real}, imag_ {imag} {}
    complex& operator=(const complex<T>& rhs) = default;

    T real() const noexcept { return real_; }
    T imag() const noexcept { return imag_; }

    complex<T> operator+() { return *this; }
    complex<T> operator-() { return {-real_, -imag_}; }

    complex<T> operator+(const complex<T>& rhs) noexcept
    {
        return {real_ + rhs.real_, imag_ + rhs.imag_};
    }

    complex<T> operator-(const complex<T>& rhs) noexcept
    {
        return {real_ - rhs.real_, imag_ - rhs.imag_};
    }

    complex<T> operator*(const complex<T>& rhs) noexcept
    {
        return {real_ * rhs.real_ - imag_ * rhs.imag_, imag_ * rhs.real_ + real_ * rhs.imag_};
    }

    complex<T> operator/(const complex<T>& rhs) noexcept
    {
        const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
        const T real_part = (real_ * rhs.real_ + imag_ * rhs.imag_) / divisor;
        const T imag_part = (imag_ * rhs.real_ - real_ * rhs.imag_) / divisor;
        return {real_part, imag_part};
    }

    complex& operator+=(const complex<T>& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    complex& operator-=(const complex<T>& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    complex& operator*=(const complex<T>& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    complex& operator/=(const complex<T>& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }
};

inline complex<boost::multiprecision::cpp_bin_float_50> polar(const boost::multiprecision::cpp_bin_float_50& rho, const boost::multiprecision::cpp_bin_float_50& theta) noexcept
{
    using std::sin;
    using std::cos;

    BOOST_MP_ASSERT_MSG(rho >= boost::multiprecision::cpp_bin_float_50{0}, "Rho must be positive");

    return {rho * cos(theta), rho * sin(theta)};
}

}

#endif //BOOST_MP_COMPLEX_HPP
