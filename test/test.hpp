
#ifndef BOOST_MULTIPRECISION_TEST_HPP
#define BOOST_MULTIPRECISION_TEST_HPP

template <class T>
T relative_error(T a, T b)
{
   T min_val = (std::numeric_limits<T>::min)();
   T max_val = (std::numeric_limits<T>::max)();

   if((a != 0) && (b != 0))
   {
      // TODO: use isfinite:
      if(fabs(b) >= max_val)
      {
         if(fabs(a) >= max_val)
            return 0;  // one infinity is as good as another!
      }
      // If the result is denormalised, treat all denorms as equivalent:
      if((a < min_val) && (a > 0))
         a = min_val;
      else if((a > -min_val) && (a < 0))
         a = -min_val;
      if((b < min_val) && (b > 0))
         b = min_val;
      else if((b > -min_val) && (b < 0))
         b = -min_val;
      return (std::max)(fabs((a-b)/a), fabs((a-b)/b)) / std::numeric_limits<T>::epsilon();
   }

   // Handle special case where one or both are zero:
   if(min_val == 0)
      return fabs(a-b);
   if(fabs(a) < min_val)
      a = min_val;
   if(fabs(b) < min_val)
      b = min_val;
   return (std::max)(fabs((a-b)/a), fabs((a-b)/b)) / std::numeric_limits<T>::epsilon();
}

#endif
