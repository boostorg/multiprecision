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

    friend BOOST_MP_CXX14_CONSTEXPR complex operator+(const complex& lhs, const complex& rhs) noexcept
    {
        return {lhs.real_ + rhs.real_, lhs.imag_ + rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator+(const complex& lhs, const T& rhs) noexcept
    {
        return {lhs.real_ + rhs, lhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator+(const T& lhs, const complex& rhs) noexcept
    {
        return {lhs + rhs.real_, rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator-(const complex& lhs, const complex& rhs) noexcept
    {
        return {lhs.real_ - rhs.real_, lhs.imag_ - rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator-(const complex& lhs, const T& rhs) noexcept
    {
        return {lhs.real_ - rhs, lhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator-(const T& lhs, const complex& rhs) noexcept
    {
        return {lhs - rhs.real_, -rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator*(const complex& lhs, const complex& rhs) noexcept
    {
        return {lhs.real_ * rhs.real_ - lhs.imag_ * rhs.imag_, lhs.imag_ * rhs.real_ + lhs.real_ * rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator*(const complex& lhs, const T& rhs) noexcept
    {
        return {lhs.real_ * rhs, lhs.imag_ * rhs};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator*(const T& lhs, const complex& rhs) noexcept
    {
        return {lhs * rhs.real_, lhs * rhs.imag_};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator/(const complex& lhs, const complex& rhs) noexcept
    {
        const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
        const T real_part = (lhs.real_ * rhs.real_ + lhs.imag_ * rhs.imag_) / divisor;
        const T imag_part = (lhs.imag_ * rhs.real_ - lhs.real_ * rhs.imag_) / divisor;
        return {real_part, imag_part};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator/(const complex& lhs, const T& rhs) noexcept
    {
        const T divisor = rhs * rhs;
        const T real_part = (lhs.real_ * rhs) / divisor;
        const T imag_part = (lhs.imag_ * rhs) / divisor;
        return {real_part, imag_part};
    }

    friend BOOST_MP_CXX14_CONSTEXPR complex operator/(const T& lhs, const complex& rhs) noexcept
    {
        const T divisor = rhs.real_ * rhs.real_ + rhs.imag_ * rhs.imag_;
        const T real_part = (lhs * rhs.real_) / divisor;
        const T imag_part = -(lhs.real_ * rhs.imag_) / divisor;
        return {real_part, imag_part};
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator+=(const complex& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator+=(const T& rhs) noexcept
    {
        *this = *this + rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator-=(const complex& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator-=(const T& rhs) noexcept
    {
        *this = *this - rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator*=(const complex& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator*=(const T& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator/=(const complex& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR complex& operator/=(const T& rhs) noexcept
    {
        *this = *this / rhs;
        return *this;
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator==(const complex& rhs) const noexcept
    {
        return real_ == rhs.real_ && imag_ == rhs.imag_;
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator!=(const complex& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator==(const T& rhs) const noexcept
    {
        return real_ == rhs && imag_ == T{0};
    }

    BOOST_MP_CXX14_CONSTEXPR bool operator!=(const T& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const complex& z)
    {
        std::basic_ostringstream<CharT, Traits> s;
        s.flags(os.flags());
        s.imbue(os.getloc());
        s.precision(os.precision());
        s << '(' << z.real_ << ',' << z.imag_ << ')';

        return os << s.str();
    }

    // Supported formats:
    // 1) real
    // 2) (real)
    // 3) (real, imag)
    template <typename CharT, typename Traits>
    friend std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, complex& z)
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
            z.real_ = real;
            z.imag_ = imag;
        }

        return is;
    }
};

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> polar(const boost::multiprecision::number<T, ET>& rho,
                                                                                    const boost::multiprecision::number<T, ET>& theta) noexcept
{
    BOOST_MP_ASSERT_MSG(rho >= 0, "Rho must be positive");
    return {rho * cos(theta), rho * sin(theta)};
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

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> proj(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    if (isinf(z.real()) || isinf(z.imag()))
    {
        return {std::numeric_limits<boost::multiprecision::number<T, ET>>::infinity(), copysign(boost::multiprecision::number<T, ET>{0}, z.imag())};
    }

    return z;
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> exp(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return polar(exp(z.real()), z.imag());
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> log(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return {log(abs(z)), arg(z)};
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> log10(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return log(z) / log(boost::multiprecision::number<T, ET>{10});
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> pow(const complex<boost::multiprecision::number<T, ET>>& x,
                                                                                  const complex<boost::multiprecision::number<T, ET>>& y) noexcept
{
    return exp(y * log(x));
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> pow(const boost::multiprecision::number<T, ET>& x,
                                                                                  const complex<boost::multiprecision::number<T, ET>>& y) noexcept
{
    const complex<boost::multiprecision::number<T, ET>> new_x {x};
    return exp(y * log(new_x));
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> pow(const complex<boost::multiprecision::number<T, ET>>& x,
                                                                                  const boost::multiprecision::number<T, ET>& y) noexcept
{
    const complex<boost::multiprecision::number<T, ET>> new_y {y};
    return exp(new_y * log(x));
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> sqrt(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return polar(sqrt(abs(z)), arg(z) / 2);
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> sinh(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return {sinh(z.real()) * cos(z.imag()), cosh(z.real()) * sin(z.imag())};
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> cosh(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return {cosh(z.real()) * cos(z.imag()), sinh(z.real()) * sin(z.imag())};
}

template <typename T, expression_template_option ET>
inline BOOST_MP_CXX14_CONSTEXPR complex<boost::multiprecision::number<T, ET>> tanh(const complex<boost::multiprecision::number<T, ET>>& z) noexcept
{
    return {sinh(z) / cosh(z)};
}
*/
} // Namespace multiprecision
} // Namespace boost

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
