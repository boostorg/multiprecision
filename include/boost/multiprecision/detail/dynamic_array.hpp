///////////////////////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock.
//  Copyright Christopher Kormanyos 2013. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_DETAIL_DYNAMIC_ARRAY_HPP
#define BOOST_MP_DETAIL_DYNAMIC_ARRAY_HPP

#include <algorithm>
#include <initializer_list>
#include <vector>
#include <boost/multiprecision/detail/rebind.hpp>

namespace boost { namespace multiprecision { namespace backends { namespace detail {
template <class value_type, const boost::uint32_t elem_number, class my_allocator>
struct dynamic_array : public std::vector<value_type, typename rebind<value_type, my_allocator>::type>
{
private:
   using base_class_type = std::vector<value_type, typename rebind<value_type, my_allocator>::type>;

public:
   dynamic_array() : base_class_type(static_cast<typename base_class_type::size_type>(elem_number), static_cast<value_type>(0))
   {
   }

   dynamic_array( std::initializer_list<value_type> lst ) : base_class_type(static_cast<typename base_class_type::size_type>(elem_number), static_cast<value_type>(0))
   {
     std::copy(lst.begin(),
               lst.end(),
               base_class_type::begin());
   }

   value_type*       data() { return &(*(this->begin())); }
   const value_type* data() const { return &(*(this->begin())); }
};
}}}} // namespace boost::multiprecision::backends::detail

#endif // BOOST_MP_DETAIL_DYNAMIC_ARRAY_HPP
