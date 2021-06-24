///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef Q_FLOAT_LIMITS_2005_03_26_H_
  #define Q_FLOAT_LIMITS_2005_03_26_H_

  // Specialization of std::numeric_limits<q_float>.
  namespace std
  {
    template <> class numeric_limits<q_float>
    {
    public:

      // Implement all of the "usual" members for floating point types.
      static const bool  is_specialized    = true;
      static const bool  is_signed         = true;
      static const bool  is_integer        = false;
      static const bool  is_exact          = false;
      static const bool  is_bounded        = true;
      static const bool  is_modulo         = false;
      static const bool  is_iec559         = false;
      static const INT32 digits            = 102;
      static const INT32 digits10          = static_cast<INT32>(float(digits) * 0.301029995663981195F);
      static const INT32 radix             = 2;
      static const INT32 round_style       = std::round_to_nearest;
      static const bool  has_infinity      = true;
      static const bool  has_quiet_NaN     = true;
      static const bool  has_signaling_NaN = false;
      static const INT32 has_denorm        = std::denorm_absent;
      static const bool  has_denorm_loss   = false;
      static const bool  traps             = false;
      static const bool  tinyness_before   = false;

      static const INT32 max_exponent10    = q_float::max_exponent10;
      static const INT32 min_exponent10    = q_float::min_exponent10;
      static const INT32 max_exponent      = q_float::max_exponent;
      static const INT32 min_exponent      = q_float::min_exponent;

      static const q_float& (min)(void) throw()         { return q_float::minimum(); }
      static const q_float& (max)(void) throw()         { return q_float::maximum(); }
      static const q_float& epsilon(void) throw()       { return q_float::epsilon(); }
      static const q_float& round_error(void) throw()   { return q_float::half(); }
      static const q_float& denorm_min(void) throw()    { return q_float::minimum(); }
      static const q_float& infinity (void) throw()     { return q_float::infinity(); }
      static const q_float& quiet_NaN(void) throw()     { return q_float::quiet_NaN(); }
    };
  }

#endif // Q_FLOAT_LIMITS_2005_03_26_H_
