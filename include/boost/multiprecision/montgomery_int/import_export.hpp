//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_INT_IMPORT_EXPORT_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_INT_IMPORT_EXPORT_HPP

//#include <boost/multiprecision/montgomery_int/montgomery_int.hpp>
//#include <boost/multiprecision/montgomery_int/misc.hpp>

namespace boost {
namespace multiprecision {

namespace detail {
template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, class Iterator>
montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &eval_import_bits_generic(montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, Iterator i, Iterator j, unsigned chunk_size = 0,
                                                                                                                bool msv_first = true)
{
   montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> newval;

   typedef typename std::iterator_traits<Iterator>::value_type                                                         value_type;
   typedef typename boost::make_unsigned<value_type>::type                                                             unsigned_value_type;
   typedef typename std::iterator_traits<Iterator>::difference_type                                                    difference_type;
   typedef typename boost::make_unsigned<difference_type>::type                                                        size_type;
   typedef typename montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>::trivial_tag tag_type;

   if (!chunk_size)
   {
      chunk_size = std::numeric_limits<value_type>::digits;
   }

   size_type limbs = std::distance(i, j);
   size_type bits  = limbs * chunk_size;

   detail::resize_to_bit_size(newval, static_cast<unsigned>(bits), tag_type());

   difference_type bit_location        = msv_first ? bits - chunk_size : 0;
   difference_type bit_location_change = msv_first ? -static_cast<difference_type>(chunk_size)
                                                   : chunk_size;

   while (i != j)
   {
      detail::assign_bits(newval, static_cast<unsigned_value_type>(*i),
                          static_cast<unsigned>(bit_location), chunk_size, tag_type());
      ++i;
      bit_location += bit_location_change;
   }

   newval.normalize();

   val.swap(newval);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, typename T>
inline typename boost::disable_if_c<backends::is_trivial_cpp_int<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> >::value,
                                    montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &>::type
eval_import_bits_fast(montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, T *i, T *j, unsigned chunk_size = 0)
{
   std::size_t byte_len = (j - i) * (chunk_size ? chunk_size / CHAR_BIT : sizeof(*i));
   std::size_t limb_len = byte_len / sizeof(limb_type);
   if (byte_len % sizeof(limb_type))
   {
      ++limb_len;
   }
   val.resize(static_cast<unsigned>(limb_len),
              static_cast<unsigned>(limb_len)); // checked types may throw here if they're not large enough to hold the data!
   val.limbs()[val.size() - 1] = 0u;
   std::memcpy(val.limbs(), i, (std::min)(byte_len, val.size() * sizeof(limb_type)));
   val.normalize(); // In case data has leading zeros.
   return val;
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, class T>
inline typename boost::enable_if_c<backends::is_trivial_cpp_int<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> >::value,
                                   montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &>::type
eval_import_bits_fast(montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, T *i, T *j, unsigned chunk_size = 0)
{
   std::size_t byte_len = (j - i) * (chunk_size ? chunk_size / CHAR_BIT : sizeof(*i));
   std::size_t limb_len = byte_len / sizeof(val.limbs()[0]);
   if (byte_len % sizeof(val.limbs()[0]))
   {
      ++limb_len;
   }
   val.limbs()[0] = 0u;
   val.resize(static_cast<unsigned>(limb_len),
              static_cast<unsigned>(limb_len)); // checked types may throw here if they're not large enough to hold the data!
   std::memcpy(val.limbs(), i, (std::min)(byte_len, val.size() * sizeof(val.limbs()[0])));
   val.normalize(); // In case data has leading zeros.
   return val;
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class Iterator>
number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
       ExpressionTemplates> &
import_bits_generic(
    number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
           ExpressionTemplates> &val,
    Iterator i, Iterator j, unsigned chunk_size = 0,
    bool msv_first = true)
{
   typename number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                   ExpressionTemplates>::backend_type newval;

   typedef typename std::iterator_traits<Iterator>::value_type      value_type;
   typedef typename boost::make_unsigned<value_type>::type          unsigned_value_type;
   typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
   typedef typename boost::make_unsigned<difference_type>::type     size_type;
   typedef typename montgomery_int_backend<MinBits, MaxBits, SignType, Checked,
                                           Allocator>::trivial_tag  tag_type;

   if (!chunk_size)
   {
      chunk_size = std::numeric_limits<value_type>::digits;
   }

   size_type limbs = std::distance(i, j);
   size_type bits  = limbs * chunk_size;

   detail::resize_to_bit_size(newval, static_cast<unsigned>(bits), tag_type());

   difference_type bit_location        = msv_first ? bits - chunk_size : 0;
   difference_type bit_location_change = msv_first ? -static_cast<difference_type>(chunk_size)
                                                   : chunk_size;

   while (i != j)
   {
      detail::assign_bits(newval, static_cast<unsigned_value_type>(*i),
                          static_cast<unsigned>(bit_location), chunk_size, tag_type());
      ++i;
      bit_location += bit_location_change;
   }

   newval.normalize();

   val.backend().swap(newval);
   return val;
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class T>
inline typename boost::disable_if_c<boost::multiprecision::backends::is_trivial_montgomery_int<
                                        montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> >::value,
                                    number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                           ExpressionTemplates> &>

    ::type import_bits_fast(number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                   ExpressionTemplates> &val,
                            T *i, T *j, unsigned chunk_size = 0)
{
   std::size_t byte_len = (j - i) * (chunk_size ? chunk_size / CHAR_BIT : sizeof(*i));
   std::size_t limb_len = byte_len / sizeof(limb_type);
   if (byte_len % sizeof(limb_type))
   {
      ++limb_len;
   }
   montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &result = val.backend();
   result.resize(static_cast<unsigned>(limb_len),
                 static_cast<unsigned>(limb_len)); // checked types may throw here if they're not large enough to hold the data!
   result.limbs()[result.size() - 1] = 0u;
   std::memcpy(result.limbs(), i, (std::min)(byte_len, result.size() * sizeof(limb_type)));
   result.normalize(); // In case data has leading zeros.
   return val;
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class T>
inline typename boost::enable_if_c<boost::multiprecision::backends::is_trivial_montgomery_int<
                                       montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> >::value,
                                   number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                          ExpressionTemplates> &>

    ::type import_bits_fast(number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                   ExpressionTemplates> &val,
                            T *i, T *j, unsigned chunk_size = 0)
{
   montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &result   = val.backend();
   std::size_t                                                                            byte_len = (j - i) * (chunk_size ? chunk_size / CHAR_BIT : sizeof(*i));
   std::size_t                                                                            limb_len = byte_len / sizeof(result.limbs()[0]);
   if (byte_len % sizeof(result.limbs()[0]))
   {
      ++limb_len;
   }
   result.limbs()[0] = 0u;
   result.resize(static_cast<unsigned>(limb_len),
                 static_cast<unsigned>(limb_len)); // checked types may throw here if they're not large enough to hold the data!
   std::memcpy(result.limbs(), i, (std::min)(byte_len, result.size() * sizeof(result.limbs()[0])));
   result.normalize(); // In case data has leading zeros.
   return val;
}
} // namespace detail

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, class Iterator>
inline montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &eval_import_bits(montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, Iterator i, Iterator j, unsigned chunk_size = 0,
                                                                                                               bool msv_first = true)
{
   return detail::eval_import_bits_generic(val, i, j, chunk_size, msv_first);
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, class T>
inline montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &eval_import_bits(montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, T *i, T *j, unsigned chunk_size = 0, bool msv_first = true)
{
#ifdef BOOST_LITTLE_ENDIAN
   if (((chunk_size % CHAR_BIT) == 0) && !msv_first)
   {
      return detail::eval_import_bits_fast(val, i, j, chunk_size);
   }
#endif
   return detail::eval_import_bits_generic(val, i, j, chunk_size, msv_first);
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, class OutputIterator>
OutputIterator eval_export_bits(const montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend> &val, OutputIterator out, unsigned chunk_size,
                                bool msv_first = true)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

   typedef typename montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>::trivial_tag tag_type;
   if (eval_is_zero(val))
   {
      *out = 0;
      ++out;
      return out;
   }
   unsigned bitcount = backends::eval_msb_imp(val) + 1;
   unsigned chunks   = bitcount / chunk_size;
   if (bitcount % chunk_size)
   {
      ++chunks;
   }

   int bit_location = msv_first ? bitcount - chunk_size : 0;
   int bit_step     = msv_first ? -static_cast<int>(chunk_size) : chunk_size;
   while (bit_location % bit_step)
   {
      ++bit_location;
   }

   do
   {
      *out = detail::extract_bits(val, bit_location, chunk_size, tag_type());
      ++out;
      bit_location += bit_step;
   } while ((bit_location >= 0) && (bit_location < (int)bitcount));

   return out;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class Iterator>
inline number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
              ExpressionTemplates> &
import_bits(
    number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
           ExpressionTemplates> &val,
    Iterator i, Iterator j, unsigned chunk_size = 0,
    bool msv_first = true)
{
   return detail::import_bits_generic(val, i, j, chunk_size, msv_first);
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class T>
inline number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
              ExpressionTemplates> &
import_bits(
    number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
           ExpressionTemplates> &val,
    T *i, T *j, unsigned chunk_size = 0, bool msv_first = true)
{
#ifdef BOOST_LITTLE_ENDIAN
   if (((chunk_size % CHAR_BIT) == 0) && !msv_first)
   {
      return detail::import_bits_fast(val, i, j, chunk_size);
   }
#endif
   return detail::import_bits_generic(val, i, j, chunk_size, msv_first);
}

template <unsigned MinBits, unsigned MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked,
          class Allocator, typename ParamsBackend, expression_template_option ExpressionTemplates, class OutputIterator>
OutputIterator export_bits(const number<montgomery_int_backend<MinBits, MaxBits, SignType, Checked, Allocator, ParamsBackend>,
                                        ExpressionTemplates> &val,
                           OutputIterator out, unsigned chunk_size, bool msv_first = true)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
   typedef typename montgomery_int_backend<MinBits, MaxBits, SignType, Checked,
                                           Allocator>::trivial_tag tag_type;
   if (!val)
   {
      *out = 0;
      ++out;
      return out;
   }
   unsigned bitcount = boost::multiprecision::backends::eval_msb_imp(val.backend()) + 1;
   unsigned chunks   = bitcount / chunk_size;
   if (bitcount % chunk_size)
   {
      ++chunks;
   }

   int bit_location = msv_first ? bitcount - chunk_size : 0;
   int bit_step     = msv_first ? -static_cast<int>(chunk_size) : chunk_size;
   while (bit_location % bit_step)
   {
      ++bit_location;
   }

   do
   {
      *out = detail::extract_bits(val.backend(), bit_location, chunk_size, tag_type());
      ++out;
      bit_location += bit_step;
   } while ((bit_location >= 0) && (bit_location < (int)bitcount));

   return out;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

}
} // namespace boost::multiprecision

#endif // BOOST_MULTIPRECISION_MP_MONTGOMERY_INT_IMPORT_EXPORT_HPP
