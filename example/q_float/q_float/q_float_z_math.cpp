///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include "q_float_qf.h"

std::complex<q_float> qfz::polar(const q_float& mod, const q_float& arg)
{
  q_float s, c;
  qf::sincos(arg, &s, &c);
  return std::complex<q_float>(c, s) * mod;
}

std::complex<q_float> qfz::sin(const std::complex<q_float>& z)
{
  q_float sin_x, cos_x, sinh_y, cosh_y;

  qf::sincos  (z.real(), &sin_x, &cos_x);
  qf::sinhcosh(z.imag(), &sinh_y, &cosh_y);

  return std::complex<q_float>(sin_x * cosh_y, cos_x * sinh_y);
}

std::complex<q_float> qfz::cos(const std::complex<q_float>& z)
{
  q_float sin_x, cos_x, sinh_y, cosh_y;

  qf::sincos  (z.real(), &sin_x,  &cos_x);
  qf::sinhcosh(z.imag(), &sinh_y, &cosh_y);

  return std::complex<q_float>(cos_x * cosh_y, -(sin_x * sinh_y));
}

void qfz::sincos(const std::complex<q_float>& z, std::complex<q_float>* p_sin, std::complex<q_float>* p_cos)
{
  q_float sin_x, cos_x, sinh_y, cosh_y;

  qf::sincos  (z.real(), &sin_x,  &cos_x);
  qf::sinhcosh(z.imag(), &sinh_y, &cosh_y);

  if(p_sin)
  {
    *p_sin = std::complex<q_float>(sin_x * cosh_y, cos_x * sinh_y);
  }
  
  if(p_cos)
  {
    *p_cos = std::complex<q_float>(cos_x * cosh_y, -(sin_x * sinh_y));
  }
}

std::complex<q_float> qfz::tan(const std::complex<q_float>& z)
{
  std::complex<q_float> s, c;
  qfz::sincos(z, &s, &c);
  return s / c;
}

std::complex<q_float> qfz::asin(const std::complex<q_float>& z)
{
  return -qfz::iz(qfz::log(qfz::iz(z) + qfz::sqrt(qfz::one() - z * z)));
}

std::complex<q_float> qfz::acos(const std::complex<q_float>& z)
{
  return std::complex<q_float>(qf::pi_half()) - qfz::asin(z);
}

std::complex<q_float> qfz::atan(const std::complex<q_float>& z)
{
  const std::complex<q_float> iz = qfz::iz(z);
  return qfz::iz(qfz::log(std::complex<q_float>::one() - iz) - qfz::log(std::complex<q_float>::one() + iz)).div_by_int(2);
}

std::complex<q_float> qfz::inv(const std::complex<q_float>& z)
{
  return std::complex<q_float>(z).inv();
}

std::complex<q_float> qfz::sqrt(const std::complex<q_float>& z)
{
  return std::complex<q_float>(z).sqrt();
}

std::complex<q_float> qfz::exp(const std::complex<q_float>& z)
{
  q_float s, c;
  qf::sincos(z.imag(), &s, &c);
  return std::complex<q_float>(c , s) * qf::exp(z.real());
}

std::complex<q_float> qfz::log(const std::complex<q_float>& z)
{
  return std::complex<q_float>(qf::log(qfz::norm(z)).div_by_int(2), qf::atan2(z.imag(), z.real()));
}

std::complex<q_float> qfz::pown(const std::complex<q_float>& z, const INT32 p)
{
  // Normal argument?
  if(!z.is_nrm())
  {
    return z;
  }

  if(p < 0)
  {
    return qfz::pown(z, -p).inv();
  }
  else
  {
    switch(p)
    {
      case 0:
        return qfz::one();

      case 1:
        return z;

      case 2:
        return z * z;

      case 3:
        return z * z * z;

      case 4:
      {
        const std::complex<q_float> z2(z * z);
        return z2 * z2;
      }

      default:
      {
        std::complex<q_float> zn(z);
        INT32 n;
        for(n = 1; n <= p / 2; n*= 2)
        {
          zn *= zn;
        }

        // Call the function recursively for computing the remaining pown.
        return p - n == 0 ? zn : zn * qfz::pown(z, p - n);
      }
    }
  }
}

std::complex<q_float> qfz::pow(const std::complex<q_float>& z, const std::complex<q_float>& a)
{
  return qfz::exp(a * qfz::log(z));
}

std::complex<q_float> qfz::rootn(const std::complex<q_float>& z, const INT32 p)
{
  if(p < 0)
  {
    return qfz::pown(z, p);
  }
  else if(p == 0)
  {
    return std::complex<q_float>(qf::NaN(), qf::zero());
  }
  else if(p == 1)
  {
    return z;
  }
  else
  {
    return qfz::polar(qf::rootn(qfz::norm(z), 2 * p), qfz::arg(z).div_by_int(p));
  }
}

std::complex<q_float> qfz::sinh(const std::complex<q_float>& z)
{
  q_float sin_y, cos_y, sinh_x, cosh_x;

  qf::sincos(z.imag(), &sin_y,  &cos_y);
  qf::sinhcosh(z.real(), &sinh_x, &cosh_x);

  return std::complex<q_float>(cos_y * sinh_x, cosh_x * sin_y);
}

std::complex<q_float> qfz::cosh(const std::complex<q_float>& z)
{
  q_float sin_y, cos_y, sinh_x, cosh_x;
  
  qf::sincos(z.imag(), &sin_y,  &cos_y);
  qf::sinhcosh(z.real(), &sinh_x, &cosh_x);

  return std::complex<q_float>(cos_y * cosh_x, sin_y * sinh_x);
}

void qfz::sinhcosh(const std::complex<q_float>& z, std::complex<q_float>* p_sinh, std::complex<q_float>* p_cosh)
{
  q_float sin_y, cos_y, sinh_x, cosh_x;

  qf::sincos  (z.imag(), &sin_y, &cos_y);
  qf::sinhcosh(z.real(), &sinh_x, &cosh_x);

  if(p_sinh)
  {
    *p_sinh = std::complex<q_float>(cos_y * sinh_x, cosh_x * sin_y);
  }

  if(p_cosh)
  {
    *p_cosh = std::complex<q_float>(cos_y * cosh_x, sin_y  * sinh_x);
  }
}

std::complex<q_float> qfz::tanh(const std::complex<q_float>& z)
{
  std::complex<q_float> sh, ch;
  qfz::sinhcosh(z, &sh, &ch);
  return sh / ch;
}

std::complex<q_float> qfz::asinh(const std::complex<q_float>& z)
{
  return qfz::log(z + qfz::sqrt(z * z + std::complex<q_float>::one()));
}

std::complex<q_float> qfz::acosh(const std::complex<q_float>& z)
{
  const std::complex<q_float> zp = z + std::complex<q_float>::one();
  const std::complex<q_float> zm = z - std::complex<q_float>::one();

  return qfz::log(z + zp * qfz::sqrt(zm / zp));
}

std::complex<q_float> qfz::atanh(const std::complex<q_float>& z)
{
  return (  qfz::log(std::complex<q_float>::one() + z)
          - qfz::log(std::complex<q_float>::one() - z)).div_by_int(2);
}
