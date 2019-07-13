#ifndef CRYPTO3_MP_MONTGOMERY_INT_COMPARISON_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_COMPARISON_HPP

#include <boost/type_traits/make_unsigned.hpp>

namespace boost {
    namespace multiprecision {

        namespace backends {

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4018 4389 4996)
#endif

//
// Start with non-trivial cpp_int's:
//
            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
                    unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend> >::value &&
                    !is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend>>::value>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend> &a,
                           const Backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend> &b) BOOST_NOEXCEPT {
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1600)
                return (a.sign() == b.sign())
                   && (a.size() == b.size())
                   && std::equal(a.limbs(), a.limbs() + a.size(),
                      stdext::checked_array_iterator<montgomery_int_backend<MinBits, MaxBits, SignType, Checked,Allocator, ParamsBackend>::const_limb_pointer>(b.limbs(), b.size()));
#else
                return (a.sign() == b.sign()) && (a.size() == b.size()) &&
                       std::equal(a.limbs(), a.limbs() + a.size(), b.limbs());
#endif
            }


            template<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename > class Backend,
                    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
                    class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
                    class Allocator2, typename ParamsBackend2>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                    !is_trivial_montgomery_int<
                            Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value &&
                                                                      is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>>::value>

            ::type eval_eq(const montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> &a,
                           const Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                   Allocator2, ParamsBackend2> &b) BOOST_NOEXCEPT {
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1600)
                return (a.sign() == b.sign())
                   && (a.size() == b.size())
                   && std::equal(a.limbs(), a.limbs() + a.size(), stdext::checked_array_iterator<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>::const_limb_pointer>(b.limbs(), b.size()));
#else
                return (a.sign() == b.sign()) && (a.size() == b.size()) &&
                       std::equal(a.limbs(), a.limbs() + a.size(), b.limbs());
#endif
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                return (a.sign() == false) && (a.size() == 1) && (*a.limbs() == b);
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                return (a.sign() == (b < 0)) && (a.size() == 1) &&
                       (*a.limbs() == boost::multiprecision::detail::unsigned_abs(b));
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                return (a.size() == 1) && (*a.limbs() == b);
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                return (b < 0) ? eval_eq(a,
                        montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend>(b)) : eval_eq(
                        a, static_cast<limb_type>(b)); // Use bit pattern of b for comparison
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                if (a.sign()) {
                    return true;
                }
                if (a.size() > 1) {
                    return false;
                }
                return *a.limbs() < b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            inline typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                if ((b == 0) || (a.sign() != (b < 0))) {
                    return a.sign();
                }
                if (a.sign()) {
                    if (a.size() > 1) {
                        return true;
                    }
                    return *a.limbs() > boost::multiprecision::detail::unsigned_abs(b);
                } else {
                    if (a.size() > 1) {
                        return false;
                    }
                    return *a.limbs() < boost::multiprecision::detail::unsigned_abs(b);
                }
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                if (a.size() > 1) {
                    return false;
                }
                return *a.limbs() < b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                return (b < 0) ? a.compare(b) < 0 : eval_lt(a,
                        static_cast<limb_type>(b)); // Use bit pattern of b for comparison
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                if (a.sign()) {
                    return false;
                }
                if (a.size() > 1) {
                    return true;
                }
                return *a.limbs() > b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            inline typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                if (b == 0) {
                    return !a.sign() && ((a.size() > 1) || *a.limbs());
                }
                if (a.sign() != (b < 0)) {
                    return !a.sign();
                }
                if (a.sign()) {
                    if (a.size() > 1) {
                        return false;
                    }
                    return *a.limbs() < boost::multiprecision::detail::unsigned_abs(b);
                } else {
                    if (a.size() > 1) {
                        return true;
                    }
                    return *a.limbs() > boost::multiprecision::detail::unsigned_abs(b);
                }
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           limb_type b) BOOST_NOEXCEPT {
                if (a.size() > 1) {
                    return true;
                }
                return *a.limbs() > b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,Allocator, ParamsBackend> &a,
                           signed_limb_type b) BOOST_NOEXCEPT {
                return (b < 0) ? a.compare(b) > 0 : eval_gt(a,
                        static_cast<limb_type>(b)); // Use bit pattern of b for comparison.
            }

            /*!template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
            unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class Allocator, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    Backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::value eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                            const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked,
                                    void> &b) BOOST_NOEXCEPT {
                return (a.sign() == b.sign()) && (*a.limbs() == *b.limbs());
            }*/


