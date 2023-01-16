#include <boost/multiprecision/cpp_int.hpp>

template <class T>
struct convertible_to
{
   operator T() const;
};


int main()
{
   using namespace boost::multiprecision;

   static_assert(std::is_constructible<cpp_int, __int128>::value, "oops");
   static_assert(boost::multiprecision::detail::is_convertible_arithmetic<__int128, cpp_int::backend_type>::value, "oops");
   static_assert(!boost::multiprecision::detail::is_restricted_conversion<typename detail::canonical<__int128, cpp_int::backend_type>::type, cpp_int::backend_type>::value, "oops");


   return 0;
}

