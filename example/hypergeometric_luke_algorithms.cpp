
///////////////////////////////////////////////////////////////////////////////
// Copyright Christopher Kormanyos 2013.
// Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469
//

#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>
#include <boost/chrono.hpp>

//#define USE_CPP_BIN_FLOAT
//#define USE_CPP_DEC_FLOAT
//#define USE_MPFR

#ifndef DIGIT_COUNT
#error "No precision specificied - set the macro DIGIT_COUNT to the number of decimal digits to test"
#endif

#if defined(USE_CPP_BIN_FLOAT) && defined(USE_CPP_DEC_FLOAT)
  #error the multiple precision type is ambiguous
#elif defined(USE_CPP_BIN_FLOAT)
  #include <boost/multiprecision/cpp_bin_float.hpp>
  typedef boost::multiprecision::number<boost::multiprecision::cpp_bin_float<DIGIT_COUNT> > mp_type;
#elif defined(USE_CPP_DEC_FLOAT)
  #include <boost/multiprecision/cpp_dec_float.hpp>
  typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<DIGIT_COUNT> > mp_type;
#elif defined(USE_MPFR)
  #include <boost/multiprecision/mpfr.hpp>
  typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<DIGIT_COUNT> > mp_type;
#else
  #error no multiple precision type is defined
#endif

#include <boost/math/constants/constants.hpp>
#include <boost/noncopyable.hpp>

template <class Clock>
struct stopwatch
{
   typedef typename Clock::duration duration;
   stopwatch()
   {
      m_start = Clock::now();
   }
   duration elapsed()
   {
      return Clock::now() - m_start;
   }
   void reset()
   {
      m_start = Clock::now();
   }

private:
   typename Clock::time_point m_start;
};

namespace my_math
{
  mp_type chebyshev_t(const std::int32_t n, const mp_type& x);
  mp_type chebyshev_u(const std::int32_t n, const mp_type& x);
  mp_type hermite    (const std::int32_t n, const mp_type& x);
  mp_type laguerre   (const std::int32_t n, const mp_type& x);
  mp_type legendre_p (const std::int32_t n, const mp_type& x);
  mp_type legendre_q (const std::int32_t n, const mp_type& x);

  mp_type chebyshev_t(const std::uint32_t n, const mp_type& x, std::vector<mp_type>* vp);
  mp_type chebyshev_u(const std::uint32_t n, const mp_type& x, std::vector<mp_type>* vp);
  mp_type hermite    (const std::uint32_t n, const mp_type& x, std::vector<mp_type>* vp);
  mp_type laguerre   (const std::uint32_t n, const mp_type& x, std::vector<mp_type>* vp);

  bool isneg(const mp_type& x) { return (x < mp_type(0)); }

  const mp_type& zero() { static const mp_type value_zero(0); return value_zero; }
  const mp_type& one () { static const mp_type value_one (1); return value_one; }
  const mp_type& two () { static const mp_type value_two (2); return value_two; }
}

namespace orthogonal_polynomial_series
{
  typedef enum enum_polynomial_type
  {
    chebyshev_t_type = 1,
    chebyshev_u_type = 2,
    laguerre_l_type  = 3,
    hermite_h_type   = 4
  }
  polynomial_type;

