///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020-2021.        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

/// ---------------------------------------------------------------------------
/// \file    q_float.cpp
///
/// \author  Christopher Kormanyos
/// \owner   
///
/// \date    26.03.2005
///
/// \brief   Quad-precision floating point type. For the implementation of the
///          functions for addition, subtraction, multiplication, division,
///          and square-root as well as normalization, see information below.
///
/// ---------------------------------------------------------------------------

#include <sstream>
#include <algorithm>
#include <iomanip>

#include "q_float_qf.h"

q_float::q_float(const double d)
{
  const bool b_neg = d < 0.0;
  const double dd = (!b_neg ? d : -d);

  using std::floor;
  using std::log10;

  const INT32 n_exp = static_cast<INT32>(dd > 0.0 ? floor(log10(dd)) : 0);
  
  if(n_exp - (std::numeric_limits<double>::digits10 >= std::numeric_limits<double>::min_exponent10 - 1))
  {
    // Convert the scaled mantissa to a 64-bit integer with as many digits of precision
    // as std::numeric_limits<double>::digits10. Then construct *this from the 64-bit
    // integer accordingly. The technique is quite simple: The double is divided by
    // a power of ten which has been selected to produce, upon division, a 64-bit
    // integer part with std::numeric_limits<double>::digits10 digits. This 64-bit
    // integer part is then used to create a q_float which is subsequently re-scaled
    // with the appropriate power of ten.

    // Create the q_float from the scaled 64-bit integer part.
    from_uint64(static_cast<INT64>(0.5 + dd / double_p10(n_exp - (std::numeric_limits<double>::digits10 - 1))));

    // Re-scale with the appropriate power of ten.
    operator*=(qf::pow10(n_exp - (std::numeric_limits<double>::digits10 - 1)));

    if(b_neg)
    {
      negate();
    }
  }
  else
  {
    // The magnitude of the input number is too small for the scaled conversion
    // of the mantissa to a 64-bit integer. This q_float must be constructed
    // from a string.
    std::stringstream ss;

    ss << std::setprecision(std::numeric_limits<double>::digits10) << d << '\0';

    if(!rd_string(ss.str().data()))
    {
      throw qf::exception_BadIO();
      operator=(quiet_NaN());
    }
  }
}

/// ---------------------------------------------------------------------------
/// \func     double q_float::double_p10(const INT32 n)
///
/// \brief    Array values of the pure power of 10^p for doubles.
/// ---------------------------------------------------------------------------
double q_float::double_p10(const INT32 n)
{
  if(   n > std::numeric_limits<double>::max_exponent10
     || n < std::numeric_limits<double>::min_exponent10
    )
  {
    throw qf::exception_nan();
    return std::numeric_limits<double>::quiet_NaN();
  }

  if(n < 0)
  {
    return 1.0 / double_p10(-n);
  }

  static double p10_array[std::numeric_limits<double>::max_exponent10 + 1];
  static bool b_init;
  
  if(!b_init)
  {
    b_init = true;

    p10_array[0] = 1.0;
    
    for(UINT32 i = 1; i < sizeof(p10_array) / sizeof(p10_array[0]); i++)
    {
      p10_array[i] = p10_array[i - 1] * 10.0;
    }
  }
  
  return p10_array[n];
}

/// ---------------------------------------------------------------------------
/// \func     q_float::q_float(const std::string& str)
///
/// \brief    Construct a q_float from a STL-string.
/// ---------------------------------------------------------------------------
q_float::q_float(const std::string& str)
{
  if(!rd_string(str.data()))
  {
    throw qf::exception_BadIO();
    operator=(quiet_NaN());
  }
}

void q_float::exception_check(void) const
{
  const bool b_nan =  std::isnan   (hi) ||  std::isnan   (lo);
  const bool b_inf = !std::isfinite(hi) || !std::isfinite(lo);

  // Check for NAN's. Check NAN's first since the check for infinite
  // also picks up NAN's.
  if(b_nan)
  {
    throw qf::exception_nan();
  }

  // Check for infintite.
  if(b_inf)
  {
    throw qf::exception_inf();
  }
}

