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
      static constexpr bool                    is_specialized    = true;
      static constexpr bool                    is_signed         = true;
      static constexpr bool                    is_integer        = false;
      static constexpr bool                    is_exact          = false;
      static constexpr bool                    is_bounded        = true;
      static constexpr bool                    is_modulo         = false;
      static constexpr bool                    is_iec559         = false;
      static constexpr int                     digits            = 2 * (std::numeric_limits<double>::digits - 1);
      static constexpr int                     digits10          = (int) (float(digits - 1) * 0.301F);
      static constexpr int                     max_digits10      = (int) float(digits * 0.301F) + 2;
      static constexpr int                     radix             = 2;
      static constexpr std::float_round_style  round_style       = std::round_to_nearest;
      static constexpr bool                    has_infinity      = true;
      static constexpr bool                    has_quiet_NaN     = true;
      static constexpr bool                    has_signaling_NaN = false;
      static constexpr std::float_denorm_style has_denorm        = std::denorm_absent;
      static constexpr bool                    has_denorm_loss   = false;
      static constexpr bool                    traps             = false;
      static constexpr bool                    tinyness_before   = false;

      static constexpr int                     max_exponent10    = q_float::max_exponent10;
      static constexpr int                     min_exponent10    = q_float::min_exponent10;
      static constexpr int                     max_exponent      = q_float::max_exponent;
      static constexpr int                     min_exponent      = q_float::min_exponent;

      // TBD: Can there be better constexpr correctness here?
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
