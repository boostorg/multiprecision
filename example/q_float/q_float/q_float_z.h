///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef Q_FLOAT_Z_2005_03_26_H_
  #define Q_FLOAT_Z_2005_03_26_H_

  #include "q_float_qf.h"

  #include <complex>

  namespace std
  {
    // TBD: It is not appropriate to specialize the class std::complex
    // for any type othjer that built-in float, double or long double.

    // TBD: Rather use some kind of "extended complex" or complex adapter
    // for this

    template<> class complex<q_float>
    {
    private:
    
      q_float R;
      q_float I;

    public:

      inline complex<q_float>() { }

      inline complex<q_float>(const std::complex<q_float>& z) : R(z.R), I(z.I) { }

      inline explicit complex<q_float>(const  INT32 n)   : R(q_float(n)), I(qf::zero()) { }
      inline explicit complex<q_float>(const UINT32 u)   : R(q_float(u)), I(qf::zero()) { }
      inline explicit complex<q_float>(const  INT64 n)   : R(q_float(n)), I(qf::zero()) { }
      inline explicit complex<q_float>(const UINT64 u)   : R(q_float(u)), I(qf::zero()) { }
      inline explicit complex<q_float>(const double&  d) : R(q_float(d)), I(qf::zero()) { }
      inline explicit complex<q_float>(const q_float& r) : R(r),          I(qf::zero()) { }

      inline complex<q_float>(const q_float& fr, const q_float& fi) : R(fr),          I(fi)          { }
      inline complex<q_float>(const  INT32  nr, const  INT32  ni)   : R(q_float(nr)), I(q_float(ni)) { }
      inline complex<q_float>(const UINT32  ur, const UINT32  ui)   : R(q_float(ur)), I(q_float(ui)) { }
      inline complex<q_float>(const  INT64& nr, const  INT64& ni)   : R(q_float(nr)), I(q_float(ni)) { }
      inline complex<q_float>(const UINT64& ur, const UINT64& ui)   : R(q_float(ur)), I(q_float(ui)) { }
      inline complex<q_float>(const double& dr, const double& di)   : R(q_float(dr)), I(q_float(di)) { }

      inline explicit complex<q_float>(const complex<double>& c) : R(q_float(c.real())), I(q_float(c.imag())) { }

      inline explicit complex<q_float>(const std::string& str)
      {
        if(!rd_string(str.data()))
        {
          throw qf::exception_BadIO();
          R = I = qf::NaN();
        }
      }

      inline explicit complex<q_float>(const char* const s) { operator=(complex<q_float>(std::string(s))); }
      
    public:
    
      inline complex<q_float>& operator= (const complex<q_float>& u) { R  = u.R; I  = u.I; return *this; }
      inline complex<q_float>& operator+=(const complex<q_float>& u) { R += u.R; I += u.I; return *this; }
      inline complex<q_float>& operator-=(const complex<q_float>& u) { R -= u.R; I -= u.I; return *this; }
      
      inline complex<q_float>& operator*=(const complex<q_float>& u)
      {
        const q_float rr = R * u.R - I * u.I;
        const q_float ii = R * u.I + I * u.R;
        R = rr;
        I = ii;
        return *this;
      }
      
      inline complex<q_float>& operator/=(const complex<q_float>& u)
      {
        const q_float one_over_u_sq = (u.R * u.R + u.I * u.I).inv();
        const q_float rr = (R * u.R + I * u.I) * one_over_u_sq;
        const q_float ii = (I * u.R - R * u.I) * one_over_u_sq;
        R = rr;
        I = ii;
        return *this;
      }

      inline complex<q_float>& operator+=(const q_float& u) { R += u; return *this; }
      inline complex<q_float>& operator-=(const q_float& u) { R -= u; return *this; }
      inline complex<q_float>& operator*=(const q_float& u) { R *= u; I *= u; return *this; }
      inline complex<q_float>& operator/=(const q_float& u) { R /= u; I /= u; return *this; }

      inline complex<q_float> operator+(const complex<q_float>& u) { return complex<q_float>(*this) += u; }
      inline complex<q_float> operator-(const complex<q_float>& u) { return complex<q_float>(*this) -= u; }
      inline complex<q_float> operator*(const complex<q_float>& u) { return complex<q_float>(*this) *= u; }
      inline complex<q_float> operator/(const complex<q_float>& u) { return complex<q_float>(*this) /= u; }

      inline complex<q_float> mul_by_int(const INT32 n) { R.mul_by_int(n); I.mul_by_int(n); return *this; }
      inline complex<q_float> div_by_int(const INT32 n) { R.div_by_int(n); I.div_by_int(n); return *this; }

    public:
  
      complex<q_float>& inv(void)
      {
        // Compute inverse 1 / (x + iy) = (x - iy) / (x^2 + y^2)
        const q_float denom = norm().inv();
        R *= denom;
        I *= denom;
        I  = -I;
        return *this;
      }

      complex<q_float>& sqrt(void)
      {
        // Equation from MPFUN documentation page 12.
        // See: http://www.nersc.gov/~dhb/mpdist/mpdist.html

        // Pure zero or non-normal?
        if(is_zero() || !is_nrm())
        {
          return *this;
        }
        else
        {
          // sqrt(*this) = (s, I / 2s)     for R >= 0
          //               (|I| / 2s, +-s) for R <  0
          // where s = sqrt{ [ |R| + sqrt(R^2 + I^2) ] / 2 },
          // and the +- sign is the same as the sign of I.
          const q_float s = qf::sqrt((qf::fabs(R) + qf::sqrt(R * R + I * I)).div_by_int(2));
          
          if(R.is_pos())
          {
            R  = s;
            I /= s;
            I.div_by_int(2);
          }
          else
          {
            R  = qf::fabs(I);
            R /= s;
            R.div_by_int(2);
            I  = I.is_pos() ? s : -s;
          }
          
          return *this;
        }
      }

    public:

      // Unary +/-
      inline complex<q_float>  operator-() const { return complex<q_float>(-R, -I); }
      inline complex<q_float>& operator+()       { return *this; }

    private:

      bool rd_string(const char* const s)
      {
        const std::string str(s);

        // String must contain one and only one open parentheses, one and
        // only one close parentheses, as well as one and only one comma.
        const std::string::size_type pos_open  = str.find('(');
        const std::string::size_type pos_close = str.find(')');
        const std::string::size_type pos_com   = str.find(',');

        if(    pos_open  != std::string::npos
            && pos_close != std::string::npos
            && pos_com   != std::string::npos
            && pos_close > pos_open
          )
        {
          // Make sure there are no other commas or parentheses.
          if(   str.rfind('(') == pos_open
             && str.rfind(')') == pos_close
             && str.rfind(',') == pos_com
            )
          {
            // Extract the real and imaginary parts from the two sub-strings.
            R = q_float(str.substr(pos_open + 1, pos_com - pos_open - 1).data());
            I = q_float(str.substr(pos_com + 1, pos_close - pos_com - 1).data());

            return true;
          }
        }

        return false;
      }

    public:
  
      inline const q_float& real(void) const { return R; }
      inline const q_float& imag(void) const { return I; }

      inline q_float norm(void) const { return R * R + I * I; }
      inline q_float abs (void) const { return qf::sqrt(norm()); }
      inline q_float mod (void) const { return mod(); }
      inline q_float arg (void) const { return qf::atan2(I, R); }
      
    public:
    
      inline bool is_zero(void) const { return R.is_zero() && I.is_zero(); }
      inline bool is_nrm (void) const { return R.is_nrm()  && I.is_nrm(); }
      inline bool is_pos (void) const { return R.is_pos(); }
      inline bool is_neg (void) const { return R.is_neg(); }

    public:
    
      static const complex<q_float>& zero  (void) { static const complex<q_float> val(qf::zero(),   qf::zero()); return val; }
      static const complex<q_float>& one   (void) { static const complex<q_float> val(qf::one(),    qf::zero()); return val; }
      static const complex<q_float>& two   (void) { static const complex<q_float> val(qf::two(),    qf::zero()); return val; }
      static const complex<q_float>& half  (void) { static const complex<q_float> val(qf::half(),   qf::zero()); return val; }
      static const complex<q_float>& pi    (void) { static const complex<q_float> val(qf::pi(),     qf::zero()); return val; }
      static const complex<q_float>& two_pi(void) { static const complex<q_float> val(qf::two_pi(), qf::zero()); return val; }
      static const complex<q_float>& i     (void) { static const complex<q_float> val(qf::zero(),   qf::one() ); return val; }

    public:

      // Conversion to std::complex<double>
      std::complex<double> to_double(void) const
      {
        return std::complex<double>(R.to_double(), I.to_double());
      }

    };
  }

  inline std::complex<q_float> operator+(const std::complex<q_float>& u, const std::complex<q_float>& v) { return std::complex<q_float>(u) += v; }
  inline std::complex<q_float> operator-(const std::complex<q_float>& u, const std::complex<q_float>& v) { return std::complex<q_float>(u) -= v; }
  inline std::complex<q_float> operator*(const std::complex<q_float>& u, const std::complex<q_float>& v) { return std::complex<q_float>(u) *= v; }
  inline std::complex<q_float> operator/(const std::complex<q_float>& u, const std::complex<q_float>& v) { return std::complex<q_float>(u) /= v; }

  inline std::complex<q_float> operator+(const std::complex<q_float>& uz, const q_float& v) { return std::complex<q_float>(uz) += v; }
  inline std::complex<q_float> operator-(const std::complex<q_float>& uz, const q_float& v) { return std::complex<q_float>(uz) -= v; }
  inline std::complex<q_float> operator*(const std::complex<q_float>& uz, const q_float& v) { return std::complex<q_float>(uz) *= v; }
  inline std::complex<q_float> operator/(const std::complex<q_float>& uz, const q_float& v) { return std::complex<q_float>(uz) /= v; }

  // I/O operations.
  inline std::ostream& operator<<(std::ostream& os, const std::complex<q_float>& z)
  {
    os << '(';
    os << z.real();
    os << ',';
    os << z.imag();
    os << ')';
    return os;
  }

  inline std::istream& operator>>(std::istream& is, std::complex<q_float>& z)
  {
    std::string str;
    is >> str;
    z = std::complex<q_float>(str);
    return is;
  }

  namespace qfz
  {
    inline q_float norm(const std::complex<q_float>& z) { return z.norm(); }
    inline q_float abs (const std::complex<q_float>& z) { return z.abs(); }
    inline q_float mod (const std::complex<q_float>& z) { return z.mod(); }
    inline q_float arg (const std::complex<q_float>& z) { return z.arg(); }

    inline const std::complex<q_float>& zero  (void) { return std::complex<q_float>::zero(); }
    inline const std::complex<q_float>& one   (void) { return std::complex<q_float>::one(); }
    inline const std::complex<q_float>& two   (void) { return std::complex<q_float>::two(); }
    inline const std::complex<q_float>& half  (void) { return std::complex<q_float>::half(); }
    inline const std::complex<q_float>& pi    (void) { return std::complex<q_float>::pi(); }
    inline const std::complex<q_float>& two_pi(void) { return std::complex<q_float>::two_pi(); }
    inline const std::complex<q_float>& i     (void) { return std::complex<q_float>::i(); }

    inline std::complex<double> to_double(const std::complex<q_float>& z) { return z.to_double(); }

    inline std::complex<q_float> sgn(const std::complex<q_float>& z) { return z.is_zero() ? qfz::zero() : z / qfz::abs(z); }
    inline std::complex<q_float> iz (const std::complex<q_float>& z) { const q_float tmp(z.real()); return std::complex<q_float>(-z.imag(), tmp); }

    std::complex<q_float> polar   (const q_float& mod, const q_float& arg);
    std::complex<q_float> sin     (const std::complex<q_float>& z);
    std::complex<q_float> cos     (const std::complex<q_float>& z);
    void                    sincos  (const std::complex<q_float>& z, std::complex<q_float>* p_sin, std::complex<q_float>* p_cos);
    std::complex<q_float> tan     (const std::complex<q_float>& z);
    std::complex<q_float> asin    (const std::complex<q_float>& z);
    std::complex<q_float> acos    (const std::complex<q_float>& z);
    std::complex<q_float> atan    (const std::complex<q_float>& z);
    std::complex<q_float> inv     (const std::complex<q_float>& z);
    std::complex<q_float> sqrt    (const std::complex<q_float>& z);
    std::complex<q_float> exp     (const std::complex<q_float>& z);
    std::complex<q_float> log     (const std::complex<q_float>& z);
    std::complex<q_float> pown    (const std::complex<q_float>& z, const INT32 p);
    std::complex<q_float> pow     (const std::complex<q_float>& z, const std::complex<q_float>& a);
    std::complex<q_float> rootn   (const std::complex<q_float>& z, const INT32 p);
    std::complex<q_float> sinh    (const std::complex<q_float>& z);
    std::complex<q_float> cosh    (const std::complex<q_float>& z);
    std::complex<q_float> tanh    (const std::complex<q_float>& z);
    void                    sinhcosh(const std::complex<q_float>& z, std::complex<q_float>* p_sinh, std::complex<q_float>* p_cosh);
    std::complex<q_float> asinh   (const std::complex<q_float>& z);
    std::complex<q_float> acosh   (const std::complex<q_float>& z);
    std::complex<q_float> atanh   (const std::complex<q_float>& z);
    std::complex<q_float> gamma   (const std::complex<q_float>& x);
  }

  namespace std
  {
    // Extend the STL to include the missing templates for standard
    // mathematical functions with a complex argument.

    #if defined(_MSC_VER) && (_MSC_VER <= 1200)

      // Evidently MSVC6 does not include support for tan(z) or tanh(z).
      template<typename T> complex<T> tan (const complex<T>& z) { return std::sin(z) / std::cos(z); }
      template<typename T> complex<T> tanh(const complex<T>& z) { return std::sinh(z) / std::cosh(z); }

    #endif

    template<typename T> complex<T> iz(const complex<T>& z) { return complex<T>(-z.imag(), z.real()); }

    #if defined(_MSC_VER) && (_MSC_VER <= 1700)

    template<typename T> complex<T> asin(const complex<T>& z)  { return -std::iz<T>(std::log(std::iz<T>(z) + std::sqrt(1.0 - z * z))); }
    template<typename T> complex<T> acos(const complex<T>& z)  { return 3.14159265358979323 / 2.0 - std::asin(z); }
    template<typename T> complex<T> atan(const complex<T>& z)  { return std::iz<T>(std::log(1.0 - std::iz<T>(z)) - std::log(1.0 + std::iz<T>(z))) / 2.0; }
    template<typename T> complex<T> asinh(const complex<T>& z) { return std::log(z + std::sqrt(z * z + 1.0)); }
    template<typename T> complex<T> acosh(const complex<T>& z) { return std::log(z + (z + 1.0) * std::sqrt((z - 1.0) / (z + 1.0))); }
    template<typename T> complex<T> atanh(const complex<T>& z) { return (std::log(1.0 + z) - std::log(1.0 - z)) / 2.0; }

    #endif

    // Template specializations for mathematical functions with
    // std::complex<q_float> z, extended float complex argument.
    template<> inline q_float          norm<q_float>(const complex<q_float>& z) { return qfz::norm(z); }
    template<> inline q_float          abs<q_float>(const complex<q_float>& z)  { return qfz::abs(z); }
    template<> inline q_float          arg<q_float>(const complex<q_float>& z)  { return qfz::arg(z); }
    template<> inline complex<q_float> polar<q_float>(const q_float& rho, const q_float& theta) { return qfz::polar(rho, theta); }

    template<> inline complex<q_float> sin<q_float>(const complex<q_float>& z)  { return qfz::sin(z); }
    template<> inline complex<q_float> cos<q_float>(const complex<q_float>& z)  { return qfz::cos(z); }
    template<> inline complex<q_float> tan<q_float>(const complex<q_float>& z)  { return qfz::tan(z); }
    template<> inline complex<q_float> asin<q_float>(const complex<q_float>& z) { return qfz::asin(z); }
    template<> inline complex<q_float> acos<q_float>(const complex<q_float>& z) { return qfz::acos(z); }
    template<> inline complex<q_float> atan<q_float>(const complex<q_float>& z) { return qfz::atan(z); }

    template<> inline complex<q_float> sqrt<q_float>(const complex<q_float>& z) { return qfz::sqrt(z); }
    template<> inline complex<q_float> exp<q_float>(const complex<q_float>& z)  { return qfz::exp(z); }
    template<> inline complex<q_float> log<q_float>(const complex<q_float>& z)  { return qfz::log(z); }
    template<> inline complex<q_float> pow<q_float>(const complex<q_float>& z, const complex<q_float>& a) { return qfz::pow(z, a); }

    template<> inline complex<q_float> sinh<q_float>(const complex<q_float>& z)  { return qfz::sinh(z); }
    template<> inline complex<q_float> cosh<q_float>(const complex<q_float>& z)  { return qfz::cosh(z); }
    template<> inline complex<q_float> tanh<q_float>(const complex<q_float>& z)  { return qfz::tanh(z); }
    template<> inline complex<q_float> asinh<q_float>(const complex<q_float>& z) { return qfz::asinh(z); }
    template<> inline complex<q_float> acosh<q_float>(const complex<q_float>& z) { return qfz::acosh(z); }
    template<> inline complex<q_float> atanh<q_float>(const complex<q_float>& z) { return qfz::atanh(z); }
  }

#endif // Q_FLOAT_Z_2005_03_26_H_