  template<typename T> static inline T orthogonal_polynomial_template(const T& x, const std::uint32_t n, const polynomial_type type, std::vector<T>* const vp = static_cast<std::vector<T>*>(0u))
  {
    // Compute the value of an orthogonal polynomial of one of the following types:
    // Chebyshev 1st, Chebyshev 2nd, Laguerre, or Hermite

    if(vp != static_cast<std::vector<T>*>(0u))
    {
      vp->clear();
      vp->reserve(static_cast<std::size_t>(n + 1u));
    }

    T y0 = my_math::one();

    if(vp != static_cast<std::vector<T>*>(0u))
    {
      vp->push_back(y0);
    }

    if(n == static_cast<std::uint32_t>(0u))
    {
      return y0;
    }
    
    T y1;

    if(type == chebyshev_t_type)
    {
      y1 = x;
    }
    else if(type == laguerre_l_type)
    {
      y1 = my_math::one() - x;
    }
    else
    {
      y1 = x * static_cast<std::uint32_t>(2u);
    }

    if(vp != static_cast<std::vector<T>*>(0u))
    {
      vp->push_back(y1);
    }

    if(n == static_cast<std::uint32_t>(1u))
    {
      return y1;
    }

    T a = my_math::two();
    T b = my_math::zero();
    T c = my_math::one();

    T yk;
  
    // Calculate higher orders using the recurrence relation.
    // The direction of stability is upward recurrence.
    for(std::int32_t k = static_cast<std::int32_t>(2); k <= static_cast<std::int32_t>(n); k++)
    {
      if(type == laguerre_l_type)
      {
        a = -my_math::one() / k;
        b =  my_math::two() + a;
        c =  my_math::one() + a;
      }
      else if(type == hermite_h_type)
      {
        c = my_math::two() * (k - my_math::one());
      }
      
      yk = (((a * x) + b) * y1) - (c * y0);
      
      y0 = y1;
      y1 = yk;

      if(vp != static_cast<std::vector<T>*>(0u))
      {
        vp->push_back(yk);
      }
    }

    return yk;
  }
}

mp_type my_math::chebyshev_t(const std::int32_t n, const mp_type& x)
{
  if(my_math::isneg(x))
  {
    const bool b_negate = ((n % static_cast<std::int32_t>(2)) != static_cast<std::int32_t>(0));

    const mp_type y = chebyshev_t(n, -x);

    return (!b_negate ? y : -y);
  }

  if(n < static_cast<std::int32_t>(0))
  {
    const std::int32_t nn = static_cast<std::int32_t>(-n);

    return chebyshev_t(nn, x);
  }
  else
  {
    return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::uint32_t>(n), orthogonal_polynomial_series::chebyshev_t_type);
  }
}

mp_type my_math::chebyshev_u(const std::int32_t n, const mp_type& x)
{
  if(my_math::isneg(x))
  {
    const bool b_negate = ((n % static_cast<std::int32_t>(2)) != static_cast<std::int32_t>(0));

    const mp_type y = chebyshev_u(n, -x);

    return (!b_negate ? y : -y);
  }

  if(n < static_cast<std::int32_t>(0))
  {
    if(n == static_cast<std::int32_t>(-2))
    {
      return my_math::one();
    }
    else if(n == static_cast<std::int32_t>(-1))
    {
      return my_math::zero();
    }
    else
    {
      const std::int32_t n_minus_two = static_cast<std::int32_t>(static_cast<std::int32_t>(-n) - static_cast<std::int32_t>(2));

      return -chebyshev_u(n_minus_two, x);
    }
  }
  else
  {
    return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::uint32_t>(n), orthogonal_polynomial_series::chebyshev_u_type);
  }
}

mp_type my_math::hermite(const std::int32_t n, const mp_type& x)
{
  if(n < static_cast<std::int32_t>(0))
  {
    // Negative order is not supported.
    return my_math::zero();
  }
  else
  {
    return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::uint32_t>(n), orthogonal_polynomial_series::hermite_h_type);
  }
}

mp_type my_math::laguerre(const std::int32_t n, const mp_type& x)
{
  if(n < static_cast<std::int32_t>(0))
  {
    // Negative order is not supported.
    return my_math::zero();
  }
  else if(n == static_cast<std::int32_t>(0))
  {
    return my_math::one();
  }
  else if(n == static_cast<std::int32_t>(1))
  {
    return my_math::one() - x;
  }

  if(my_math::isneg(x))
  {
    // Negative argument is not supported.
    return my_math::zero();
  }
  else
  {
    return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::uint32_t>(n), orthogonal_polynomial_series::laguerre_l_type);
  }
}

