//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MP_MONTGOMERY_INT_LIM_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_LIM_HPP

namespace std {

namespace detail {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4307)
#endif

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_min(const boost::mpl::true_ &, const boost::mpl::true_ &)
{
   // Bounded and signed.
   typedef boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       result_type;
   typedef boost::multiprecision::number<boost::multiprecision::montgomery_int_backend<MaxBits, MaxBits,
                                                                                       boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked>,
                                         ExpressionTemplates>
       ui_type;
   static const result_type val = -result_type(~ui_type(0));
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_min(const boost::mpl::true_ &, const boost::mpl::false_ &)
{
   // Bounded and unsigned:
   static const boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       val(0u);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_min(const boost::mpl::false_ &, const boost::mpl::true_ &)
{
   // Unbounded and signed.
   // There is no minimum value, just return 0:
   static const boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       val(0u);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_min(const boost::mpl::false_ &, const boost::mpl::false_ &)
{
   // Unbound and unsigned:
   static const boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       val(0u);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_max(const boost::mpl::true_ &, const boost::mpl::true_ &)
{
   // Bounded and signed.
   typedef boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       result_type;
   typedef boost::multiprecision::number<boost::multiprecision::montgomery_int_backend<MaxBits, MaxBits,
                                                                                       boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked>,
                                         ExpressionTemplates>
       ui_type;
   static const result_type val = ~ui_type(0);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_max(const boost::mpl::true_ &, const boost::mpl::false_ &)
{
   // Bound and unsigned:
   typedef boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       result_type;
   typedef boost::multiprecision::number<boost::multiprecision::montgomery_int_backend<MinBits, MaxBits,
                                                                                       boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, Allocator, ParamsBackend>,
                                         ExpressionTemplates>
       ui_type;
   static const result_type val = ~ui_type(0);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_max(const boost::mpl::false_ &, const boost::mpl::true_ &)
{
   // Unbounded and signed.
   // There is no maximum value, just return 0:
   static const boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       val(0u);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates>
get_max(const boost::mpl::false_ &, const boost::mpl::false_ &)
{
   // Unbound and unsigned:
   static const boost::multiprecision::number<
       boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates>
       val(0u);
   return val;
}

} // namespace detail

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
class numeric_limits<boost::multiprecision::number<
    boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
    ExpressionTemplates> >
{
   typedef boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked,
                                                         Allocator>
       backend_type;
   typedef boost::multiprecision::number<backend_type, ExpressionTemplates> number_type;

   struct inititializer
   {
      inititializer()
      {
         (std::numeric_limits<number_type>::max)();
         (std::numeric_limits<number_type>::min)();
      }

      void do_nothing() const
      {
      }
   };

   static const inititializer init;

 public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;

   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   static number_type(min)()
   {
      init.do_nothing();
      return detail::get_min<MinBits, MaxBits, SignType, Checked, Allocator, ExpressionTemplates>(
          boost::multiprecision::backends::is_fixed_precision<backend_type>(),
          boost::multiprecision::is_signed_number<backend_type>());
   }

   static number_type(max)()
   {
      init.do_nothing();
      return detail::get_max<MinBits, MaxBits, SignType, Checked, Allocator, ExpressionTemplates>(
          boost::multiprecision::backends::is_fixed_precision<backend_type>(),
          boost::multiprecision::is_signed_number<backend_type>());
   }

   static number_type lowest()
   {
      return (min)();
   }

   BOOST_STATIC_CONSTEXPR int digits =
       boost::multiprecision::backends::max_precision<backend_type>::value == UINT_MAX ? INT_MAX
                                                                                       : boost::multiprecision::backends::max_precision<
                                                                                             backend_type>::value;
   BOOST_STATIC_CONSTEXPR int  digits10     = (digits > INT_MAX / 301) ? (digits / 1000) * 301L : (digits * 301) / 1000;
   BOOST_STATIC_CONSTEXPR int  max_digits10 = digits10 + 3;
   BOOST_STATIC_CONSTEXPR bool is_signed    = boost::multiprecision::is_signed_number<backend_type>::value;
   BOOST_STATIC_CONSTEXPR bool is_integer   = true;
   BOOST_STATIC_CONSTEXPR bool is_exact     = true;
   BOOST_STATIC_CONSTEXPR int  radix        = 2;

   static number_type epsilon()
   {
      return 0;
   }

   static number_type round_error()
   {
      return 0;
   }

   BOOST_STATIC_CONSTEXPR int  min_exponent                  = 0;
   BOOST_STATIC_CONSTEXPR int  min_exponent10                = 0;
   BOOST_STATIC_CONSTEXPR int  max_exponent                  = 0;
   BOOST_STATIC_CONSTEXPR int  max_exponent10                = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity                  = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN                 = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN             = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm      = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool               has_denorm_loss = false;

   static number_type infinity()
   {
      return 0;
   }

   static number_type quiet_NaN()
   {
      return 0;
   }

   static number_type signaling_NaN()
   {
      return 0;
   }

   static number_type denorm_min()
   {
      return 0;
   }

   BOOST_STATIC_CONSTEXPR bool is_iec559  = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = boost::multiprecision::backends::is_fixed_precision<
       backend_type>::value;
   BOOST_STATIC_CONSTEXPR bool is_modulo                = (boost::multiprecision::backends::is_fixed_precision<backend_type>::value &&
                                            (Checked == boost::multiprecision::unchecked));
   BOOST_STATIC_CONSTEXPR bool traps                    = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before          = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
const typename numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::inititializer numeric_limits<boost::multiprecision::number<boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                                                                           ExpressionTemplates> >::init;

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::digits;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::digits10;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::max_digits10;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_signed;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_integer;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_exact;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::radix;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::min_exponent;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::min_exponent10;
template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::max_exponent;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::max_exponent10;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::has_infinity;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::has_quiet_NaN;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::has_signaling_NaN;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST float_denorm_style
    numeric_limits<boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::has_denorm;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::has_denorm_loss;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_iec559;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_bounded;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::is_modulo;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::traps;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<
    boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::tinyness_before;

template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, boost::multiprecision::cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST float_round_style
    numeric_limits<boost::multiprecision::number<
        boost::multiprecision::montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
        ExpressionTemplates> >::round_style;

#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace std

#endif