/// ---------------------------------------------------------------------------
/// \func     void q_float::normalize(const double& xhi, const double& xlo)
///
/// \brief    Normalize *this q_float.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
/// ---------------------------------------------------------------------------
void q_float::normalize(double xhi, double xlo)
{
  const UINT16 fp = qf::x86_fp_ControlWordSet();

  const volatile double u = xhi + xlo;
  volatile double v = xhi - u;
  v = v + xlo;

  qf::x86_fp_ControlWordReset(fp);

  hi = u;
  lo = v;
}

/// ---------------------------------------------------------------------------
/// \func     void q_float::negate(void)
///
/// \brief    Negate *this q_float. Negate the hi and low parts.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
///           In addition, see this comment from: Victor Shoup, package WinNTL-5_3_2.
///           "It is a good idea to renormalize here, just in case
///           the rounding rule depends on sign, and thus we will
///           maintain the "normal form" for q_float's".
/// ---------------------------------------------------------------------------
void q_float::negate(void)
{
  normalize(-hi, -lo);
}

/// ---------------------------------------------------------------------------
/// \func     void q_float::from_uint64(const UINT64& u)
///
/// \brief    Algorithm for creation of a q_float from an unsigned 64-bit integer.
/// ---------------------------------------------------------------------------
void q_float::from_uint64(const UINT64 u)
{
  const UINT32 uhi = static_cast<UINT32>(u >> 32);

  hi = double(uhi);
  lo = 0.0;

  if(uhi)
  {
    static const q_float value_0x100000000 = q_float(static_cast<UINT32>(0xFFFFFFFF)) + one();
    operator*=(value_0x100000000);
  }

  operator+=(q_float(static_cast<UINT32>(u)));
}

