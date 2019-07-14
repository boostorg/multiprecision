#ifndef CRYPTO3_POWER_MOD_H_
#define CRYPTO3_POWER_MOD_H_

#include <boost/multiprecision/cpp_int.hpp>

namespace nil {
namespace crypto3 {

using namespace boost::multiprecision;

/**
 * Modular Exponentiator Interface
 */
class modular_exponentiator {
public:
  virtual void set_base(const cpp_int &) = 0;

  virtual void set_exponent(const cpp_int &) = 0;

  virtual cpp_int execute() const = 0;

  virtual modular_exponentiator *copy() const = 0;

  modular_exponentiator() = default;

  modular_exponentiator(const modular_exponentiator &) = default;

  modular_exponentiator &operator=(const modular_exponentiator &) = default;

  virtual ~modular_exponentiator() = default;
};

/**
 * Modular Exponentiator Proxy
 */
class power_mod {
public:
  enum usage_hints {
    NO_HINTS = 0x0000,

    BASE_IS_FIXED = 0x0001,
    BASE_IS_SMALL = 0x0002,
    BASE_IS_LARGE = 0x0004,
    BASE_IS_2 = 0x0008,

    EXP_IS_FIXED = 0x0100,
    EXP_IS_SMALL = 0x0200,
    EXP_IS_LARGE = 0x0400
  };

  /*
   * Try to choose a good window size
   */
  static size_t window_bits(size_t exp_bits, size_t base_bits,
                            power_mod::usage_hints hints);

  /**
   * @param modulus the modulus
   * @param hints Passed to set_modulus if modulus > 0
   * @param disable_montgomery_arith Disables use of Montgomery
   * representation. Likely only useful for testing.
   */
  void set_modulus(const cpp_int &modulus, usage_hints hints = NO_HINTS,
                   bool disable_montgomery_arith = false) const;

  /**
   * Set the base
   */
  void set_base(const cpp_int &base) const;

  /**
   * Set the exponent
   */
  void set_exponent(const cpp_int &exponent) const;

  /**
   * All three of the above functions must have already been called.
   * @return result of g^x%p
   */
  cpp_int execute() const;

  power_mod &operator=(const power_mod &);

  /**
   * @param modulus Optionally call set_modulus
   * @param hints Passed to set_modulus if modulus > 0
   * @param disable_montgomery_arith Disables use of Montgomery
   * representation. Likely only useful for testing.
   */
  power_mod(const cpp_int &modulus = 0, usage_hints hints = NO_HINTS,
            bool disable_montgomery_arith = false);

  power_mod(const power_mod &);

  virtual ~power_mod() = default;

private:
  mutable std::unique_ptr<modular_exponentiator> m_core;
};

/**
 * Fixed Exponent Modular Exponentiator Proxy
 */
class fixed_exponent_power_mod final : public power_mod {
public:
  cpp_int operator()(const cpp_int &b) const {
    set_base(b);
    return execute();
  }

  fixed_exponent_power_mod() = default;

  fixed_exponent_power_mod(const cpp_int &exponent, const cpp_int &modulus,
                           usage_hints hints = NO_HINTS);
};

/**
 * Fixed Base Modular Exponentiator Proxy
 */
class fixed_base_power_mod final : public power_mod {
public:
  cpp_int operator()(const cpp_int &e) const {
    set_exponent(e);
    return execute();
  }

  fixed_base_power_mod() = default;

  fixed_base_power_mod(const cpp_int &base, const cpp_int &modulus,
                       usage_hints hints = NO_HINTS);
};

/**
 * Modular exponentation
 * @param base an integer base
 * @param exp a positive exponent
 * @param mod a positive modulus
 * @return (b^x) % m
 */
template <typename Backend>
inline Backend eval_power_mod(const Backend &base, const Backend &exp,
                              const Backend &mod) {
  using default_ops::eval_eq;
  using default_ops::eval_get_sign;
  using default_ops::eval_integer_modulus;
  using default_ops::eval_is_zero;
  using default_ops::eval_lt;

  if (eval_get_sign(mod) < 0 || eval_eq(mod, 1)) {
    return 0;
  }

  if (eval_is_zero(base) || eval_is_zero(mod)) {
    if (eval_is_zero(exp)) {
      return 1;
    }
    return 0;
  }

  class power_mod pow_mod(mod);

  /*
   * Calling set_base before set_exponent means we end up using a
   * minimal window. This makes sense given that here we know that any
   * precomputation is wasted.
   */

  if (eval_get_sign(base) < 0) {
    pow_mod.set_base(-base);
    pow_mod.set_exponent(exp);
    if (!(eval_integer_modulus(exp, 2))) {
      return pow_mod.execute();
    } else {
      return (mod - pow_mod.execute());
    }
  } else {
    pow_mod.set_base(base);
    pow_mod.set_exponent(exp);
    return pow_mod.execute();
  }
}

template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
power_mod(const number<Backend, ExpressionTemplates> &base,
          const number<Backend, ExpressionTemplates> &exp,
          const number<Backend, ExpressionTemplates> &mod) {
  return number<Backend, ExpressionTemplates>(
      eval_power_mod(base.backend(), exp.backend(), mod.backend()));
}
} // namespace crypto3
} // namespace nil

#endif