//
// And again for trivial cpp_ints:
//
            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
            unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>

            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value &&
                    is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend>>::value>

            ::value eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> &a,
                            const Backend<MinBits, MaxBits, signed_magnitude, Checked,
                                    void, ParamsBackend> &b) BOOST_NOEXCEPT {
                return (a.sign() == b.sign()) && (*a.limbs() == *b.limbs());
            }

            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
            unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>

            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend>>::value>

            ::value eval_eq(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> &a,
                            const Backend<MinBits, MaxBits, unsigned_magnitude, Checked,
                                    void, ParamsBackend> &b) BOOST_NOEXCEPT {
                return *a.limbs() == *b.limbs();
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                           U b) BOOST_NOEXCEPT {
                return !a.sign() && (*a.limbs() == b);
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                return (a.sign() == (b < 0)) && (*a.limbs() == boost::multiprecision::detail::unsigned_abs(b));
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           U b) BOOST_NOEXCEPT {
                return *a.limbs() == b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_eq(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                typedef typename make_unsigned<S>::type ui_type;
                if (b < 0) {
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> t(b);
                    return *a.limbs() == *t.limbs();
                } else {
                    return *a.limbs() == static_cast<ui_type>(b);
                }
            }

            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
                    unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend>>::value>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> &a,
                           const Backend<MinBits, MaxBits, unsigned_magnitude, Checked,
                                   void, ParamsBackend> &b) BOOST_NOEXCEPT {
                if (a.sign() != b.sign()) {
                    return a.sign();
                }
                return a.sign() ? *a.limbs() > *b.limbs() : *a.limbs() < *b.limbs();
            }

            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
                     unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend>>::value>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> &a,
                           const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked,
                                   void, ParamsBackend> &b) BOOST_NOEXCEPT {
                return *a.limbs() < *b.limbs();
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                           U b) BOOST_NOEXCEPT {
                if (a.sign()) {
                    return true;
                }
                return *a.limbs() < b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                if (a.sign() != (b < 0)) {
                    return a.sign();
                }
                return a.sign() ? (*a.limbs() > boost::multiprecision::detail::unsigned_abs(b)) : (*a.limbs() <
                                                                                                   boost::multiprecision::detail::unsigned_abs(
                                                                                                           b));
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           U b) BOOST_NOEXCEPT {
                return *a.limbs() < b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_lt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                typedef typename make_unsigned<S>::type ui_type;
                if (b < 0) {
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> t(b);
                    return *a.limbs() < *t.limbs();
                } else {
                    return *a.limbs() < static_cast<ui_type>(b);
                }
            }

            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
                     unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend>>::value>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> &a,
                           const Backend<MinBits, MaxBits, signed_magnitude, Checked,
                                   void, ParamsBackend> &b) BOOST_NOEXCEPT {
                if (a.sign() != b.sign()) {
                    return !a.sign();
                }
                return a.sign() ? *a.limbs() < *b.limbs() : *a.limbs() > *b.limbs();
            }

            template<template<unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
                     unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_trivial_montgomery_int<
                            Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> >::value &&
                    is_equal_montgomery_int_backend<Backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend>>::value>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void, ParamsBackend> &a,
                           const Backend<MinBits, MaxBits, unsigned_magnitude, Checked,
                                   void, ParamsBackend> &b) BOOST_NOEXCEPT {
                return *a.limbs() > *b.limbs();
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, typename ParamsBackend, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void, ParamsBackend> &a,
                           U b) BOOST_NOEXCEPT {
                if (a.sign()) {
                    return false;
                }
                return *a.limbs() > b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, signed_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                if (a.sign() != (b < 0)) {
                    return !a.sign();
                }
                return a.sign() ? (*a.limbs() < boost::multiprecision::detail::unsigned_abs(b)) : (*a.limbs() >
                                                                                                   boost::multiprecision::detail::unsigned_abs(
                                                                                                           b));
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class U>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_unsigned<U>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           U b) BOOST_NOEXCEPT {
                return *a.limbs() > b;
            }

            template<unsigned MinBits, unsigned MaxBits, cpp_int_check_type Checked, class S>
            BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<S>::value && is_trivial_montgomery_int<
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> >::value, bool>

            ::type eval_gt(const montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> &a,
                           S b) BOOST_NOEXCEPT {
                typedef typename make_unsigned<S>::type ui_type;
                if (b < 0) {
                    montgomery_int_backend<MinBits, MaxBits, unsigned_magnitude, Checked, void> t(b);
                    return *a.limbs() > *t.limbs();
                } else {
                    return *a.limbs() > static_cast<ui_type>(b);
                }
            }

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

        }
    }
} // namespaces

#endif