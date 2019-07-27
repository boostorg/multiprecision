#ifndef BOOST_MULTIPRECISION_MASK_BITS_HPP
#define BOOST_MULTIPRECISION_MASK_BITS_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/number_base.hpp>
#include <boost/multiprecision/number.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend, typename Integer>
void eval_mask_bits(Backend& val, Integer n)
{
   typedef typename boost::multiprecision::limb_type limb_type;

   typedef typename boost::multiprecision::detail::canonical<unsigned, Backend>::type ui_type;
   static const ui_type                                                               zero = 0u;

   if (n == 0)
   {
      val = zero;
      return;
   }

   const size_t    top_word = n / Backend::limb_bits;
   const limb_type mask     = (limb_type(1) << (n % Backend::limb_bits)) - 1;

   if (top_word < val.size())
   {
      const size_t len = val.size() - (top_word + 1);
      if (len > 0)
      {
         //clear_mem(&val.limbs()[top_word + 1], len); #TODO return this
      }
      val.limbs()[top_word] &= mask;
   }
}
}
} // namespace boost::multiprecision

#endif //BOOST_MULTIPRECISION_MASK_BITS_HPP