mp_type my_math::chebyshev_t(const std::uint32_t n, const mp_type& x, std::vector<mp_type>* const vp) { return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::int32_t>(n), orthogonal_polynomial_series::chebyshev_t_type, vp); }
mp_type my_math::chebyshev_u(const std::uint32_t n, const mp_type& x, std::vector<mp_type>* const vp) { return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::int32_t>(n), orthogonal_polynomial_series::chebyshev_u_type, vp); }
mp_type my_math::hermite    (const std::uint32_t n, const mp_type& x, std::vector<mp_type>* const vp) { return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::int32_t>(n), orthogonal_polynomial_series::hermite_h_type,   vp); }
mp_type my_math::laguerre   (const std::uint32_t n, const mp_type& x, std::vector<mp_type>* const vp) { return orthogonal_polynomial_series::orthogonal_polynomial_template(x, static_cast<std::int32_t>(n), orthogonal_polynomial_series::laguerre_l_type,  vp); }

namespace util
{
  template<typename T1,
           typename T2 = T1>
  struct alternating_sum
  {
  public:
    alternating_sum(const bool b_neg = false, const T2& init = T2(0)) : b_neg_term(b_neg),
                                                                        initial   (init) { }
                                                                          
    T1 operator()(const T1& sum, const T2& ck)
    {
      const T1 the_sum = (!b_neg_term ? T1(sum + ck) : T1(sum - ck));
      b_neg_term = !b_neg_term;
      return the_sum + initial;
    }

  private:
    bool b_neg_term;
    const T2 initial;

    const alternating_sum& operator=(const alternating_sum&);
  };

  template<typename T1,
           typename T2 = T1>
  struct point
  {
    T1 x;
    T2 y;

    point(const T1& X = T1(),
          const T2& Y = T2()) : x(X), y(Y) { }
  };

  template<typename T1,
           typename T2>
  inline bool operator<(const point<T1, T2>& left,
                        const point<T1, T2>& right)
  {
    return (left.x < right.x);
  }

  template<typename T1,
           typename T2 = T1>
  struct linear_interpolate
  {
    static T2 interpolate(const T1& x, const std::vector<util::point<T1, T2> >& points)
    {
      if(points.empty())
      {
        return T2(0);
      }
      else if(x <= points.front().x || points.size() == static_cast<std::size_t>(1u))
      {
        return points.front().y;
      }
      else if(x >= points.back().x)
      {
        return points.back().y;
      }
      else
      {
        const util::point<T1, T2> x_find(x);

        const typename std::vector<util::point<T1, T2> >::const_iterator it =
          std::lower_bound(points.begin(), points.end(), x_find);

        const T1 xn            = (it - 1u)->x;
        const T1 xnp1_minus_xn = it->x - xn;
        const T1 delta_x       = x - xn;
        const T2 yn            = (it - 1u)->y;
        const T2 ynp1_minus_yn = it->y - yn;

        return T2(yn + T2((delta_x * ynp1_minus_yn) / xnp1_minus_xn));
      }
    }
  };

  double digit_scale()
  {
    const std::array<util::point<double>, static_cast<std::size_t>(5U)> scale_data =
    {{
      util::point<double>(  50.0, 1.0 / 6.0),
      util::point<double>( 100.0, 1.0 / 3.0),
      util::point<double>( 200.0, 1.0 / 2.0),
      util::point<double>( 300.0, 1.0),
      util::point<double>(1000.0, 3.0 + (1.0 / 3.0)),
    }};

    const std::vector<util::point<double> > scale(scale_data.begin(), scale_data.end());

    const double the_scale = util::linear_interpolate<double>::interpolate(static_cast<double>(std::numeric_limits<mp_type>::digits10), scale);

    return the_scale;
  }
}

