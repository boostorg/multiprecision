#ifndef CRYPTO3_MP_MONTGOMERY_INT_DIV_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_DIV_HPP

namespace boost {
    namespace multiprecision {
        namespace backends {

            template<typename Backend2, typename Backend3,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>

            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value &&
                    !is_trivial_montgomery_int<Backend3>::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend2 &a,
                               const Backend3 &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign() != b.sign();
                divide_unsigned_helper(&result, a, b, r);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend2 &a,
                               limb_type &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign();
                divide_unsigned_helper(&result, a, b, r);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend2 &a,
                               signed_limb_type &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign() != (b < 0);
                divide_unsigned_helper(&result, a,
                                       static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(b)), r);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend2 &b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               signed_limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }

            template<typename Backend2, typename Backend3,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value &&
                    !is_trivial_montgomery_int<Backend3>::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const Backend2 &a,
                                const Backend3 &b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a, b,
                        result);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const Backend2 &a,
                                limb_type b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a, b,
                        result);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const Backend2 &a,
                                signed_limb_type b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a,
                        static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(b)), result);
                result.sign(s);
            }

            template<typename Backend2,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<Backend2>::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const Backend2 &b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                signed_limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

//
// Over again for trivial cpp_int's:
//
            template<typename Backend,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<Backend>::value &&
                    (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                     is_signed_number<Backend>::value)>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() /= *o.limbs();
                result.sign(result.sign() != o.sign());
            }

            template<typename Backend,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<Backend>::value &&
                    is_unsigned_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_unsigned_number<Backend>::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const Backend &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() /= *o.limbs();
            }

            template<typename Backend,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<Backend>::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const Backend &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() %= *o.limbs();
                result.sign(result.sign());
            }
            /*
            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3,
                    class Allocator3, typename ParamsBackend3>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                               const montgomery_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign() != b.sign();
                divide_unsigned_helper(&result, a, b, r);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                               limb_type &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign();
                divide_unsigned_helper(&result, a, b, r);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                               signed_limb_type &b) {
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> r;
                bool s = a.sign() != (b < 0);
                divide_unsigned_helper(&result, a,
                        static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(b)), r);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               signed_limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_divide(result, a, b);
            }
            ---
            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3,
                    class Allocator3, typename ParamsBackend3>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                                const montgomery_int_backend<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> &b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a, b,
                        result);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                                limb_type b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a, b,
                        result);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &a,
                                signed_limb_type b) {
                bool s = a.sign();
                divide_unsigned_helper(
                        static_cast<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> * >(0), a,
                        static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(b)), result);
                result.sign(s);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> &b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                signed_limb_type b) {
                // There is no in place divide:
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> a(result);
                eval_modulus(result, a, b);
            }

//
// Over again for trivial cpp_int's:
//
            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                     is_signed_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value)>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() /= *o.limbs();
                result.sign(result.sign() != o.sign());
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_unsigned_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_unsigned_number<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_divide(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                               const montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() /= *o.limbs();
            }

            template<unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1>
            BOOST_MP_FORCEINLINE typename enable_if_c<
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

            ::type eval_modulus(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &result,
                                const montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &o) {
                if (!*o.limbs()) {
                    BOOST_THROW_EXCEPTION(std::overflow_error("Division by zero."));
                }
                *result.limbs() %= *o.limbs();
                result.sign(result.sign());
            }
             */

        }
    }
} // namespaces

#endif
