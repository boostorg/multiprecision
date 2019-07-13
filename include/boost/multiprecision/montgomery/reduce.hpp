#include <nil/crypto3/utilities/ct_utils.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/container/vector.hpp>
#include <boost/multiprecision/cpp_int/misc.hpp>
#include <nil/crypto3/multiprecision/montgomery_int/misc.hpp>
#include <nil/crypto3/multiprecision/montgomery_int/comparison.hpp>

//#include <boost/multiprecision/detail/default_ops.hpp>
//#include <nil/crypto3/multiprecision/detail/default_ops.hpp>

#ifndef CRYPTO3_MP_MONTGOMERY_INT_REDC_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_REDC_HPP

namespace boost {
    namespace multiprecision {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
#endif
            template<typename MontgomeryBackend>
            inline void redc(MontgomeryBackend &result) BOOST_MP_NOEXCEPT_IF (
                    is_non_throwing_montgomery_int<MontgomeryBackend>::value) {

                using default_ops::eval_multiply_add;
                using default_ops::eval_lt;

                typedef cpp_int_backend<MontgomeryBackend::limb_bits * 3, MontgomeryBackend::limb_bits * 3, unsigned_magnitude, unchecked, void> cpp_three_int_backend;
                typename MontgomeryBackend::allocator_type alloc;

                const size_t p_size = result.m_params.p_words();
                const limb_type p_dash = result.m_params.p_dash();
                const size_t z_size = 2 * (result.m_params.p_words() + 1);


                container::vector<limb_type, typename MontgomeryBackend::allocator_type> z(result.size(), 0);

                eval_export_bits(result, z.rbegin(), MontgomeryBackend::limb_bits);

                z.resize(z_size, 0);

                if (result.size() < z_size) {
                    result.resize(z_size, z_size);
                }

                cpp_three_int_backend w(z[0]);

                result.limbs()[0] = w.limbs()[0] * p_dash;

                eval_multiply_add(w, result.limbs()[0], result.m_params.p().backend().limbs()[0]);
                eval_right_shift(w, MontgomeryBackend::limb_bits);

                for (size_t i = 1; i != p_size; ++i) {
                    for (size_t j = 0; j < i; ++j) {
                        eval_multiply_add(w, result.limbs()[j], result.m_params.p().backend().limbs()[i - j]);
                    }

                    eval_add(w, z[i]);

                    result.limbs()[i] = w.limbs()[0] * p_dash;

                    eval_multiply_add(w, result.limbs()[i], result.m_params.p().backend().limbs()[0]);

                    eval_right_shift(w, MontgomeryBackend::limb_bits);
                }

                for (size_t i = 0; i != p_size; ++i) {
                    for (size_t j = i + 1; j != p_size; ++j) {
                       eval_multiply_add(w, result.limbs()[j], result.m_params.p().backend().limbs()[p_size + i - j]);
                    }

                    eval_add(w, z[p_size + i]);

                    result.limbs()[i] = w.limbs()[0];

                    eval_right_shift(w, MontgomeryBackend::limb_bits);
                }

                eval_add(w, z[z_size - 1]);

                result.limbs()[p_size] = w.limbs()[0];
                result.limbs()[p_size + 1] = w.limbs()[1];


                if (result.size() != p_size + 1) {
                    result.resize(p_size + 1, p_size + 1);
                }
                result.normalize();

                //if (!eval_lt(result, result.m_params.p().backend())) {
                    //eval_subtract(result, result.m_params.p().backend());
                //}


                //nil::crypto3::ct::conditional_copy_mem(borrow, result.limbs(), ws.begin(), ws.begin() + (result.m_params.p_words() + 1), (result.m_params.p_words() + 1));
                //clear_mem(result.m_params.limbs() + result.m_params.p_words(), z_size - result.m_params.p_words() - 2);

                // This check comes after we've used it but that's ok here
                //nil::crypto3::ct::unpoison(&borrow, 1);

            }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    }
} // namespaces

#endif