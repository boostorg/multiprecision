///////////////////////////////////////////////////////////////////////////////
//  Copyright 2023 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_DETAIL_FORMAT_HPP
#define BOOST_MP_DETAIL_FORMAT_HPP

#ifdef BOOST_NO_CXX20_HDR_FORMAT
#error "This header should not be included in C++20 mode"
#endif

#include <boost/multiprecision/detail/number_base.hpp>
#include <format>
#include <string>

namespace boost {
   namespace multiprecision
   {
      namespace detail
      {
         template <class Number>
         std::string print_binary_string(const Number&)
         {
            throw std::format_error("Binary string not supported for this number type.");
         }
         template <std::size_t MinBits, std::size_t MaxBits, boost::multiprecision::cpp_integer_type SignType, cpp_int_check_type Checked, class Allocator, expression_template_option ExpressionTemplates>
         std::string print_binary_string(const number<cpp_int_backend<MinBits, MaxBits, SignType, Checked, Allocator>, ExpressionTemplates>& value)
         {
            std::string result;

            if(value < 0)
               throw std::format_error("Binary string not supported negative values in sign-magnitude format.");
            const limb_type* plimbs = value.backend().limbs();
            std::size_t count = value.backend().size();

            // Format each limb:
            for (int i = count - 1; i >= 0; --i)
               result += std::format("{:0{}b}", plimbs[i], sizeof(limb_type) * CHAR_BIT);
            
            // remove leading zeros:
            std::string::size_type pos = result.find('1');
            if (pos != std::string::npos)
               result.erase(0, pos);
            
            return result;
         }
         template <expression_template_option ExpressionTemplates>
         std::string print_binary_string(const number<gmp_int, ExpressionTemplates>& value);

         template <class charT>
         inline std::basic_string<charT> make_wide_string(const std::string& s)
         {
            return std::basic_string<charT>(s.begin(), s.end());
         }
         template<>
         inline std::basic_string<char> make_wide_string<char>(const std::string& s)
         {
            return s;
         }

         template <class charT>
         void insert_locale_grouping(std::basic_string<charT>& s, const std::locale& loc, std::size_t start)
         {
            const std::numpunct<charT>& facet = std::use_facet<std::numpunct<charT>>(loc);
            charT thousands_separator = facet.thousands_sep();
            std::string punct = facet.grouping();
            if (punct.empty())
               return;
            std::size_t current_separator = 0;
            std::size_t current_position = s.size();

            while ((punct[current_separator] <= current_position) && (current_position - punct[current_separator] > start))
            {
               current_position -= punct[current_separator];
               s.insert(current_position, 1, thousands_separator);
               if (current_separator + 1 < punct.size())
                  ++current_separator;
            }
         }