namespace examples
{
  namespace nr_006
  {
    template<typename T> class HypergPFQ_Base : private boost::noncopyable
    {
    protected:
      const   T       Z;
      const   mp_type W;
              std::int32_t   N;
      mutable std::deque<T> C;


    protected:

      HypergPFQ_Base(const T& z,
                     const mp_type& w) : Z(z),
                                         W(w),
                                         N(static_cast<std::int32_t>(util::digit_scale() * 500.0)),
                                         C(0u) { }

    public:

      virtual ~HypergPFQ_Base() { }

      virtual void ccoef(void) const = 0;

      virtual T series(void) const
      {
        using my_math::chebyshev_t;

        // Compute the Chebyshev coefficients.
        // Get the values of the shifted Chebyshev polynomials.
        std::vector<T> Tn_shifted;
        const T z_shifted = ((Z / W) * static_cast<std::int32_t>(2)) - static_cast<std::int32_t>(1);

        chebyshev_t(static_cast<std::uint32_t>(C.size()), z_shifted, &Tn_shifted);

        // Luke: C     ---------- COMPUTE SCALE FACTOR                       ----------
        // Luke: C
        // Luke: C     ---------- SCALE THE COEFFICIENTS                     ----------
        // Luke: C

        // The coefficient scaling is preformed after the Chebyshev summation,
        // and it is carried out with a single division operation.
        const T scale = std::accumulate(C.begin(), C.end(), T(0), util::alternating_sum<T>());

        // Compute the result of the series expansion using unscaled coefficients.
        const T sum = std::inner_product(C.begin(), C.end(), Tn_shifted.begin(), T(0));

        // Return the properly scaled result.
        return sum / scale;
      }
    };

    template<typename T> class Ccoef3Hyperg1F1 : public HypergPFQ_Base<T>
    {
    private:

      const T AP;
      const T CP;

    public:

      Ccoef3Hyperg1F1(const T& a, const T& c, const T& z, const mp_type& w) : HypergPFQ_Base<T>(z, w),
                                                                              AP(a),
                                                                              CP(c) { }

      virtual ~Ccoef3Hyperg1F1() { }

    public:

      virtual void ccoef(void) const
      {
        // See Luke 1977 page 74.
        const std::int32_t N1 = static_cast<std::int32_t>(HypergPFQ_Base<T>::N + static_cast<std::int32_t>(1));
        const std::int32_t N2 = static_cast<std::int32_t>(HypergPFQ_Base<T>::N + static_cast<std::int32_t>(2));

        // Luke: C     ---------- START COMPUTING COEFFICIENTS USING         ----------
        // Luke: C     ---------- BACKWARD RECURRENCE SCHEME                 ----------
        // Luke: C
        T A3(0);
        T A2(0);
        T A1(1);

        HypergPFQ_Base<T>::C.resize(1u, A1);

        std::int32_t X  = N1;
        std::int32_t X1 = N2;

        T XA  =  X + AP;
        T X3A = (X + 3) - AP;

        const T Z1 = T(4) / HypergPFQ_Base<T>::W;

        for(std::int32_t k = static_cast<std::int32_t>(0); k < N1; k++)
        {
          --X;
          --X1;
          --XA;
          --X3A;

          const T X3A_over_X2 = X3A / static_cast<std::int32_t>(X + 2);

          // The terms have been slightly re-arranged resulting in fewer multiplications.
          // Parentheses have been added to avoid reliance on operator precedence.
          const T PART1 =  A1 * (((X + CP) * Z1) - X3A_over_X2);
          const T PART2 =  A2 * (Z1 * ((X + 3) - CP) + (XA / X1));
          const T PART3 =  A3 * X3A_over_X2;

          const T term = (((PART1 + PART2) + PART3) * X1) / XA;

          HypergPFQ_Base<T>::C.push_front(term);

          A3 = A2;
          A2 = A1;
          A1 = HypergPFQ_Base<T>::C.front();
        }

        HypergPFQ_Base<T>::C.front() /= static_cast<std::int32_t>(2);
      }
    };

