///////////////////////////////////////////////////////////////
//  Copyright Beman Dawes 1994-99.
//  Copyright 2020 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include <algorithm>
#include <ctime>
#include <limits>

//
// This file archives the old (now deprecated) Boost.Timer.
// It is however, all that we need for a simple timeout.
//
// TODO: replace with std::chrono once we remove C++03
// support in 2021.
//

class timer
{
 public:
   timer() : _start_time(std::clock()) { }          // postcondition: elapsed()==0
                                                    //         timer( const timer& src );      // post: elapsed()==src.elapsed()
                                                    //        ~timer(){}
                                                    //  timer& operator=( const timer& src );  // post: elapsed()==src.elapsed()
   void   restart() { _start_time = std::clock(); } // post: elapsed()==0
   double elapsed() const                           // return elapsed time in seconds
   {
      const auto delta =
         static_cast<std::clock_t>
         (
            std::clock() - _start_time
         );

      return static_cast<double>(static_cast<double>(delta) / safe_demominator());
   }

   double elapsed_max() const // return estimated maximum value for elapsed()
   // Portability warning: elapsed_max() may return too high a value on systems
   // where std::clock_t overflows or resets at surprising values.
   {
      const auto delta_max =
         static_cast<std::clock_t>
         (
            (std::numeric_limits<std::clock_t>::max)() - _start_time
         );

      return static_cast<double>(static_cast<double>(delta_max) / safe_demominator());
   }

   double elapsed_min() const // return minimum value for elapsed()
   {
      return static_cast<double>(1.0 / safe_demominator());
   }

 private:
   std::clock_t _start_time;

   static double safe_demominator()
   {
      return (std::max)(10.0, static_cast<double>(CLOCKS_PER_SEC));
   }
}; // timer
