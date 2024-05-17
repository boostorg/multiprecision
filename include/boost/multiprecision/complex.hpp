///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMPLEX_HPP
#define BOOST_MP_COMPLEX_HPP

#include <boost/multiprecision/detail/assert.hpp>
#include <boost/multiprecision/traits/is_backend.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/fwd.hpp>
#include <utility>
#include <ostream>
#include <istream>
#include <complex>
#include <cmath>

namespace std {

template <typename T, boost::multiprecision::expression_template_option ET>
class complex<boost::multiprecision::number<T, ET>>
{
private:
    using float_type = boost::multiprecision::number<T, ET>;
    using self_type = complex<boost::multiprecision::number<T, ET>>;
    using complex_type = decltype(boost::multiprecision::polar(std::declval<boost::multiprecision::number<T, ET>>(), std::declval<boost::multiprecision::number<T, ET>>()));

    complex_type m_data_;

public:
    complex() = default;
    explicit complex(float_type real) : m_data_ {real} {}
    complex(float_type real, float_type imag) : m_data_ {real, imag} {}
    complex(const complex_type& data) : m_data_ {data} {}
    complex(complex_type&& data) : m_data_ {data} {}

    complex_type& data() { return m_data_; }
    const complex_type& data() const { return m_data_; }

    float_type real() const { return m_data_.real(); }
    float_type imag() const { return m_data_.imag(); }

    self_type operator+() const { return *this; }
    self_type operator-() const { return {-m_data_.real(), -m_data_.imag()}; }

    friend self_type operator+(const self_type& lhs, const self_type& rhs)
    {
        return {lhs.m_data_ + rhs.m_data_};
    }

    friend self_type operator+(const self_type& lhs, const float_type& rhs)
    {
        return {lhs.m_data_ + rhs};
    }

    friend self_type operator+(const float_type& lhs, const self_type& rhs)
    {
        return {lhs + rhs.m_data_};
    }

    friend self_type operator-(const self_type& lhs, const self_type& rhs)
    {
        return {lhs.m_data_ - rhs.m_data_};
    }

    friend self_type operator-(const self_type& lhs, const float_type& rhs)
    {
        return {lhs.m_data_ - rhs};
    }

    friend self_type operator-(const float_type& lhs, const self_type& rhs)
    {
        return {lhs - rhs.m_data_};
    }

    friend self_type operator*(const self_type& lhs, const self_type& rhs)
    {
        return {lhs.m_data_ * rhs.m_data_};
    }

    friend self_type operator*(const self_type& lhs, const float_type& rhs)
    {
        return {lhs.m_data_ * rhs};
    }

    friend self_type operator*(const float_type& lhs, const self_type& rhs)
    {
        return {lhs * rhs.m_data_};
    }

    friend self_type operator/(const self_type& lhs, const self_type& rhs)
    {
        return {lhs.m_data_ / rhs.m_data_};
    }

    friend self_type operator/(const self_type& lhs, const float_type& rhs)
    {
        return {lhs.m_data_ / rhs};
    }

    friend self_type operator/(const float_type& lhs, const self_type& rhs)
    {
        return {lhs / rhs.m_data_};
    }

    self_type& operator+=(const self_type& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    self_type& operator+=(const float_type& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    self_type& operator-=(const self_type& rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    self_type& operator-=(const float_type& rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    self_type& operator*=(const self_type& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    self_type& operator*=(const float_type& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    self_type& operator/=(const self_type& rhs)
    {
        *this = *this / rhs;
        return *this;
    }

    self_type& operator/=(const float_type& rhs)
    {
        *this = *this / rhs;
        return *this;
    }

    bool operator==(const self_type& rhs) const
    {
        return this->m_data_ == rhs.m_data_;
    }

    bool operator!=(const self_type& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator==(const float_type& rhs) const
    {
        return this->real() == rhs && this->imag() == float_type{0};
    }

    bool operator!=(const float_type& rhs) const
    {
        return !(*this == rhs);
    }

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const self_type& z)
    {
        std::basic_ostringstream<CharT, Traits> s;
        s.flags(os.flags());
        s.imbue(os.getloc());
        s.precision(os.precision());
        s << '(' << z.real() << ',' << z.imag() << ')';

        return os << s.str();
    }

    // Supported formats:
    // 1) real
    // 2) (real)
    // 3) (real, imag)
    template <typename CharT, typename Traits>
    friend std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, self_type& z)
    {
        CharT ch {};
        float_type real {};
        float_type imag {};

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
            z = self_type{real, imag};
        }

        return is;
    }
};

template <typename T, boost::multiprecision::expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> polar(const boost::multiprecision::number<T, ET>& a, const boost::multiprecision::number<T, ET>& b)
{
   return { boost::multiprecision::polar(a, b) };
}

} // namespace std

namespace boost {
namespace multiprecision {

template <typename T, expression_template_option ET>
inline boost::multiprecision::number<T, ET> real(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return z.real();
}

template <typename T, expression_template_option ET>
inline boost::multiprecision::number<T, ET> imag(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return z.imag();
}

template <typename T, expression_template_option ET>
inline boost::multiprecision::number<T, ET> abs(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return abs(z.data());
}

template <typename T, expression_template_option ET>
inline boost::multiprecision::number<T, ET> arg(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return arg(z.data());
}

template <typename T, expression_template_option ET>
inline boost::multiprecision::number<T, ET> norm(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return norm(z.data());
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> conj(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {conj(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> proj(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {proj(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> exp(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {exp(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> log(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {log(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> log10(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {log10(z.data())};
}

template <typename T, expression_template_option ET>
inline  std::complex<boost::multiprecision::number<T, ET>> pow(const std::complex<boost::multiprecision::number<T, ET>>& x,
                                                               const std::complex<boost::multiprecision::number<T, ET>>& y)
{
    return {pow(x.data(), y.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> pow(const boost::multiprecision::number<T, ET>& x,
                                                              const std::complex<boost::multiprecision::number<T, ET>>& y)
{
    return {pow(x, y.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> pow(const std::complex<boost::multiprecision::number<T, ET>>& x,
                                                              const boost::multiprecision::number<T, ET>& y)
{
    return {pow(x.data(), y)};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> sqrt(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {sqrt(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> sin(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {sin(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> cos(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {cos(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> tan(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {tan(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> asin(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {asin(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> acos(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {acos(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> atan(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {atan(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> sinh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {sinh(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> cosh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {cosh(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> tanh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {tanh(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> asinh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {asinh(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> acosh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {acosh(z.data())};
}

template <typename T, expression_template_option ET>
inline std::complex<boost::multiprecision::number<T, ET>> atanh(const std::complex<boost::multiprecision::number<T, ET>>& z)
{
    return {atanh(z.data())};
}

} // namespace MP
} // namespace boost

#endif //BOOST_MP_COMPLEX_HPP
