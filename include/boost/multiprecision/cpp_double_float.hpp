///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
#define BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP

// Tradition is to put boost config as the first included header file
#include <boost/config.hpp>

#include <type_traits>
// Other Standard Library headers go here

#include <boost/multiprecision/number.hpp>
// Other needed Boost headers come here.

namespace boost { namespace multiprecision {

namespace backends {

template<typename FloatingPointType>
class cpp_double_float;

} // namespace backends

template<typename FloatingPointType>
struct number_category<backends::cpp_double_float<FloatingPointType>>
  : public std::integral_constant<int, number_kind_floating_point>
{
};

namespace backends {

// TBD: Add the template class double_float.
// Step 1 Add double_float.
// Step 2 We should probably rename to something like cpp_double_float.

} } } // namespace boost::multiprecision::backends

#endif // BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