    template<typename T> class Ccoef6Hyperg1F2 : public HypergPFQ_Base<T>
    {
    private:

      const T AP;
      const T BP;
      const T CP;

    public:

      Ccoef6Hyperg1F2(const T& a,
                      const T& b,
                      const T& c,
                      const T& z,
                      const mp_type& w) : HypergPFQ_Base<T>(z, w),
                                          AP(a),
                                          BP(b),
                                          CP(c) { }

      virtual ~Ccoef6Hyperg1F2() { }

    public:

      virtual void ccoef(void) const
      {
        // See Luke 1977 page 85.
        const std::int32_t N1 = static_cast<std::int32_t>(HypergPFQ_Base<T>::N + static_cast<std::int32_t>(1));

        // Luke: C     ---------- START COMPUTING COEFFICIENTS USING         ----------
        // Luke: C     ---------- BACKWARD RECURRENCE SCHEME                 ----------
        // Luke: C
        T A4(0);
        T A3(0);
        T A2(0);
        T A1(1);

        HypergPFQ_Base<T>::C.resize(1u, A1);

        std::int32_t X  = N1;
        T     PP = X + AP;

        const T Z1 = T(4) / HypergPFQ_Base<T>::W;

        for(std::int32_t k = static_cast<std::int32_t>(0); k < N1; k++)
        {
          --X;
          --PP;

          const std::int32_t TWO_X    = static_cast<std::int32_t>(X * 2);
          const std::int32_t X_PLUS_1 = static_cast<std::int32_t>(X + 1);
          const std::int32_t X_PLUS_3 = static_cast<std::int32_t>(X + 3);
          const std::int32_t X_PLUS_4 = static_cast<std::int32_t>(X + 4);

          const T QQ = T(TWO_X + 3) / static_cast<std::int32_t>(TWO_X + static_cast<std::int32_t>(5));
          const T SS = (X + BP) * (X + CP);

          // The terms have been slightly re-arranged resulting in fewer multiplications.
          // Parentheses have been added to avoid reliance on operator precedence.
          const T PART1 =   A1 * (((PP - (QQ * (PP + 1))) * 2) + (SS * Z1));
          const T PART2 =  (A2 * (X + 2)) * ((((TWO_X + 1) * PP) / X_PLUS_1) - ((QQ * 4) * (PP + 1)) + (((TWO_X + 3) * (PP + 2)) / X_PLUS_3) + ((Z1 * 2) * (SS - (QQ * (X_PLUS_1 + BP)) * (X_PLUS_1 + CP))));
          const T PART3 =   A3 * ((((X_PLUS_3 - AP) - (QQ * (X_PLUS_4 - AP))) * 2) + (((QQ * Z1) * (X_PLUS_4 - BP)) * (X_PLUS_4 - CP)));
          const T PART4 = ((A4 * QQ) * (X_PLUS_4 - AP)) / X_PLUS_3;

          const T term = (((PART1 - PART2) + (PART3 - PART4)) * X_PLUS_1) / PP;

          HypergPFQ_Base<T>::C.push_front(term);

          A4 = A3;
          A3 = A2;
          A2 = A1;
          A1 = HypergPFQ_Base<T>::C.front();
        }

        HypergPFQ_Base<T>::C.front() /= static_cast<std::int32_t>(2);
      }
    };

