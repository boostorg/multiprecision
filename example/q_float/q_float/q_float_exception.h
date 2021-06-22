///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef _Q_FLOAT_EXCEPTION_2005_03_26_H_
  #define _Q_FLOAT_EXCEPTION_2005_03_26_H_

  #include <stdexcept>

  namespace qf
  {
    // Support for C++ exceptions.
    struct exception : public std::exception
    {
      virtual const char* what(void) const throw() { return "q_float exception"; }
    };

    struct exception_BadIO : public qf::exception
    {
      virtual const char* what(void) const throw() { return "q_float exception bad I/O operation"; }
    };

    struct exception_inf : public qf::exception
    {
      virtual const char* what(void) const throw() { return "q_float exception infinite"; }
    };
    
    struct exception_nan : public qf::exception
    {
      virtual const char* what(void) const throw() { return "q_float exception NaN"; }
    };
  }

#endif // _Q_FLOAT_EXCEPTION_2005_03_26_H_
