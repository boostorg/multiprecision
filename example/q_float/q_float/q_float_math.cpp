///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include "q_float_qf.h"

/// ---------------------------------------------------------------------------
/// \func     q_float qf::pow2(const INT32 p)
///
/// \brief    Compute 2 raised to the power of p for positive or negative p.
///           Binary splitting of the power is used. The resulting computational
///           complexity scales with log2(p). There is a simple check for overflow.
/// ---------------------------------------------------------------------------
q_float qf::pow2(const INT32 p)
{
  if(   p > std::numeric_limits<q_float>::max_exponent
     || p < std::numeric_limits<q_float>::min_exponent
    )
  {
    throw qf::exception_nan();
    return std::numeric_limits<q_float>::quiet_NaN();
  }
  
  if(p < 0)
  {
    return qf::pow2(-p).inv();
  }
  else if(p == 0)
  {
    return qf::one();
  }
  else if(p == 1)
  {
    return qf::two();
  }
  else
  {
    // Constant sequence pn[n] = p^1, p^2, p^4, p^8, ... with n = 1...9
    static const q_float pn[] =
    {
      qf::two(),
      q_float(pn[ 0]) * pn[ 0],
      q_float(pn[ 1]) * pn[ 1],
      q_float(pn[ 2]) * pn[ 2],
      q_float(pn[ 3]) * pn[ 3],
      q_float(pn[ 4]) * pn[ 4],
      q_float(pn[ 5]) * pn[ 5],
      q_float(pn[ 6]) * pn[ 6],
      q_float(pn[ 7]) * pn[ 7],
      q_float(pn[ 8]) * pn[ 8]
    };
  
    q_float val(qf::one());

    for(UINT32 i = 0; i < sizeof(pn) / sizeof(pn[0]); i++)
    {
      if((static_cast<UINT32>(p) >> i) & 1)
      {
        val *= pn[i];
      }
    }

    return val;
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::pow10(const INT32 p)
///
/// \brief    Compute 10 raised to the power of p for positive or negative p.
///           The so-called ladder method is used. At the top of the subroutine,
///           there is a simple check for overflow.
/// ---------------------------------------------------------------------------
q_float qf::pow10(const INT32 p)
{
  if(   p > std::numeric_limits<q_float>::max_exponent10
     || p < std::numeric_limits<q_float>::min_exponent10
    )
  {
    throw qf::exception_nan();
    return std::numeric_limits<q_float>::quiet_NaN();
  }

  q_float result;

  if     (p <  0) { result = qf::pow10(-p).inv(); }
  else if(p == 0) { result = qf::one(); }
  else if(p == 1) { result = qf::ten(); }
  else if(p == 2) { result = q_float(100U); }
  else if(p == 3) { result = q_float(1000U); }
  else if(p == 4) { result = q_float(10000U); }
  else
  {
    result = q_float(qf::one());

    q_float y(qf::ten());

    UINT32 p_local = (UINT32) p;

    for(;;)
    {
      if(std::uint_fast8_t(p_local & 1U) != 0U)
      {
        result *= y;
      }

      p_local >>= 1U;

      if  (p_local == 0U) { break; }
      else                { y *= y; }
    }
  }

  return result;
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::pown(const q_float& x, const INT32 p)
///
/// \brief    Compute x raised to the power of p for positive or negative p.
///           Binary splitting of the power is used. The resulting computational
///           complexity scales with log2(p). There is no check for overflow.
/// ---------------------------------------------------------------------------
q_float qf::pown(const q_float& x, const INT32 p)
{
  // Normal argument?
  if(!x.is_nrm())
  {
    return x;
  }

  if(p < 0)
  {
    return pown(x, -p).inv();
  }
  else
  {
    switch(p)
    {
      case 0:
        return qf::one();

      case 1:
        return x;

      case 2:
        return q_float(x) * x;

      case 3:
      {
        q_float value(x);
        value *= value;
        return value * x;
      }

      case 4:
      {
        q_float value(x);
        value *= value;
        return value * value;
      }

      default:
      {
        q_float value(x);
        INT32 n;
        for(n = 1; n <= p / 2; n*= 2)
        {
          value *= value;
        }

        // Call the function recursively for computing the remaining pown.
        return p - n == 0 ? value : value * qf::pown(x, p - n);
      }
    }
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::exp(const q_float& x)
///
/// \brief    Returns the exponent of q_float x.
///           The algorithm for exp has been taken from MPFUN.
///           exp(t) = [ exp(r)^p2 ] * 2^n where p2 is a power of 2,
///           such as 256, and r = t_prime / p2, and t_prime = t - n*ln2,
///           with n chosen to minimize the absolute value of t_prime.
///           In the resulting small-argument calculation, |r| is bounded
///           by ln2 / p2. For small arguments, no scaling is done.
///           The subsequent series expansion is done using a Pade
///           approximation of Order[6] computed from Mathematica.
/// ---------------------------------------------------------------------------
q_float qf::exp(const q_float& x)
{
  // Normal argument?
  if(!x.is_nrm())
  {
    return x;
  }

  // Get a local copy of the argument and force it to be positive.
  const bool b_neg = x.is_neg();
  q_float xx(!b_neg ? x : -x);

  // Check the range of the input. Will it overflow?
  static const q_float max_exp_input(std::log(qf::to_double((std::numeric_limits<q_float>::max)())));
  static const q_float min_exp_input(std::log(qf::to_double((std::numeric_limits<q_float>::min)())));

  if(xx.is_zero() || xx < min_exp_input)
  {
    return qf::one();
  }

  if(xx > max_exp_input)
  {
    return qf::NaN();
  }

  if(xx.is_one())
  {
    static const q_float one_over_e = q_float(e()).inv();
    return (!b_neg ? e() : one_over_e);
  }

  // Use an argument reduction algorithm for exp() in classic MPFUN-style.
  static const q_float one_over_ln2 = q_float(ln2()).inv();
  const q_float nf = qf::floor(xx * one_over_ln2);

  // Prepare the scaled variables.
  static const INT32 p2 = 256;
  const bool b_scale    = xx.order() > -4;
  const q_float r       = b_scale ? (xx - nf * ln2()) / p2 : xx;
  const q_float r2      = r * r;
  const q_float r4      = r2 * r2;

  // Use a Pade approximation of Order[6].
  // Pade[Exp[r], {r, 0, 6, 6}]
  // FullSimplify[%]

  static const q_float n84  (  84);
  static const q_float n240 ( 240);
  static const q_float n7920(7920);
  
  static const q_float n665280(665280);
  static const q_float n332640(332640);
  static const q_float n75600 ( 75600);
  static const q_float n10080 ( 10080);
  static const q_float n840   (   840);
  static const q_float n42    (    42);

  xx = qf::one() +   (n84 * r * (n7920 + n240 * r2 + r4))
                   / (n665280 + r * (-n332640 + r * (n75600 + r * (-n10080 + r * (n840 + (-n42 + r) * r)))));

  // Rescale the result.
  if(b_scale)
  {
    xx  = qf::pown(xx, p2);
    xx *= qf::pow2(static_cast<INT32>(qf::to_int64(nf)));
  }

  return !b_neg ? xx : xx.inv();
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::log(const q_float& x)
///
/// \brief    Returns the logarithm of q_float x.
///           For arguments near 1, use a Pade approximation of Order[6],
///           otherwise use one single step of Newton-Raphson iteration.
/// ---------------------------------------------------------------------------
q_float qf::log(const q_float& x)
{
  if(!x.is_nrm())
  {
    return x;
  }
  
  if(x.is_neg())
  {
    return qf::NaN();
  }
  
  if(x.is_one())
  {
    return qf::zero();
  }
  
  if(x.near_one())
  {
    // For arguments near 1, use a Pade approximation of Order[6].
    // Calculated from Mathematica Version 4.1 using the following:
    // 1) In[1]:= << Calculus`Pade`;
    // 2) In[2]:= Pade[Log[x], {x, 1, 6, 6}]
    // 3) Out[2]= ..... [Mathematica output]
    // 4) In[3]:= FullSimplify[%]
    // 5) Out[3]= ..... [Mathematica output]
    //
    // The Mathematica output is:
    //
    // 7(-1 + x)(1 + x)(7 + x(132 + x(382 + x(132 + 7x))))
    // ----------------------------------------------------
    // 10(1 + x(36 + x(225 + x(400 + x(225 + x(36 + x))))))
    //
    static const q_float n7(7), n10(10), n36(36), n132(132), n225(225), n382(382), n400(400);

    const q_float numerator   = ((x + qf::one()) * (x * (x * (x * (x * n7 + n132) + n382) + n132) + n7)) * n7;
          q_float denominator = (x * (x * (x * (x * (x * (x + n36) + n225) + n400) + n225) + n36) + qf::one()) * n10;

    return (numerator / denominator) * (x - qf::one());
  }
  else
  {
    using std::log;

    // Get initial estimate using the standard math function log.
    const q_float s(log(qf::to_double(x)));
    const q_float E = qf::exp(s);

    // Do one single step of Newton-Raphson iteration
    return s + (x - E) / E;
  }
}

/// ---------------------------------------------------------------------------
/// \func     void qf::sincos(const q_float& x, q_float* p_sin, q_float* p_cos)
///
/// \brief    Computation of sin and/or cos of q_float x. If the pointer
///           p_sin or p_cos is zero, then no value is retrieved.
/// ---------------------------------------------------------------------------
void qf::sincos(const q_float& x, q_float* p_sin, q_float* p_cos)
{
  if(!x.is_nrm() || (!p_sin && !p_cos))
  {
    return;
  }

  // Local copy of the argument
  q_float xx(x);
  
  // Analyze and prepare the phase of the argument.
  // Make a local, positive copy of the argument, xx.
  // The argument xx will be reduced to 0 <= xx <= pi/2.
  bool b_negate_sin = false;
  bool b_negate_cos = false;

  if(x.is_neg())
  {
    xx = -xx;
    b_negate_sin = !b_negate_sin;
  }

  // Remove even multiples of pi.
  if(xx > qf::pi())
  {
    q_float n_pi = qf::floor(xx / qf::pi());
    xx -= n_pi * qf::pi();

    // Adjust signs if the multiple of pi is not even.
    if(qf::to_int64(n_pi) % 2)
    {
      b_negate_sin = !b_negate_sin;
      b_negate_cos = !b_negate_cos;
    }
  }

  // Reduce the argument to 0 <= xx <= pi/2.
  if(xx > qf::pi_half())
  {
    xx = qf::pi() - xx;
    b_negate_cos = !b_negate_cos;
  }
  
  const bool b_zero    = xx.is_zero();
  const bool b_pi_half = xx == qf::pi_half();

  if(b_zero)
  {
    if(p_sin)
    {
      *p_sin = qf::zero();
    }
    if(p_cos)
    {
      *p_cos = !b_negate_cos ? qf::one() : -qf::one();
    }
  }
  else if(b_pi_half)
  {
    if(p_sin)
    {
      *p_sin = !b_negate_sin ? qf::one() : -qf::one();
    }
    if(p_cos)
    {
      *p_cos = qf::zero();
    }
  }
  else
  {
    // Scale to a small argument for an efficient Taylor series.
    // Use a standard divide by three identity a certain number
    // of times. Here we use division by 3^5 --> (81 = 3^5).
    const bool  b_scale = xx.order() > -4;
    const INT32 n_scale = 5;
    if(b_scale)
    {
      xx.div_by_int(243);
    }

    // Establish the tolerance for the Taylor series expansion.
    const INT32 target = xx.order() - std::numeric_limits<q_float>::digits10 - 4;

    // Now with small arguments, we are ready for a series expansion.
    // Compute the first term of the sin series: x
    q_float term(xx);
    q_float sin_val(term);

    // Check if a Taylor expansion is necessary.
    if(term.is_zero() || term.order() < -std::numeric_limits<q_float>::digits10)
    {
    }
    else
    {
      // Taylor expansion for sin.
      const q_float x2(xx * xx);
      bool b_neg_term = true;
      INT32 k = 3;
      do
      {
        term *= x2;
        term.div_by_int(k * (k - 1));

        b_neg_term ? sin_val -= term : sin_val += term;
        b_neg_term = !b_neg_term;

        k += 2;
      }
      while(term.order() > target);
    }
    
    // Convert back using multiple angle identity.
    if(b_scale)
    {
      for(INT32 k = 0; k < n_scale; k++)
      {
        // Rescale sin part
        q_float sum1 = sin_val;
        q_float sum3 = sin_val * sin_val * sin_val;

        sin_val  = sum1.mul_by_int(3);
        sin_val -= sum3.mul_by_int(4);
      }
    }

    if(p_sin)
    {
      *p_sin = sin_val;
      if(b_negate_sin)
      {
        *p_sin = -*p_sin;
      }
    }

    if(p_cos)
    {
      *p_cos = qf::sqrt(qf::one() - sin_val * sin_val);
      if(b_negate_cos)
      {
        *p_cos = -*p_cos;
      }
    }
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::sin(const q_float& x)
///
/// \brief    Returns the sin of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::sin(const q_float& x)
{
  q_float s(x);
  qf::sincos(x, &s, NULL);
  return s;
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::cos(const q_float& x)
///
/// \brief    Returns the cos of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::cos(const q_float& x)
{
  q_float c(x);
  qf::sincos(x, NULL, &c);
  return c;
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::tan(const q_float& x)
///
/// \brief    Returns the tan of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::tan(const q_float& x)
{
  q_float s(x), c(x);
  qf::sincos(x, &s, &c);
  return s * c.inv();
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::asin(const q_float& x)
///
/// \brief    Returns the asin of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::asin(const q_float& x)
{
  // Normal argument?
  if(!x.is_nrm())
  {
    return x;
  }

  const bool b_neg = x.is_neg();
  q_float xx(!b_neg ? x : -x);

  if(xx > qf::one())
  {
    return qf::NaN();
  }
  else if(x.is_zero())
  {
    return qf::zero();
  }
  
  if(xx.is_one())
  {
    return !b_neg ? qf::pi_half() : -qf::pi_half();
  }
  
  // Get initial estimate using standard math function asin.
  q_float value(std::asin(qf::to_double(xx)));

  // Do one single step of Newton-Raphson iteration
  q_float s, c;
  qf::sincos(value, &s, &c);
  value -= (s - xx) / c;

  return ((b_neg == false) ? value : -value);
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::acos(const q_float& x)
///
/// \brief    Returns the acos of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::acos(const q_float& x)
{
  // Normal argument?
  if(!x.is_nrm())
  {
    return x;
  }

  return x.is_zero() ? qf::pi_half() : (qf::fabs(x) > qf::one() || !x.is_nrm() ? qf::NaN() : qf::pi_half() - qf::asin(x));
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::atan(const q_float& x)
///
/// \brief    Returns the atan of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::atan(const q_float& x)
{
  // Normal argument?
  if(!x.is_nrm())
  {
    return x;
  }

  const bool b_neg = x.is_neg();
  
  q_float xx((b_neg == false) ? x : -x);
  
  // Get initial estimate using standard math function atan.
  q_float value(std::atan(qf::to_double(xx)));

  // Do one single step of Newton-Raphson iteration
  q_float s, c;
  qf::sincos(value, &s, &c);
  value += c * (xx * c - s);

  return ((b_neg == false) ? value : -value);
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::atan2(const q_float& y, const q_float& x)
///
/// \brief    Returns the atan of y / x.
/// ---------------------------------------------------------------------------
q_float qf::atan2(const q_float& y, const q_float& x)
{
  // Normal arguments?
  if(!x.is_nrm() || !y.is_nrm())
  {
    return x;
  }

  // y is zero
  if(y.is_zero())
  {
    return x.is_neg() ? qf::pi() : qf::zero();
  }

  // x is zero
  if(x.is_zero())
  {
    return qf::sgn(y) * qf::pi_half();
  }
  
  // y is infinite
  if(y.is_inf())
  {
    return qf::inf();
  }

  // Compute atan(y / x), ignoring sign.
  q_float atan_term(qf::atan(y / x));

  // Determine quadrant (sign) based on signs of x, y
  const bool y_neg = y.is_neg();
  const bool x_neg = x.is_neg();
  
  if(y_neg == x_neg)
  {
    // Both negative or both positive
    return x_neg ? atan_term - qf::pi() : atan_term;
  }
  else
  {
    // Different signs of x, y
    return x_neg ? atan_term + qf::pi() : atan_term;
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::sinh(const q_float& x)
///
/// \brief    Returns the sinh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::sinh(const q_float& x)
{
  q_float s(x);
  qf::sinhcosh(x, &s, NULL);
  return s;
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::cosh(const q_float& x)
///
/// \brief    Returns the cosh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::cosh(const q_float& x)
{
  q_float c(x);
  qf::sinhcosh(x, NULL, &c);
  return c;
}

/// ---------------------------------------------------------------------------
/// \func     void qf::sinhcosh(const q_float& x, q_float* p_sinh, q_float* p_cosh)
///
/// \brief    Computation of sinh and/or cosh of q_float x. If the pointer
///           p_sinh or p_cosh is zero, then no value is retrieved.
/// ---------------------------------------------------------------------------
void qf::sinhcosh(const q_float& x, q_float* p_sinh, q_float* p_cosh)
{
  if(!x.is_nrm() || (!p_sinh && !p_cosh))
  {
    return;
  }
  
  if(x.is_zero())
  {
    if(p_sinh)
    {
      *p_sinh = qf::zero();
    }

    if(p_cosh)
    {
      *p_cosh = qf::one();
    }
    
    return;
  }

  const q_float e_px = qf::exp(x);
  const q_float e_mx = q_float(e_px).inv();

  if(p_sinh)
  {
    *p_sinh  = e_px;
    *p_sinh -= e_mx;
    p_sinh->div_by_int(2);
  }

  if(p_cosh)
  {
    *p_cosh  = e_px;
    *p_cosh += e_mx;
    p_cosh->div_by_int(2);
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::tanh(const q_float& x)
///
/// \brief    Returns the tanh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::tanh(const q_float& x)
{
  q_float c(x), s(x);
  qf::sinhcosh(x, &s, &c);

  return s / c;
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::asinh(const q_float& x)
///
/// \brief    Returns the asinh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::asinh(const q_float& x)
{
  if(!x.is_nrm())
  {
    return x;
  }

  if(x.is_zero())
  {
    return qf::zero();
  }
  else
  {
    const q_float value = qf::log(qf::fabs(x) + qf::sqrt(x * x + qf::one()));
    return !x.is_neg() ? value : -value;
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::acosh(const q_float& x)
///
/// \brief    Returns the acosh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::acosh(const q_float& x)
{
  if(!x.is_nrm())
  {
    return x;
  }

  return x.is_neg() || x < qf::one() ? qf::NaN() : (x.is_one() ? qf::one() : qf::log(x + qf::sqrt(x * x - qf::one())));
}

/// ---------------------------------------------------------------------------
/// \func     q_float qf::atanh(const q_float& x)
///
/// \brief    Returns the atanh of q_float x.
/// ---------------------------------------------------------------------------
q_float qf::atanh(const q_float& x)
{
  if(!x.is_nrm())
  {
    return x;
  }

  const q_float xx = qf::fabs(x);
  
  if(xx >= qf::one())
  {
    return qf::NaN();
  }

  const q_float value = (qf::log((xx + qf::one()) / (-xx + qf::one()))).div_by_int(2);

  return !xx.is_neg() ? value : -value;
}
