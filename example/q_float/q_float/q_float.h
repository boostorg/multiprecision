///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef Q_FLOAT_2005_03_26_H_
  #define Q_FLOAT_2005_03_26_H_

  #include <iostream>
  #include <limits>
  #include <string>
  #include <cmath>

  #include "q_float_types.h"

  class q_float;

  namespace qf
  {
    /// ---------------------------------------------------------------------------
    /// \func     inline void x86_fp_ControlWordSet(void)
    ///
    /// \brief    Intel x86 FP control word manipulation for correct rounding rules.
    /// ---------------------------------------------------------------------------
    inline UINT16 x86_fp_ControlWordSet(void)
    {
      #if defined(_MSC_VER)
        #if defined(X86)
          volatile UINT16 old_fpControlWord;
          _asm fnstcw old_fpControlWord
          volatile const UINT16 new_fpControlWord = (old_fpControlWord & ~0x0300u) | 0x0200u;
          _asm fldcw new_fpControlWord
          return old_fpControlWord;
        #else
          volatile UINT16 old_fpControlWord = 0U;
          return old_fpControlWord;
        #endif
      #elif defined(__GNUC__)
        #if defined(X86)
          volatile UINT16 old_fpControlWord;
          asm volatile("fnstcw %0" : "=m" (old_fpControlWord));
          volatile const UINT16 new_fpControlWord = (old_fpControlWord & ~0x0300u) | 0x0200u;
          asm volatile("fldcw %0": : "m" (new_fpControlWord));
          return old_fpControlWord;
        #else
          volatile UINT16 old_fpControlWord = 0U;
          return old_fpControlWord;
        #endif
      #else
        #error Compiler system not defined!
      #endif
    }

    /// ---------------------------------------------------------------------------
    /// \func     inline void x86_fp_ControlWordReset(const UINT16 fp)
    ///
    /// \brief    Intel x86 FP control word reset to default rounding rules.
    /// ---------------------------------------------------------------------------
    inline void x86_fp_ControlWordReset(const UINT16 fp)
    {
      #if defined(_MSC_VER)
        #if defined(X86)
          volatile const UINT16 old_fpControlWord = fp;
          _asm fldcw old_fpControlWord;
        #else
          (void) fp;
        #endif
      #elif defined(__GNUC__)
        #if defined(X86)
          volatile const UINT16 old_fpControlWord = fp;
          asm volatile("fldcw %0" : : "m" (old_fpControlWord));
        #else
          (void) fp;
        #endif
      #else
        #error Compiler system not defined!
      #endif
    }
  }

  // Define the function sgn() for double if needed.
  inline int sgn(double x) { return ((x > 0.0) ? 1 : ((x < 0.0) ? -1 : 0)); }

  class q_float
  {
  public:
    // Constructors
    q_float() { }
    q_float(const q_float& q) : hi(q.hi), lo(q.lo) { }
    q_float(q_float&& q) : hi(q.hi), lo(q.lo) { }

    explicit q_float(const UINT64 u) { from_uint64(u); }

    explicit q_float(const UINT32 u) : hi(u), lo(0) { }

    explicit q_float(const INT32 n) { operator=(n < 0 ? -q_float(static_cast<UINT32>(-n)) : q_float(static_cast<UINT32>(n))); }
    explicit q_float(const INT64 n) { operator=(n < 0 ? -q_float(static_cast<UINT64>(-n)) : q_float(static_cast<UINT64>(n))); }

    explicit q_float(const double d);

    explicit q_float(const std::string& str);
    explicit q_float(const char* const s) { operator=(q_float(std::string(s))); }

    ~q_float() = default;

    // Assignment operators.
    q_float& operator=(const q_float& v)
    {
      if(this != &v)
      {
        hi = v.hi;
        lo = v.lo;
      }

      return *this;
    }

    q_float& operator=(q_float&& v)
    {
      hi = v.hi;
      lo = v.lo;

      return *this;
    }

    // Mathematical functions.
    q_float& operator+=(const q_float& v)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      // Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
      const double S = hi + v.hi;
      const double T = lo + v.lo;
      const double e = S - hi;
      volatile double f = T - lo;

      double t1 = S - e;
      t1 = hi - t1;
      double s = v.hi - e;
      s = s + t1;

      t1 = T - f;
      t1 = lo - t1;
      double t = v.lo - f;
      t  = t + t1;

      s = s + T;
      const double H = S + s;
      double h = S - H;
      h = h + s;

      // Simplification compared to Victor Shoup.
      h  = h + t;
      hi = H + h; 
      f  = H - hi;
      lo = f + h;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& operator-=(const q_float& v)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      // Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
      const double vhi = -v.hi;
      const double vlo = -v.lo;

      const double S = hi + vhi;
      const double T = lo + vlo;
      const double e = S - hi;
      volatile double f = T - lo;

      double t1 = S - e;
      t1 = hi - t1;
      double s  = vhi - e;
      s  = s + t1;
  
      t1 = T - f;
      t1 = lo - t1;
      double t  = vlo - f;
      t  = t + t1;

      s = s + T;
      double H = S + s;
      double h = S - H;
      h = h + s;

      // Simplification compared to Victor Shoup.
      h  = h + t;
      hi = H + h; 
      f  = H - hi;
      lo = f + h;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& operator*=(const q_float& v)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      // Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
      volatile double C  = split() * hi;
      double hu = C - hi;
      double c  = split() * v.hi;
      hu = C - hu;
      double tu = hi - hu;
      double hv = c - v.hi;
      C  = hi * v.hi;
      hv = c - hv;
      const double tv = v.hi - hv;

      double t1 = hu * hv;
      t1 = t1 - C;
      double t2 = hu * tv;
      t1 = t1 + t2;
      t2 = tu * hv;
      t1 = t1 + t2;
      t2 = tu * tv;
      c  = t1 + t2;
      t1 = hi * v.lo;
      t2 = lo * v.hi;
      t1 = t1 + t2;
      c  = c + t1;

      // Simplification compared to Victor Shoup.
      hi = C + c;
      tu = C - hi;
      lo = tu + c;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& operator/=(const q_float& v)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }
  
      if(v.is_zero())
      {
        throw qf::exception_inf();
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      // Algorithm from Victor Shoup, package WinNTL-5_3_2, slightly modified.
      volatile double C  = hi / v.hi;
      double c  = split() * C;
      volatile double hc = c - C;
      double u  = split() * v.hi;
      hc = c - hc;
      const double tc = C - hc;
      double hv = u - v.hi;
      const volatile double U  = C * v.hi;
      hv = u - hv;
      double tv = v.hi - hv;

      u  = hc * hv;
      u  = u - U;
      double t1 = hc * tv;
      u  = u + t1;
      t1 = tc * hv;
      u  = u + t1;
      t1 = tc * tv;
      u  = u + t1;

      c  = hi - U;
      c  = c - u;
      c  = c + lo;
      t1 = C * v.lo;
      c  = c - t1;
      c  = c / v.hi;

      // Simplification compared to Victor Shoup.
      hi = C + c;
      tv = C - hi;
      lo = tv + c;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& mul_by_int(const INT32 n)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      volatile double C  = split() * hi;
      double hu = C - hi;
      double c  = split() * n;
      hu = C - hu;
      double tu = hi - hu;
      double hv = c - n;
      C  = hi * n;
      hv = c - hv;
      const double tv = n - hv;

      double t1 = hu * hv;
      t1 = t1 - C;
      double t2 = hu * tv;
      t1 = t1 + t2;
      t2 = tu * hv;
      t1 = t1 + t2;
      t2 = tu * tv;
      c  = t1 + t2;
      t2 = lo * n;
      c  = c + t2;

      // Simplification compared to Victor Shoup.
      hi = C + c;
      tu = C - hi;
      lo = tu + c;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& div_by_int(const INT32 n)
    {
      // Normal value?
      if(!is_nrm())
      {
        return *this;
      }
  
      if(!n)
      {
        throw qf::exception_inf();
      }

      const UINT16 fp = qf::x86_fp_ControlWordSet();

      volatile double C  = hi / n;
      double c  = split() * C;
      double hc = c - C;
      double u  = split() * n;
      hc = c - hc;
      double tc = C - hc;
      double hv = u - n;
      const volatile double U  = C * n;
      hv = u - hv;
      double tv = n - hv;

      u  = hc * hv;
      u  = u - U;
      double t1 = hc * tv;
      u  = u + t1;
      t1 = tc * hv;
      u  = u + t1;
      t1 = tc * tv;
      u  = u + t1;

      c  = hi - U;
      c  = c - u;
      c  = c + lo;
      c  = c / n;

      // Simplification compared to Victor Shoup.
      hi = C + c;
      tv = C - hi;
      lo = tv + c;

      qf::x86_fp_ControlWordReset(fp);

      exception_check();

      return *this;
    }

    q_float& inv      (void);
    q_float& sqrt     (void);
    q_float& rootn_inv(const INT32 p);
    q_float& floor    (void);
    q_float& ceil     (void);

    q_float operator+(const q_float& u) { return q_float(*this) += u; }
    q_float operator-(const q_float& u) { return q_float(*this) -= u; }
    q_float operator*(const q_float& u) { return q_float(*this) *= u; }
    q_float operator/(const q_float& u) { return q_float(*this) /= u; }

    // Comparison functions
    bool operator< (const q_float& u) const { return compare(u) <  0; }
    bool operator<=(const q_float& u) const { return compare(u) <= 0; }
    bool operator==(const q_float& u) const { return compare(u) == 0; }
    bool operator!=(const q_float& u) const { return compare(u) != 0; }
    bool operator>=(const q_float& u) const { return compare(u) >= 0; }
    bool operator> (const q_float& u) const { return compare(u) >  0; }

    bool is_zero(void) const { return operator==(zero()); }
    bool is_one (void) const { return operator==(one()); }
    bool is_neg (void) const { return operator<(zero()); }
    bool is_pos (void) const { return !is_neg(); }
    bool is_NaN (void) const { return std::isnan(hi) != 0; }
    bool is_inf (void) const { return !is_NaN() && std::isfinite(hi) == 0; }
    bool is_nrm (void) const { return !is_NaN() && !is_inf(); }

    // Unary +/-
    q_float  operator-(void) const { q_float u(*this); u.negate(); return u; }
    q_float& operator+(void)       { return *this; }

    double to_double(void) const { return hi; }
    INT64  to_int64(void) const  { return static_cast<INT64>(hi + 0.5 * ::sgn(hi)); }

    // Argument range and check functions
    INT32 order    (void) const;
    bool  small_arg(void) const;
    bool  large_arg(void) const;
    bool  near_one (void) const;

    static constexpr int max_exponent10 = std::numeric_limits<double>::max_exponent10 - (std::numeric_limits<double>::digits10 + 1);
    static constexpr int min_exponent10 = -max_exponent10;
    static constexpr int max_exponent   = static_cast<int>(static_cast<INT64>((max_exponent10 / 0.301029995663981195) + 0.5));
    static constexpr int min_exponent   = static_cast<int>(static_cast<INT64>((min_exponent10 / 0.301029995663981195) - 0.5));

    // Special values.
    static const q_float& quiet_NaN(void);
    static const q_float& infinity(void);
    static const q_float& maximum(void);
    static const q_float& minimum(void);
    static const q_float& epsilon(void);

    static const q_float& zero       (void);
    static const q_float& one        (void);
    static const q_float& two        (void);
    static const q_float& ten        (void);
    static const q_float& one_minus  (void);
    static const q_float& half       (void);
    static const q_float& sqrt2      (void);
    static const q_float& pi         (void);
    static const q_float& e          (void);
    static const q_float& ln2        (void);
    static const q_float& ln10       (void);
    static const q_float& lg2        (void);
    static const q_float& euler_gamma(void);
    static const q_float& pi_half    (void);
    static const q_float& two_pi     (void);
    static const q_float& degree     (void);

    // I/O operations.
    static void dump(const q_float& x, std::ostream& os = std::cout);
    static void dump(const q_float& x, std::string& str);

    static bool dump_digits(const q_float& x, std::ostream& os = std::cout);
    static bool dump_digits(const q_float& x, std::string& str);

    void dump(void) const { dump(*this, std::cout); }

    double rep_hi() const { return hi; }
    double rep_lo() const { return lo; }

  private:
    double hi;
    double lo;

    // Private contstructor from constituent parts.
    q_float(const double& h, const double& l) : hi(h), lo(l) { }

    void exception_check(void) const;

    // Utility constants.
    static double split(void)
    {
      // For the usual IEEE754 double representation, split = 1.0 + double(1 << 27).
      return 1.0 + static_cast<double>(1ULL << ((std::numeric_limits<double>::digits + 1) / 2));
    }

    static double double_p10(const INT32 n);

    // Utility functions.
    void normalize(double xhi, double xlo);
    void negate(void);
    int compare(const q_float& v) const;
    void from_uint64(const UINT64 u);

    // String operations.
    bool wr_string(std::string& str, const std::ostream& os) const;
    bool rd_string(const char* const s);

    friend inline std::ostream& operator<<(std::ostream& os, const q_float& x)
    {
      std::string str;
      x.wr_string(str, os);
      return os << str;
    }

    friend inline std::istream& operator>>(std::istream& is, q_float& x)
    {
      std::string str;
      is >> str;
      x = q_float(str);
      return is;
    }
  };

  // Activate these functions if desired.
  // Global arithmetic functions operating on constant references of q_float.
  inline q_float operator+(const q_float& u, const q_float& v) { return q_float(u) += v; }
  inline q_float operator-(const q_float& u, const q_float& v) { return q_float(u) -= v; }
  inline q_float operator*(const q_float& u, const q_float& v) { return q_float(u) *= v; }
  inline q_float operator/(const q_float& u, const q_float& v) { return q_float(u) /= v; }

  // Activate these functions if desired.
  // Specialization for global mul/div of const q_float object reference with INT32
  inline q_float operator+(const q_float& u, const INT32 n) { return q_float(u) += q_float(n); }
  inline q_float operator-(const q_float& u, const INT32 n) { return q_float(u) -= q_float(n); }
  inline q_float operator*(const q_float& u, const INT32 n) { return q_float(u) *= q_float(n); }
  inline q_float operator/(const q_float& u, const INT32 n) { return q_float(u) /= q_float(n); }

  inline q_float operator+(const INT32 n, const q_float& u) { return q_float(n) += u; }
  inline q_float operator-(const INT32 n, const q_float& u) { return q_float(n) -= u; }
  inline q_float operator*(const INT32 n, const q_float& u) { return q_float(n) *= u; }
  inline q_float operator/(const INT32 n, const q_float& u) { return q_float(n) /= u; }

  // Activate these functions if desired.
  // Specialization for global self-mul/self-div of q_float object reference with INT32
  inline q_float& operator+=(q_float& u, const INT32 n) { return u += q_float(n); }
  inline q_float& operator-=(q_float& u, const INT32 n) { return u -= q_float(n); }
  inline q_float& operator*=(q_float& u, const INT32 n) { return u.mul_by_int(n); }
  inline q_float& operator/=(q_float& u, const INT32 n) { return u.div_by_int(n); }

  namespace qf
  {
    inline const q_float& NaN        (void) { return q_float::quiet_NaN(); }
    inline const q_float& inf        (void) { return q_float::infinity(); }
    inline const q_float& zero       (void) { return q_float::zero(); }
    inline const q_float& half       (void) { return q_float::half(); }
    inline const q_float& one        (void) { return q_float::one(); }
    inline const q_float& two        (void) { return q_float::two(); }
    inline const q_float& ten        (void) { return q_float::ten(); }
    inline const q_float& one_minus  (void) { return q_float::one_minus(); }
    inline const q_float& sqrt2      (void) { return q_float::sqrt2(); }
    inline const q_float& pi         (void) { return q_float::pi(); }
    inline const q_float& pi_half    (void) { return q_float::pi_half(); }
    inline const q_float& two_pi     (void) { return q_float::two_pi(); }
    inline const q_float& degree     (void) { return q_float::degree(); }
    inline const q_float& e          (void) { return q_float::e(); }
    inline const q_float& ln2        (void) { return q_float::ln2(); }
    inline const q_float& ln10       (void) { return q_float::ln10(); }
    inline const q_float& lg2        (void) { return q_float::lg2(); }
    inline const q_float& euler_gamma(void) { return q_float::euler_gamma(); }

    inline double to_double(const q_float& x) { return x.to_double(); }
    inline INT64  to_int64(const q_float& x)  { return x.to_int64(); }
    inline INT32  to_int32(const q_float& x)  { return static_cast<INT32>(x.to_int64()); }

    inline const q_float& sgn  (const q_float& x) { return x.is_zero() ? qf::zero() : (x.is_neg() ? qf::one_minus() : qf::one()); }
    inline       q_float  fabs (const q_float& x) { return x.is_neg() ? -x : x; }

    inline q_float floor(const q_float& x) { return x.is_nrm() ? q_float(x).floor() : x; }
    inline q_float ceil (const q_float& x) { return x.is_nrm() ? q_float(x).ceil() : x; }
    q_float pow2        (const INT32 p);
    q_float pow10       (const INT32 p);
    q_float pown        (const q_float& x, const INT32 p);
    inline q_float inv  (const q_float& x) { return x.is_nrm() ? q_float(x).inv() : x; }
    inline q_float sqrt (const q_float& x) { return x.is_nrm() ? q_float(x).sqrt() : x; }
    inline q_float cbrt (const q_float& x) { return x.is_nrm() ? (q_float(x).rootn_inv(3)).inv() : x; }
    inline q_float rootn(const q_float& x, const INT32 p) { return x.is_nrm() ? (q_float(x).rootn_inv(p)).inv() : x; }
    q_float exp         (const q_float& x);
    q_float log         (const q_float& x);
    inline q_float log10(const q_float& x) { return x.is_nrm() ? qf::log(x) / qf::ln10() : x; }
    inline q_float loga (const q_float& x, const q_float& a) { return x.is_nrm() && a.is_nrm() ? qf::log(x) / qf::log(a) : x; }
    inline q_float pow  (const q_float& x, const q_float& a) { return x.is_nrm() && a.is_nrm() ? qf::exp(qf::log(x) * a) : x; }
    void    sincos      (const q_float& x, q_float* p_sin, q_float* p_cos);
    q_float sin         (const q_float& x);
    q_float cos         (const q_float& x);
    q_float tan         (const q_float& x);
    q_float asin        (const q_float& x);
    q_float acos        (const q_float& x);
    q_float atan        (const q_float& x);
    q_float atan2       (const q_float& y, const q_float& x);
    void    sinhcosh    (const q_float& x, q_float* p_sin, q_float* p_cos);
    q_float sinh        (const q_float& x);
    q_float cosh        (const q_float& x);
    q_float tanh        (const q_float& x);
    q_float asinh       (const q_float& x);
    q_float acosh       (const q_float& x);
    q_float atanh       (const q_float& x);
    q_float gamma       (const q_float& x);
  }

#endif // Q_FLOAT_2005_03_26_H_