    template<typename T> class Ccoef2Hyperg2F1 : public HypergPFQ_Base<T>
    {
    private:

      const T AP;
      const T BP;
      const T CP;

    public:

      Ccoef2Hyperg2F1(const T& a,
                      const T& b,
                      const T& c,
                      const T& z,
                      const mp_type& w) : HypergPFQ_Base<T>(z, w),
                                          AP(a),
                                          BP(b),
                                          CP(c)
      {
        // Set anew the number of terms in the Chebyshev expansion.
        HypergPFQ_Base<T>::N = static_cast<std::int32_t>(util::digit_scale() * 1600.0);
      }

      virtual ~Ccoef2Hyperg2F1() { }

    public:

      virtual void ccoef(void) const
      {
        // See Luke 1977 page 59.
        const std::int32_t N1 = static_cast<std::int32_t>(HypergPFQ_Base<T>::N + static_cast<std::int32_t>(1));
        const std::int32_t N2 = static_cast<std::int32_t>(HypergPFQ_Base<T>::N + static_cast<std::int32_t>(2));

        // Luke: C     ---------- START COMPUTING COEFFICIENTS USING         ----------
        // Luke: C     ---------- BACKWARD RECURRENCE SCHEME                 ----------
        // Luke: C
        T A3(0);
        T A2(0);
        T A1(1);

        HypergPFQ_Base<T>::C.resize(1u, A1);

        std::int32_t X  = N1;
        std::int32_t X1 = N2;
        std::int32_t X3  = static_cast<std::int32_t>((X * 2) + 3);

        T X3A = (X + 3) - AP;
        T X3B = (X + 3) - BP;

        const T Z1 = T(4) / HypergPFQ_Base<T>::W;

        for(std::int32_t k = static_cast<std::int32_t>(0); k < N1; k++)
        {
          --X;
          --X1;
          --X3A;
          --X3B;
          X3 -= 2;

          const std::int32_t X_PLUS_2 = static_cast<std::int32_t>(X + 2);

          const T XAB = T(1) / ((X + AP) * (X + BP));

          // The terms have been slightly re-arranged resulting in fewer multiplications.
          // Parentheses have been added to avoid reliance on operator precedence.
          const T PART1 = (A1 * X1) * (2 - (((AP + X1) * (BP + X1)) * ((T(X3) / X_PLUS_2) * XAB)) + ((CP + X) * (XAB * Z1)));
          const T PART2 = (A2 * XAB) * ((X3A * X3B) - (X3 * ((X3A + X3B) - 1)) + (((3 - CP) + X) * (X1 * Z1)));
          const T PART3 = (A3 * X1) * (X3A / X_PLUS_2) * (X3B * XAB);

          const T term = (PART1 + PART2) - PART3;

          HypergPFQ_Base<T>::C.push_front(term);

          A3 = A2;
          A2 = A1;
          A1 = HypergPFQ_Base<T>::C.front();
        }

        HypergPFQ_Base<T>::C.front() /= static_cast<std::int32_t>(2);
      }
    };

    mp_type luke_ccoef3_hyperg_1f1(const mp_type& a, const mp_type& b, const mp_type& x);
    mp_type luke_ccoef6_hyperg_1f2(const mp_type& a, const mp_type& b, const mp_type& c, const mp_type& x);
    mp_type luke_ccoef2_hyperg_2f1(const mp_type& a, const mp_type& b, const mp_type& c, const mp_type& x);
  }
}

mp_type examples::nr_006::luke_ccoef3_hyperg_1f1(const mp_type& a, const mp_type& b, const mp_type& x)
{
  const Ccoef3Hyperg1F1<mp_type> c3_h1f1(a, b, x, mp_type(-10));

  c3_h1f1.ccoef();

  return c3_h1f1.series();
}

mp_type examples::nr_006::luke_ccoef6_hyperg_1f2(const mp_type& a, const mp_type& b, const mp_type& c, const mp_type& x)
{
  const Ccoef6Hyperg1F2<mp_type> c6_h1f2(a, b, c, x, mp_type(-10));

  c6_h1f2.ccoef();

  return c6_h1f2.series();
}

mp_type examples::nr_006::luke_ccoef2_hyperg_2f1(const mp_type& a, const mp_type& b, const mp_type& c, const mp_type& x)
{
  const Ccoef2Hyperg2F1<mp_type> c2_h2f1(a, b, c, x, mp_type(-20));

  c2_h2f1.ccoef();

  return c2_h2f1.series();
}