         template <class Number, class charT, int Category>
         class number_formatter
         {
            enum fill_kind { none, left, right, centre };
         public:
            constexpr typename std::basic_format_parse_context<charT>::iterator parse(const std::basic_format_parse_context<charT>& context)
            {
               auto start = context.begin();
               auto end = context.end();

               if ((start != end) && (start + 1 != end))
               {
                  // We may have fill field:
                  if (*(start + 1) == '<')
                  {
                     align = left;
                     fill = *start;
                     start += 2;
                  }
                  else if (*(start + 1) == '>')
                  {
                     align = right;
                     fill = *start;
                     start += 2;
                  }
                  else if (*(start + 1) == '^')
                  {
                     align = centre;
                     fill = *start;
                     start += 2;
                  }
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // Start looking for alignments, with no fill specified (we'll use a space):
               if (*start == '<')
               {
                  align = left;
                  ++start;
               }
               else if (*start == '>')
               {
                  align = right;
                  ++start;
               }
               else if (*start == '^')
               {
                  align = centre;
                  ++start;
               }
               if ((start == end) || (*start == '}'))
                  return start;
               
               // Sign handling:
               if (*start == '+')
               {
                  flags |= std::ios_base::showpos;
                  ++start;
               }
               else if (*start == '-')
               {
                  ++start;
               }
               else if (*start == ' ')
               {
                  space_before_positive = true;
                  ++start;
               }
               else if (*start == '#')
               {
                  flags |= std::ios_base::showbase;
                  ++start;
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // zero padding:
               if (*start == '0')
               {
                  pad_with_zeros = true;
                  ++start;
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // Width:
               if ((*start >= '0') && (*start <= '9'))
               {
                  width = *start - '0';
                  while ((++start != end) && (*start >= '0') && (*start <= '9'))
                  {
                     width *= 10;
                     width += *start - '0';
                  }
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // Precision:
               if (*start == '.')
               {
                  ++start;
                  if ((*start >= '0') && (*start <= '9'))
                  {
                     precision = *start - '0';
                     while ((++start != end) && (*start >= '0') && (*start <= '9'))
                     {
                        precision *= 10;
                        precision += *start - '0';
                     }
                  }
                  else
                     throw std::format_error("Precision specifier followed by non-numeric context");
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // Locale:
               if (*start == 'L')
               {
                  use_locale = true;
                  ++start;
               }
               if ((start == end) || (*start == '}'))
                  return start;

               // Type format:
               if (*start == 'B')
               {
                  binary_out = true;
                  flags |= std::ios_base::uppercase;
                  ++start;
               }
               else if (*start == 'b')
               {
                  binary_out = true;
                  ++start;
               }
               else if (*start == 'c')
               {
                  char_out = true;
                  ++start;
               }
               else if (*start == 'd')
               {
                  ++start;
               }
               else if (*start == 'o')
               {
                  flags |= std::ios_base::oct;
                  ++start;
               }
               else if (*start == 'x')
               {
                  flags |= std::ios_base::hex;
                  ++start;
               }
               else if (*start == 'X')
               {
                  flags |= std::ios_base::hex | std::ios_base::uppercase;
                  ++start;
               }

               // Error handling:
               if ((start != end) && (*start != '}'))
               {
                  // We have an error:
#ifndef BOOST_NO_EXCEPTIONS
                  std::string msg1("Unexpected format specifier \""), msg2("\" encountered while formatting Boost.Multiprecision integer type");
                  throw std::format_error(msg1 + static_cast<char>(*start) + msg2);
#else
         // All we can do is ignore the erroneous characters!
                  while ((start != end) && (*start != '}'))
                     ++start;
#endif
               }
               return start;
            }

            template <class OutputIterator>
            OutputIterator format(const Number& value, std::basic_format_context<OutputIterator, charT>& context)const
            {
               auto&& out = context.out();
               std::basic_string<charT> s;
               if (char_out)
               {
                  static const Number max = std::numeric_limits<charT>::max();
                  static const Number min = std::numeric_limits<Number>::is_signed ? std::numeric_limits<charT>::min() : 0;
                  if((value <= max) && (value >= min))
                     s = static_cast<charT>(value);
                  else                     
                     throw std::format_error("Value was outside the range of type charT");
               }
               else if (binary_out)
               {
                  if (flags & std::ios_base::showbase)
                  {
                     if (flags & std::ios_base::uppercase)
                     {
                        static const charT c[] = { '0', 'B', 0 };
                        s = c;
                     }
                     else
                     {
                        static const charT c[] = { '0', 'b', 0 };
                        s = c;
                     }
                  }
                  s += make_wide_string<charT>(print_binary_string(value));
               }
               else
                  s = make_wide_string<charT>(value.str(precision, flags));
               // Locale specific grouping:
               if (use_locale)
               {
                  std::size_t start = 0;
                  if (flags & std::ios_base::showbase)
                  {
                     if (binary_out)
                        start = 2;
                     else if (flags & std::ios_base::hex)
                        start = 2;
                     else if (flags & std::ios_base::oct)
                        start = 1;
                  }
                  insert_locale_grouping(s, context.locale(), start);
               }
               if (space_before_positive && (value > 0))
                  s.insert(0, 1, static_cast<charT>(' '));
               else if((flags & std::ios_base::showpos) && (s[0] != '+') && (s[0] != '-'))
                  s.insert(0, 1, static_cast<charT>('+'));
               if ((width > s.size()) && (align == none) && pad_with_zeros)
               {
                  std::size_t pos = 0;
                  if ((s[0] == ' ') || (s[0] == '+') || (s[0] == '-'))
                     ++pos;
                  s.insert(pos, width - s.size(), static_cast<charT>('0'));
               }
               std::size_t n_fill_left{ 0 }, n_fill_right{ 0 };
               if (width > s.size())
               {
                  if (align == centre)
                  {
                     n_fill_left = (width - s.size()) / 2;
                     n_fill_right = width - s.size() - n_fill_left;
                  }
                  else if (align == left)
                     n_fill_right = width - s.size();
                  else if (align == right)
                     n_fill_left = width - s.size();
               }
               if (n_fill_left)
                  out = std::fill_n(out, n_fill_left, fill);
               out = std::copy(s.begin(), s.end(), out);
               if (n_fill_right)
                  out = std::fill_n(out, n_fill_right, fill);
               return out;
            }
         private:
            std::ios_base::fmtflags flags = std::ios_base::fmtflags(0);
            charT fill = ' ';
            fill_kind align = none;
            bool space_before_positive = false;
            std::size_t width = 0;
            std::size_t precision = 6;
            bool use_locale = false;
            bool binary_out = false;
            bool char_out = false;
            bool pad_with_zeros = false;
         };

      }
   }
}

namespace std
{
   template <class Backend, boost::multiprecision::expression_template_option ExpressionTemplates, class charT>
   struct formatter<boost::multiprecision::number<Backend, ExpressionTemplates>, charT> : public boost::multiprecision::detail::number_formatter<boost::multiprecision::number<Backend, ExpressionTemplates>, charT, boost::multiprecision::number_category<boost::multiprecision::number<Backend, ExpressionTemplates>>::value> {};
}

#endif // BOOST_MP_DETAIL_FORMAT_HPP
