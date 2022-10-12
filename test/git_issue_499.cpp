///////////////////////////////////////////////////////////////////////////////
// Copyright Matthew Borland 2022.
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>

using boost::multiprecision::cpp_dec_float_50;

int main()
{
    try
    {
        cpp_dec_float_50 val {"wrong"};
        std::cout << "no exception. val=" << val << "\n";
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "std::runtime_error. what():" << error.what() << "\n";
        return 0;
    }
    catch (const std::invalid_argument& error)
    {
        std::cout << "std::invalid_argument. what():" << error.what() << "\n";
        return 1;
    }
}
