///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMPLEX_HPP
#define BOOST_MP_COMPLEX_HPP

#include <boost/multiprecision/detail/assert.hpp>
#include <boost/multiprecision/traits/is_backend.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/fwd.hpp>
#include <ostream>
#include <istream>
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
    BOOST_MP_CXX14_CONSTEXPR complex() noexcept = default;
    BOOST_MP_CXX14_CONSTEXPR complex(T real) noexcept : real_ {real}, imag_ {T{0}} {}
    BOOST_MP_CXX14_CONSTEXPR complex(T real, T imag) noexcept : real_ {real}, imag_ {imag} {}
    BOOST_MP_CXX14_CONSTEXPR complex& operator=(const complex& rhs)
    {
        real_ = rhs.real_;
        imag_ = rhs.imag_;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR T real() const noexcept { return real_; }
    BOOST_MP_CXX14_CONSTEXPR T imag() const noexcept { return imag_; }

    BOOST_MP_CXX14_CONSTEXPR complex operator+() const { return *this; }
    BOOST_MP_CXX14_CONSTEXPR complex operator-() const { return {-real_, -imag_}; }

    BOOST_MP_CXX14_CONSTEXPR complex operator+(const complex& rhs) noexcept
    {
        return {real_ + rhs.real_, imag_ + rhs.imag_};
    }

    BOOST_MP_CXX14_CONSTEXPR complex operator-(const complex& rhs) noexcept
    {
        return {real_ - rhs.real_, imag_ - rhs.imag_};
    }

    BOOST_MP_CXX14_CONSTEXPR complex operator*(const complex& rhs) noexcept
    {
        return {real_ * rhs.real_ - imag_ * rhs.imag_, imag_ * rhs.real_ + real_ * rhs.imag_};
    }

    BOOST_MP_CXX14_CONSTEXPR complex operator/(const complex& rhs) noexcept
    {
        const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
        const T real_part = (real_ * rhs.real_ + imag_ * rhs.imag_) / divisor;
        const T imag_part = (imag_ * rhs.real_ - real_ * rhs.imag_) / divisor;
        return {real_part, imag_part};
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator+=(const complex& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator-=(const complex& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator*=(const complex& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator/=(const complex& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator==(const complex& rhs) noexcept
    {
        return real_ == rhs.real_ && imag_ == rhs.imag_;
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator!=(const complex& rhs) noexcept
    {
        return !(*this == rhs);
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator==(const T& rhs) noexcept
    {
        return real_ == rhs && imag_ == T{0};
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator!=(const T& rhs) noexcept
    {
        return !(*this == rhs);
    }

    // Writes in the form (real, imag)
    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os)
    {
        std::basic_ostringstream<CharT, Traits> s;
        s.flags(os.flags());
        s.imbue(os.getloc());
        s.precision(os.precision());
        s << '(' << real_ << ',' << imag_ << ')';

        return os << s.str();
    }

    // Supported formats:
    // 1) real
    // 2) (real)
    // 3) (real, imag)
    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits>& operator>>(std::basic_ostream<CharT, Traits>& is)
    {
        CharT ch {};
        T real = T{0};
        T imag = T{0};

        is >> std::ws;
        is.get(ch);

        if (ch == '(')
        {
            // Expecting a format like 2 or 3
            is >> std::ws >> real;
            is.get(ch);
            if (ch == ',')
            {
                // A comma indicates it's 3
                is >> std::ws >> imag;
                is.get(ch); // Should be ')'
            }
            else if (ch != ')')
            {
                // Syntax error: unexpected character
                is.setstate(std::ios_base::failbit);
                return is;
            }
        }
        else
        {
            // No parentheses, just a real number from format 1
            is.putback(ch);
            is >> real;
        }

        if (!is.fail())
        {
            real_ = real;
            imag_ = imag;
        }

        return is;
    }
};

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> polar(const boost::multiprecision::number<T, ET>& rho,
                                                                                    const boost::multiprecision::number<T, ET>& theta) noexcept
{
    BOOST_MP_ASSERT_MSG(rho >= T{0}, "Rho must be positive");
    return polar(rho, theta);
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR boost::multiprecision::number<T, ET> real(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return z.real();
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR boost::multiprecision::number<T, ET> imag(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return z.imag();
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR boost::multiprecision::number<T, ET> abs(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return hypot(z.real(), z.imag());
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR boost::multiprecision::number<T, ET> arg(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return atan2(z.imag(), z.real());
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR boost::multiprecision::number<T, ET> norm(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return z.real() * z.real() + z.imag() * z.imag();
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> conj(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return {z.real(), -z.imag()};
}

} // Namespace multiprecision
} // Namespace boost

namespace std {

template <typename T, boost::multiprecision::expression_template_option ET>
class complex<boost::multiprecision::number<T, ET>> : public boost::multiprecision::complex<boost::multiprecision::number<T, ET>> {};

} // namespace std

#endif //BOOST_MP_COMPLEX_HPP
