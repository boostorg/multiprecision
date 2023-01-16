#include <boost/multiprecision/cpp_int.hpp>

int main()
{
   using namespace boost::multiprecision;

   __int128 ii128{ 2 };

   cpp_int i(ii128), j;
   j = ii128;


   return 0;
}

