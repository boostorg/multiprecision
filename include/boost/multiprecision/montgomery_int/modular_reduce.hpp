#ifndef CRYPTO3_MP_MONTGOMERY_INT_MOD_REDC_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_MOD_REDC_HPP

//#include <boost/multiprecision/cpp_int/import_export.hpp>

#include <boost/multiprecision/mask_bits.hpp>
#include <boost/multiprecision/reduce_below.hpp>

namespace boost {
    namespace multiprecision {
        namespace backends {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
#endif

            template<typename Backend>
            inline void mod_redc(Backend &result, const Backend &mod) BOOST_MP_NOEXCEPT_IF (
                    is_non_throwing_cpp_int<Backend>::value) {

                using default_ops::eval_lt;
                using default_ops::eval_gt;
                using default_ops::eval_add;
                using default_ops::eval_multiply;
                using default_ops::eval_bit_set;
                using nil::crypto3::eval_mask_bits;
                using nil::crypto3::eval_reduce_below;

                const size_t x_sw = result.size();

                Backend mod2 = mod;
                eval_add(mod2, mod);

                if (result.size() < mod.size() || eval_lt(result, mod)) {
                    if (eval_lt(result, 0)) {
                        eval_add(result, mod);
                        return;
                    } // make positive
                    return;
                } else if (eval_lt(result, mod2)) {
                    //secure_vector<word> ws;
                    Backend t1;

                    eval_import_bits(t1, result.limbs() + mod.size() - 1, result.limbs() + x_sw);
                    {
                        Backend p2;
                        eval_bit_set(p2, 2 * Backend::limb_bits * mod.size());
                        eval_divide(p2, mod);
                        eval_multiply(t1, p2);
                    }
                    eval_right_shift(t1, (Backend::limb_bits * (mod.size() + 1)));

                    eval_multiply(t1, mod);

                    eval_mask_bits(t1, Backend::limb_bits * (mod.size() + 1));

                    eval_subtract(t1, result, t1);

//                t1.rev_sub(result.data(), std::min(x_sw, mod.size() + 1), ws);

                    if (eval_lt(t1, 0)) {
                        Backend p2;
                        eval_bit_set(p2, Backend::limb_bits * (mod.size() + 1));
                        eval_add(t1, p2);
                    }

                    eval_reduce_below(t1, mod);

                    if (eval_lt(result, 0)) {
                        eval_subtract(t1, mod, t1);
                    }

                    result = t1;
                    return;
                } else {
                    // too big, fall back to normal division
                    Backend t2;
                    divide_unsigned_helper(&result, result, mod, t2);
                    result = t2;
                    return;
                }

            }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

        }
    }
} // namespaces

#endif