q_float& q_float::inv(void)
{
  if(is_zero())
  {
    throw qf::exception_inf();
    return operator=(infinity());
  }
  else
  {
    return operator=(one() / *this);
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float& q_float::sqrt(void)
///
/// \brief    Square root of *this q_float.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
/// ---------------------------------------------------------------------------
q_float& q_float::sqrt(void)
{
  if(is_neg())
  {
    throw qf::exception_nan();
    return operator=(quiet_NaN());
  }
  else if(is_zero() || !is_nrm())
  {
    return *this;
  }

  const UINT16 fp = qf::x86_fp_ControlWordSet();

  using std::sqrt;

  volatile double c = sqrt(hi);

  double p = split() * c;
  double hx = (c - p); 
  hx = hx + p;
  double tx = c - hx;
  p = hx * hx;
  double q = hx * tx;
  q = q + q;

  volatile double u = p + q;
  double uu = p - u;
  uu = uu + q;
  double t1 = tx * tx;
  uu = uu + t1;

  volatile double cc = hi - u;
  cc = cc - uu;
  cc = cc + lo;
  t1 = c + c;
  cc = cc / t1;

  // Simplification compared to Victor Shoup.
  hi = c + cc;
  tx = c - hi;
  lo = tx + cc;

  qf::x86_fp_ControlWordReset(fp);
  
  exception_check();

  return *this;
}

/// ---------------------------------------------------------------------------
/// \func     q_float& q_float::rootn_inv(const INT32 p)
///
/// \brief    Inverse n-th root of *this q_float.
/// ---------------------------------------------------------------------------
q_float& q_float::rootn_inv(const INT32 p)
{
  if(p == 0 || is_neg())
  {
    throw qf::exception_nan();
    return operator=(quiet_NaN());
  }
  else if(p == 1 || !is_nrm())
  {
    return *this;
  }
  else
  {
    // Invert if p < 0.
    INT32 pp = p;
    if(p < 0)
    {
      inv();
      pp = -p;
    }

    // Save the original value of *this.
    const q_float original_this(*this);

    // Generate the initial estimate using 1 / rootn and set *this equal
    // to the initial guess.
    operator=(q_float(::pow(to_double(), -1.0 / pp)));

    // Do one single step of Newton-Raphson iteration
    return operator*=((-qf::pown(*this, pp) * original_this + one()).div_by_int(pp) + one());
  }
}

/// ---------------------------------------------------------------------------
/// \func     q_float& q_float::floor(void)
///
/// \brief    Calculate the floor of *this. Set and return *this to the floor of *this.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
/// ---------------------------------------------------------------------------
q_float& q_float::floor(void)
{
  const double fhi = std::floor(hi);

  if(fhi != hi)
  {
    hi = fhi;
    lo = 0;
  }
  else
  {
    normalize(fhi, std::floor(lo));
  }

  return *this;
}

/// ---------------------------------------------------------------------------
/// \func     q_float& q_float::ceil(void)
///
/// \brief    Calculate the ceil of *this. Set and return *this to the ceil of *this.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
/// ---------------------------------------------------------------------------
q_float& q_float::ceil(void)
{
  // Compute -floor(-*this);
  negate();
  floor();
  negate();
  return *this;
}

/// ---------------------------------------------------------------------------
/// \func     int q_float::compare(const q_float& v)
///
/// \brief    Compare *this q_float value with the q_float value v.
///           The function returns the expected integer value -1, 0, or -1
///           depending on the result of the comparison.
///           Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
/// ---------------------------------------------------------------------------
int q_float::compare(const q_float& v) const
{
  if(hi > v.hi) 
  {
    return 1;
  }
  else if(hi < v.hi)
  {
    return -1;
  }
  else if(lo > v.lo)
  {
    return 1;
  }
  else if(lo < v.lo)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

INT32 q_float::order    (void) const { return static_cast<INT32>(::log10(hi) + 0.5 * double(is_zero() ? 0 : (is_neg() ? -1 : 1))); }
bool  q_float::small_arg(void) const { return order() < -std::numeric_limits<q_float>::digits10 / 6; }
bool  q_float::large_arg(void) const { return order() >  std::numeric_limits<q_float>::digits10 / 6; }
bool  q_float::near_one (void) const { return q_float(qf::fabs(one() - *this)).small_arg(); }

const q_float& q_float::quiet_NaN(void)
{
  static const q_float val(std::numeric_limits<double>::quiet_NaN(), 0.0);
  return val;
}

const q_float& q_float::infinity(void)
{
  static const q_float val(std::numeric_limits<double>::infinity(), 0.0);
  return val;
}

/// ---------------------------------------------------------------------------
/// \func     const q_float& q_float::max(void)
///
/// \brief    The static constant value of q_float::maximum which is intended
///           for std::numeric_limits<q_float>::max(). This is the maximum
///           value for a q_float. It is a bit smaller than the maximum value
///           for double since the mathematics of q_float would overflow if
///           the maximum value for double were used.
/// ---------------------------------------------------------------------------
const q_float& q_float::maximum(void)
{
  static const q_float val(qf::pow10(std::numeric_limits<q_float>::max_exponent10));
  return val;
}

/// ---------------------------------------------------------------------------
/// \func     const q_float& q_float::minimum(void)
///
/// \brief    The static constant value of q_float::minimum which is intended
///           for std::numeric_limits<q_float>::min(). This is the minimum
///           value for a q_float. See the explainatory notes above for max.
///           The range of min is further limited due to the limitations imposed
///           by the two members lo, hi of q_float, both of which must remain
///           valid doubles.
/// ---------------------------------------------------------------------------
const q_float& q_float::minimum(void)
{
  static const q_float val(qf::pow10(std::numeric_limits<q_float>::min_exponent10));
  return val;
}

/// ---------------------------------------------------------------------------
/// \func     const q_float& q_float::epsilon(void)
///
/// \brief    The static constant value of q_float::epsilon which is intended
///           for std::numeric_limits<q_float>::epsilon(). This is the smallest
///           non-one value which, when subtracted from one, results in a number
///           smaller than one.
/// ---------------------------------------------------------------------------
const q_float& q_float::epsilon(void)
{
  static const q_float val = qf::pow10(1 - std::numeric_limits<q_float>::digits10);
  return val;
}

const q_float& q_float::zero       (void) { static const q_float val(0); return val; }
const q_float& q_float::one        (void) { static const q_float val(1); return val; }
const q_float& q_float::two        (void) { static const q_float val(2); return val; }
const q_float& q_float::ten        (void) { static const q_float val(10); return val; }
const q_float& q_float::one_minus  (void) { static const q_float val(-1); return val; }
const q_float& q_float::half       (void) { static const q_float val(0.5); return val; }
const q_float& q_float::sqrt2      (void) { static const q_float val("1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727"); return val; }
const q_float& q_float::pi         (void) { static const q_float val("3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680"); return val; }
const q_float& q_float::e          (void) { static const q_float val("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"); return val; }
const q_float& q_float::ln2        (void) { static const q_float val("0.6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875"); return val; }
const q_float& q_float::ln10       (void) { static const q_float val("2.3025850929940456840179914546843642076011014886287729760333279009675726096773524802359972050895982983"); return val; }
const q_float& q_float::lg2        (void) { static const q_float val("0.3010299956639811952137388947244930267681898814621085413104274611271081892744245094869272521181861720"); return val; }
const q_float& q_float::euler_gamma(void) { static const q_float val("0.5772156649015328606065120900824024310421593359399235988057672348848677267776646709369470632917467495"); return val; }
const q_float& q_float::pi_half    (void) { static const q_float val(q_float(pi()).div_by_int(  2)); return val; }
const q_float& q_float::two_pi     (void) { static const q_float val(q_float(pi()).mul_by_int(  2)); return val; }
const q_float& q_float::degree     (void) { static const q_float val(q_float(pi()).div_by_int(180)); return val; }

/// ---------------------------------------------------------------------------
/// \func     bool q_float::wr_string(std::string& str, const std::ostream& os) const
///
/// \brief    Write *this q_float to a string using the ostream flags from os.
/// ---------------------------------------------------------------------------
bool q_float::wr_string(std::string& str, const std::ostream& os) const
{
  static const std::string str_unknown = "q_float UNK";

  if(!is_nrm())
  {
    if(is_NaN())
    {
      static const std::string str_NaN = "q_float NaN";
      str = str_NaN;
      return true;
    }
    else if(is_inf())
    {
      static const std::string str_inf = "q_float INF";
      str = str_inf;
      return true;
    }
    
    str = str_unknown;
    throw qf::exception_BadIO();
    return false;
  }

  const bool b_zero = is_zero();

  // Create a scaled, positive copy of *this q_float which is scaled to the form
  // as seen by the following example 1.234567... with one decimal digit to the
  // left of the decimal point.
  q_float x(*this);

  const bool b_neg  = x.is_neg();

  if(b_neg)
  {
    x.negate();
  }

  // Extract the base10 exponent.
  INT32 n_exp = !b_zero ? static_cast<INT32>(qf::to_int64(qf::floor(qf::log10(x)))) : 0;

  x /= qf::pow10(n_exp);

  str = "";
  
  if(b_zero)
  {
    static std::string str_zero("");
    static bool b_init = false;
    
    if(!b_init)
    {
      b_init = true;
      
      for(INT32 i = 0; i < std::numeric_limits<q_float>::digits10; i++)
      {
        str_zero += "0";
      }
    }

    str = str_zero;
  }
  else
  {
    // Extract each decimal digit including one digit more than the
    // least significant decimal digit of precision.
    for(std::string::size_type pos = 0; pos <= static_cast< std::string::size_type>(std::numeric_limits<q_float>::digits10) ; pos++)
    {
      char c = static_cast<char>(x.hi);
      
      // Is a borrow from the previous digit necessary?
      if(c < 0)
      {
        // Need to borrow from the previous digit.
        x += 10;
        c = static_cast<char>(x.hi);

        // Subtract ten through subtraction of one from the previous digit. If the
        // previous digit is zero, the step through each digit toward the digit of
        // highest significance.
        for(INT32 i = static_cast<INT32>(pos - 1); i >= 0; i--)
        {
          INT32 nc = str.at(static_cast< std::string::size_type>(i)) - static_cast<INT32>('0');

          if(!nc)
          {
            // The previous digit is 0. Set it to 9 and continue the
            // loop such that the borrow can be made from the next
            // previous digit which is previous to this one.
            str.at(static_cast< std::string::size_type>(i)) = '9';
          }
          else
          {
            // Borrow operation was successful. Break the loop.
            --nc;
            str.at(static_cast< std::string::size_type>(i)) = static_cast<char>(nc + static_cast<INT32>('0'));
            break;
          }
        }
      }
      
      if(c < 0 || c > 10)
      {
        str = str_unknown;
        throw qf::exception_BadIO();
        return false;
      }
        
      str += static_cast<char>(static_cast<INT32>(c) + static_cast<INT32>('0'));
      
      x -= static_cast<INT32>(c);
      x *= 10;
    }
  }

  const std::streamsize os_prec = os.precision();
  UINT32 prec = static_cast<UINT32>(os_prec);
  
  if(   os_prec > static_cast<UINT32>(std::numeric_limits<q_float>::digits10)
     || os.precision() <= 0
    )
  {
    prec = std::numeric_limits<q_float>::digits10;
  }

  // Cut the length output to the desired precision.
  if(str.length() > prec)
  {
    // Rounding: Get the digit after the last needed digit for rounding
    const INT32 n_round = str.at(prec) - static_cast<INT32>('0');

    // Truncate the string
    str = str.substr(0, prec);

    if(n_round >= 5)
    {
      std::string::size_type ix = str.length() - 1;

      // Every trailing 9 must be rounded up
      while(ix > 0 && static_cast<INT32>(str.at(ix)) - static_cast<INT32>('0') == 9)
      {
        str.at(ix) = '0';
        --ix;
      }

      // There were nothing but trailing nines.
      if(ix == 0)
      {
        // Is the leading digit a '9'?
        if(str.at(ix) - static_cast<INT32>('0') == 9)
        {
          // Increment up to the next order and adjust exponent.
          str.at(ix) = '1';
          ++n_exp;
        }
        else
        {
          // Round up the leading digit, assume ascii digit nemration.
          ++str.at(ix);
        }
      }
      else
      {
        // Round up last digit.
        ++str.at(ix);
      }
    }
  }

  // Add the decimal point.
  str.insert(1, std::string("."));

  // Append sign.
  const std::ios::fmtflags os_flags = os.flags();
  static const std::string strP("+");
  static const std::string strM("-");

  if(is_neg())
  {
    str.insert(0, strM);
  }
  else if(os_flags && (os_flags & std::ios::showpos))
  {
    str.insert(0, strP);
  }

  // Add the exponent with sign, right-justified, padded with zeros.
  if(str.length())
  {
    std::stringstream ss;

    ss << ((os_flags & std::ios::uppercase) ? 'E' : 'e')
       << (n_exp < 0 ? strM : strP)
       << std::setw(3)
       << std::setfill('0')
       << ::abs(n_exp);
       
    str += ss.str();
  }

  return str.length() > 0;
}

/// ---------------------------------------------------------------------------
/// \func     bool q_float::rd_string(const char* const s)
///
/// \brief    Read and set *this q_float from a string.
/// ---------------------------------------------------------------------------
bool q_float::rd_string(const char* const s)
{
  std::stringstream ss;

  std::string str(s);

  // Remove spaces and tabs
  str.erase(std::remove(str.begin(), str.end(), ' ' ), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());

  // Get possible exponent and remove it
  INT32 n_exp = 0;
  std::string::size_type pos;
  if(   (pos = str.find('e')) != std::string::npos
     || (pos = str.find('E')) != std::string::npos
    )
  {
    ss << str.data() + pos + 1 << std::endl;
    ss >> n_exp;
    str = str.substr(0, pos);
  }

  // Get possible +/- sign and remove it
  bool b_neg = false;
  if((pos = str.find('-')) != std::string::npos)
  {
    b_neg = true;
    str.erase(pos, 1);
  }

  if((pos = str.find('+')) != std::string::npos)
  {
    str.erase(pos, 1);
  }

  // Remove leading zeros for all input types
  while(str.length() && str.at(0) == '0')
  {
    str.erase(0, 1);
  }

  // Put the input string into the standard q_float input form aaa.bbbbE+/-n,
  // where aaa and bbbb have an arbitrary number of digits which are possibly
  // zero padded on the right-end, and n is a signed arbitrary 32-bit integer.

  // Find possible decimal point
  if((pos = str.find('.')) != std::string::npos)
  {
    // Remove trailing insignificant zeros
    while(str.at(str.length() - 1) == '0')
    {
      str.erase(str.length() - 1, 1);
    }

    // Check if input is identically zero
    if(str == std::string("."))
    {
      operator=(zero());
      return true;
    }

    // Remove leading significant zeros just after the decimal point
    // and adjust the exponent accordingly. Note that the while-loop
    // operates only on strings of the form ".000abcd..." and peels
    // away the zeros just after the decimal point.
    if((str.at(1) == '0' && pos == 0) || str.at(0) == '.')
    {
      while(str.at(1) == '0')
      {
        str.erase(1, 1);
        --n_exp;
      }

      // Bring one single digit into the mantissa and adjust exponent accordingly.
      str.insert(2, ".");
      str.erase(0, 1);
      --n_exp;
    }
  }
  else
  {
    // Input string has no decimal point: Append decimal point.
    str.append(".");
  }

  // Shift the decimal point such that there is one single digit in the
  // mantissa to the right of the decimal point. Adjust the exponent accordingly.
  if((pos = str.find('.')) == std::string::npos)
  {
    return false;
  }

  const INT32 n_shift = static_cast<INT32>(pos) - 1;
  n_exp += n_shift;

  // Erase the decimal point. Do not re-write it anywhere.
  str.erase(pos, 1);

  // Clear this q_float.
  hi = lo = 0.0;

  // Extract the individual digits.
  q_float base(one());

  const std::string::size_type str_len = str.length();

  for(pos = 0; pos < str_len; pos++)
  {
    const INT32 nc = static_cast<INT32>(str[str_len - (pos + 1U)]) - static_cast<INT32>('0');

    if(nc < 0 || nc > 9)
    {
      return false;
    }

    operator+=(base * nc);

    base *= 10;
  }

  n_exp = (n_exp - static_cast<INT32>(str.length())) + 1;

  operator*=(qf::pow10(n_exp));

  if(b_neg)
  {
    negate();
  }

  return true;
}

void q_float::dump(const q_float& x, std::ostream& os)
{
  // Dump the hi and lo parts *this q_float into an ostream reference.
  // Dump either the decimal double or the raw 64-bit hexadecimal
  // representation based on the value of the os flags.
  const std::ios::fmtflags os_flags = os.flags();
  
  if(os_flags & std::ios::hex)
  {
    // Dump the hexadecimal representation of the hi and lo parts.
    const bool b_showbase = (os_flags & std::ios::showbase) != 0;
    
    if(b_showbase)
    {
      os.unsetf(std::ios::showbase);
    }

    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #endif

    os << "Parts dump; "
       << "hi: 0x" << *((UINT64*) (&x.hi)) << ", "
       << "lo: 0x" << *((UINT64*) (&x.lo)) << std::endl;

    #if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
    #endif

    if(b_showbase)
    {
      os.setf(std::ios::showbase);
    }
  }
  else
  {
    // Dump the hi and lo parts as represented in decimal double form.
    const std::streamsize p = os.precision();
    os.precision(std::numeric_limits<double>::digits10 + 2);

    os << "Parts dump; "
       << "hi: " << x.hi << ", "
       << "lo: " << x.lo << std::endl;

    os.precision(p);
  }
}

void q_float::dump(const q_float& x, std::string& str)
{
  std::stringstream ss;
  
  dump(x, ss);
  
  str = ss.str();
}

bool q_float::dump_digits(const q_float& x, std::ostream& os)
{
  // Dump the raw string value of *this q_float into the input std::string.
  // Do not include the decimal point, the sign or the exponent.
  std::stringstream ss;
  
  // Set output precision and flags.
  ss.precision(std::numeric_limits<q_float>::digits10);
  ss.setf(std::ios::scientific | std::ios::showpos | std::ios::uppercase);

  ss << x;
  
  std::string str(ss.str());
  
  const std::string::size_type pos = str.find("E");
  
  if(pos && pos != std::string::npos)
  {
    str = str.substr(0, pos);
  }
  else
  {
    str = "";
    return false;
  }

  // Remove the sign.
  if(str.at(0) == '+' || str.at(0) == '-')
  {
    str.erase(0, 1);
  }
  else
  {
    str = "";
    return false;
  }

  // Remove the decimal point.
  if(str.at(1) == '.')
  {
    str.erase(1, 1);
  }
  else
  {
    str = "";
    return false;
  }

  // Append the digits to the dump string.  
  os << "Digit dump; " << str;
  
  return true;
}

bool q_float::dump_digits(const q_float& x, std::string& str)
{
  std::stringstream ss;
  
  const bool b_dump = dump_digits(x, ss);
  
  str = ss.str();
  
  return b_dump;
}

/// Licensing for the algorithms of addition, subtraction, multiplication
/// and division, please see the license information below.

/*
Copyright (C) 1997, 1998, 1999, 2000 Victor Shoup

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*****************************************************

The quad_float package is derived from the doubledouble package of
Keith Briggs.  However, the version employed in NTL has been extensively 
modified.  Below, I attach the copyright notice from the original
doubledouble package, which is currently available at 

   http://www.labs.bt.com/people/briggsk2

*****************************************************

Copyright (C) 1997 Keith Martin Briggs

Except where otherwise indicated,
this program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