int main()
{
  stopwatch<boost::chrono::high_resolution_clock> w;

  std::vector<mp_type> hypergeometric_2f1_results(20U);
  std::vector<mp_type> hypergeometric_1f2_results(20U);
  std::vector<mp_type> hypergeometric_1f1_results(20U);

  const mp_type a(mp_type(3) / 7);
  const mp_type b(mp_type(2) / 3);
  const mp_type c(mp_type(1) / 4);

  std::int_least16_t i;

  std::cout << "test hypergeometric_1f1." << std::endl;
  i = 1U;

  // Generate a table of values of Hypergeometric1F1.
  // Compare with the Mathematica command:
  // Table[N[HypergeometricPFQ[{3/7}, {2/3}, -(i*EulerGamma)], 100], {i, 1, 20, 1}]
  std::for_each(hypergeometric_1f1_results.begin(),
                hypergeometric_1f1_results.end(),
                [&i, &a, &b, &c](mp_type& new_value)
                {
                  const mp_type x(-(boost::math::constants::euler<mp_type>() * i));

                  new_value = examples::nr_006::luke_ccoef3_hyperg_1f1(a, b, x);

                  ++i;
                });

  // Print the values of Hypergeometric1F1.
  std::for_each(hypergeometric_1f1_results.begin(),
                hypergeometric_1f1_results.end(),
                [](const mp_type& h)
                {
                  std::cout << std::setprecision(std::numeric_limits<mp_type>::digits10 - 10)
                            << h
                            << std::endl;
                });

  std::cout << "test hypergeometric_1f2." << std::endl;
  i = 1U;

  // Generate a table of values of Hypergeometric1F2.
  // Compare with the Mathematica command:
  // Table[N[HypergeometricPFQ[{3/7}, {2/3, 1/4}, -(i*EulerGamma)], 100], {i, 1, 20, 1}]
  std::for_each(hypergeometric_1f2_results.begin(),
                hypergeometric_1f2_results.end(),
                [&i, &a, &b, &c](mp_type& new_value)
                {
                  const mp_type x(-(boost::math::constants::euler<mp_type>() * i));

                  new_value = examples::nr_006::luke_ccoef6_hyperg_1f2(a, b, c, x);

                  ++i;
                });

  // Print the values of Hypergeometric1F2.
  std::for_each(hypergeometric_1f2_results.begin(),
                hypergeometric_1f2_results.end(),
                [](const mp_type& h)
                {
                  std::cout << std::setprecision(std::numeric_limits<mp_type>::digits10 - 10)
                            << h
                            << std::endl;
                });

  std::cout << "test hypergeometric_2f1." << std::endl;
  i = 1U;

  // Generate a table of values of Hypergeometric2F1.
  // Compare with the Mathematica command:
  // Table[N[HypergeometricPFQ[{3/7, 2/3}, {1/4}, -(i * EulerGamma)], 100], {i, 1, 20, 1}]
  std::for_each(hypergeometric_2f1_results.begin(),
                hypergeometric_2f1_results.end(),
                [&i, &a, &b, &c](mp_type& new_value)
                {
                  const mp_type x(-(boost::math::constants::euler<mp_type>() * i));

                  new_value = examples::nr_006::luke_ccoef2_hyperg_2f1(a, b, c, x);

                  ++i;
                });

  // Print the values of Hypergeometric2F1.
  std::for_each(hypergeometric_2f1_results.begin(),
                hypergeometric_2f1_results.end(),
                [](const mp_type& h)
                {
                  std::cout << std::setprecision(std::numeric_limits<mp_type>::digits10 - 10)
                            << h
                            << std::endl;
                });
  double t = boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
  std::cout << "Total execution time = " << std::setprecision(3) << t << "s" << std::endl;
  return 0;
}
