///////////////////////////////////////////////////////////////////////////////
// Copyright Matthew Borland 2022.
// Distributed under the Boost Software License, Version 1.0. (See accompanying file
// LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/core/lightweight_test.hpp>

using boost::multiprecision::cpp_dec_float_50;

int main()
{
    std::size_t counter {};
    
    // All character strings case
    try
    {
        cpp_dec_float_50 val {"wrong"};
        std::cout << "no exception. val=" << val << "\n";
    }
    catch (const std::runtime_error& error)
    {
        std::cout << "std::runtime_error. what():" << error.what() << "\n";
    }
    catch (const std::invalid_argument& error)
    {
        std::cout << "std::invalid_argument. what():" << error.what() << "\n";
        ++counter;
    }

    // Malformed expressions
    try
    {
        cpp_dec_float_50 malformed {"3.4e1a2"};
        std::cout << "no exception. val=" << malformed << "\n";
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "std::runtime_error. what():" << e.what() << "\n";
    }
    catch (...)
    {
        ++counter;
    }

    try
    {
        cpp_dec_float_50 val1{"12a3.4"};
        std::cout << "no exception. val=" << val1 << "\n";
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "std::runtime_error. what():" << e.what() << "\n";
    }
    catch (...)
    {
        ++counter;
    }

    try
    {
        cpp_dec_float_50 val2{"1.2a34"};
        std::cout << "no exception. val=" << val2 << "\n";
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "std::runtime_error. what():" << e.what() << "\n";
    }
    catch (...)
    {
        ++counter;
    }

    if (counter != 0)
    {
        return 1;
    }

    return 0;
}
