///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#ifndef Q_FLOAT_TYPES_2005_03_26_H_
  #define Q_FLOAT_TYPES_2005_03_26_H_

  #if !defined(_MSC_VER) && !defined(__GNUC__)
    #error "Compiler not supported: Types can not be determined"
  #endif

  #if defined(_MSC_VER) && (_MSC_VER <= 1200)
    #include <iomanip>
  #endif // MSVC 6.0

  #ifdef TRUE
    #undef TRUE
  #endif

  #ifdef FALSE
    #undef FALSE
  #endif
  
  #if defined(_MSC_VER)

    #ifndef _WINDEF_

      typedef          int    INT32;
      typedef unsigned int   UINT32;
      typedef   signed short  INT16;
      typedef unsigned short UINT16;

    #endif

    typedef   signed __int64  INT64;
    typedef unsigned __int64 UINT64;

  #elif defined(__GNUC__)

    typedef   signed long long  INT64;
    typedef unsigned long long UINT64;
    typedef   signed int        INT32;
    typedef unsigned int       UINT32;
    typedef   signed short      INT16;
    typedef unsigned short     UINT16;

  #endif

#endif // Q_FLOAT_TYPES_2005_03_26_H_
