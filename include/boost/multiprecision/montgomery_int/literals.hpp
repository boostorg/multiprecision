//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_INT_LITERALS_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_INT_LITERALS_HPP

#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <boost/multiprecision/cpp_int/literals.hpp>

namespace boost {
namespace multiprecision {

namespace literals {
namespace detail {

template <unsigned Digits>
struct signed_montgomery_int_literal_result_type
{
   static constexpr unsigned bits = Digits * 4;
   typedef boost::multiprecision::backends::montgomery_int_backend<bits, bits, signed_magnitude,
                                                                   unchecked, void>
       backend_type;
   typedef number<backend_type, et_off> number_type;
};

template <unsigned Digits>
struct unsigned_montgomery_int_literal_result_type
{
   static constexpr unsigned bits = Digits * 4;
   typedef boost::multiprecision::backends::montgomery_int_backend<bits, bits, unsigned_magnitude,
                                                                   unchecked, void>
       backend_type;
   typedef number<backend_type, et_off> number_type;
};

} // namespace detail

template <char... STR>
constexpr typename boost::multiprecision::literals::detail::signed_montgomery_int_literal_result_type<
    (sizeof...(STR)) - 2>::number_type
operator"" _cppi()
{
   typedef typename boost::multiprecision::literals::detail::make_packed_value_from_str<STR...>::type pt;
   return boost::multiprecision::literals::detail::make_backend_from_pack<pt,
                                                                          typename boost::multiprecision::literals::detail::signed_montgomery_int_literal_result_type<
                                                                              (sizeof...(STR)) - 2>::backend_type>::value;
}

template <char... STR>
constexpr typename boost::multiprecision::literals::detail::unsigned_montgomery_int_literal_result_type<
    (sizeof...(STR)) - 2>::number_type
operator"" _cppui()
{
   typedef typename boost::multiprecision::literals::detail::make_packed_value_from_str<STR...>::type pt;
   return boost::multiprecision::literals::detail::make_backend_from_pack<pt,
                                                                          typename boost::multiprecision::literals::detail::unsigned_montgomery_int_literal_result_type<
                                                                              (sizeof...(STR)) - 2>::backend_type>::value;
}

#define BOOST_MP_DEFINE_SIZED_MNT_INT_LITERAL(Bits)                                                                                                                                                                                       \
   template <char... STR>                                                                                                                                                                                                                 \
   constexpr boost::multiprecision::number<boost::multiprecision::backends::montgomery_int_backend<Bits, Bits, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void> > operator"" BOOST_JOIN(_cppi, Bits)()    \
   {                                                                                                                                                                                                                                      \
      typedef typename boost::multiprecision::literals::detail::make_packed_value_from_str<STR...>::type pt;                                                                                                                              \
      return boost::multiprecision::literals::detail::make_backend_from_pack<                                                                                                                                                             \
          pt,                                                                                                                                                                                                                             \
          boost::multiprecision::backends::montgomery_int_backend<Bits, Bits, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void> >::value;                                                                  \
   }                                                                                                                                                                                                                                      \
   template <char... STR>                                                                                                                                                                                                                 \
   constexpr boost::multiprecision::number<boost::multiprecision::backends::montgomery_int_backend<Bits, Bits, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> > operator"" BOOST_JOIN(_cppui, Bits)() \
   {                                                                                                                                                                                                                                      \
      typedef typename boost::multiprecision::literals::detail::make_packed_value_from_str<STR...>::type pt;                                                                                                                              \
      return boost::multiprecision::literals::detail::make_backend_from_pack<                                                                                                                                                             \
          pt,                                                                                                                                                                                                                             \
          boost::multiprecision::backends::montgomery_int_backend<Bits, Bits, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> >::value;                                                                \
   }

BOOST_MP_DEFINE_SIZED_MNT_INT_LITERAL(128)

BOOST_MP_DEFINE_SIZED_MNT_INT_LITERAL(256)

BOOST_MP_DEFINE_SIZED_MNT_INT_LITERAL(512)

BOOST_MP_DEFINE_SIZED_MNT_INT_LITERAL(1024)

} // namespace literals

//
// Overload unary minus operator for constexpr use:
//
template <unsigned MinBits, cpp_int_check_type Checked>
constexpr number<montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>, et_off> operator-(
    const number<montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>, et_off> &a)
{
   return montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>(a.backend(),
                                                                                    boost::multiprecision::literals::detail::make_negate_tag());
}

template <unsigned MinBits, cpp_int_check_type Checked>
constexpr number<montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>, et_off> operator-(
    number<montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>, et_off> &&a)
{
   return montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>(
       static_cast<const number<montgomery_int_backend<MinBits, MinBits, signed_magnitude, Checked, void>,
                                et_off> &>(a)
           .backend(),
       boost::multiprecision::literals::detail::make_negate_tag());
}

}
} // namespace boost::multiprecision

#endif // BOOST_MULTIPRECISION_MP_MONTGOMERY_INT_CORE_HPP
