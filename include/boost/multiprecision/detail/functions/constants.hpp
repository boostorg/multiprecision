// Copyright 2011 John Maddock. Distributed under the Boost
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// This file has no include guards or namespaces - it's expanded inline inside
// default_ops.hpp
//

template <class T> void calc_log2(T &num, unsigned digits) {
  typedef typename boost::multiprecision::detail::canonical<boost::uint32_t,
                                                            T>::type ui_type;
  typedef typename mpl::front<typename T::signed_types>::type si_type;

  //
  // String value with 1100 digits:
  //
  static const char *string_val =
      "0."
      "693147180559945309417232121458176568075500134360255254120680009493393621"
      "9696947156058633269964186875"
      "420014810205706857336855202357581305570326707516350759619307275708283714"
      "3519030703862389167347112335"
      "011536449795523912047517268157493206515552473413952588295045300709532636"
      "6642654104239157814952043740"
      "430385500801944170641671518644712839968171784546957026271631064546150257"
      "2074024816377733896385506952"
      "606683411372738737229289564935470257626520988596932019650585547647033067"
      "9365443254763274495125040606"
      "943814710468994650622016772042452452961268794654619316517468139267250410"
      "3802546259656869144192871608"
      "293803172714367782654877566485085674077648451464439940461422603193096735"
      "4025744460703080960850474866"
      "385231381816767514386674766478908814371419854942315199735488037516586127"
      "5352916610007105355824987941"
      "472950929311389715599820565439287170007218085761025236889213244971389320"
      "3784393530887748259701715591"
      "070882368362758984258918535302436342143670611892367891923723146723217205"
      "3401649256872747782344535347"
      "648114941864238677677440606956265737960086707625719918473402265146283790"
      "4883062033061144630073719489";
  //
  // Check if we can just construct from string:
  //
  if (digits < 3640) // 3640 binary digits ~ 1100 decimal digits
  {
    num = string_val;
    return;
  }
  //
  // We calculate log2 from using the formula:
  //
  // ln(2) = 3/4 SUM[n>=0] ((-1)^n * N!^2 / (2^n(2n+1)!))
  //
  // Numerator and denominator are calculated separately and then
  // divided at the end, we also precalculate the terms up to n = 5
  // since these fit in a 32-bit integer anyway.
  //
  // See Gourdon, X., and Sebah, P. The logarithmic constant: log 2, Jan. 2004.
  // Also http://www.mpfr.org/algorithms.pdf.
  //
  num = static_cast<ui_type>(1180509120uL);
  T denom, next_term, temp;
  denom = static_cast<ui_type>(1277337600uL);
  next_term = static_cast<ui_type>(120uL);
  si_type sign = -1;

  ui_type limit = digits / 3 + 1;

  for (ui_type n = 6; n < limit; ++n) {
    temp = static_cast<ui_type>(2);
    eval_multiply(temp, ui_type(2 * n));
    eval_multiply(temp, ui_type(2 * n + 1));
    eval_multiply(num, temp);
    eval_multiply(denom, temp);
    sign = -sign;
    eval_multiply(next_term, n);
    eval_multiply(temp, next_term, next_term);
    if (sign < 0)
      temp.negate();
    eval_add(num, temp);
  }
  eval_multiply(denom, ui_type(4));
  eval_multiply(num, ui_type(3));
  INSTRUMENT_BACKEND(denom);
  INSTRUMENT_BACKEND(num);
  eval_divide(num, denom);
  INSTRUMENT_BACKEND(num);
}

template <class T> void calc_e(T &result, unsigned digits) {
  typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
  //
  // 1100 digits in string form:
  //
  const char *string_val = "2."
                           "718281828459045235360287471352662497757247093699959"
                           "5749669676277240766303535475945713821785251664274"
                           "274663919320030599218174135966290435729003342952605"
                           "9563073813232862794349076323382988075319525101901"
                           "157383418793070215408914993488416750924476146066808"
                           "2264800168477411853742345442437107539077744992069"
                           "551702761838606261331384583000752044933826560297606"
                           "7371132007093287091274437470472306969772093101416"
                           "928368190255151086574637721112523897844250569536967"
                           "7078544996996794686445490598793163688923009879312"
                           "773617821542499922957635148220826989519366803318252"
                           "8869398496465105820939239829488793320362509443117"
                           "301238197068416140397019837679320683282376464804295"
                           "3118023287825098194558153017567173613320698112509"
                           "961818815930416903515988885193458072738667385894228"
                           "7922849989208680582574927961048419844436346324496"
                           "848756023362482704197862320900216099023530436994184"
                           "9146314093431738143640546253152096183690888707016"
                           "768396424378140592714563549061303107208510383750510"
                           "1157477041718986106873969655212671546889570350354"
                           "021234078498193343210681701210056278802351930332247"
                           "4501585390473041995777709350366041699732972508869";
  //
  // Check if we can just construct from string:
  //
  if (digits < 3640) // 3640 binary digits ~ 1100 decimal digits
  {
    result = string_val;
    return;
  }

  T lim;
  lim = ui_type(1);
  eval_ldexp(lim, lim, digits);

  //
  // Standard evaluation from the definition of e:
  // http://functions.wolfram.com/Constants/E/02/
  //
  result = ui_type(2);
  T denom;
  denom = ui_type(1);
  ui_type i = 2;
  do {
    eval_multiply(denom, i);
    eval_multiply(result, i);
    eval_add(result, ui_type(1));
    ++i;
  } while (denom.compare(lim) <= 0);
  eval_divide(result, denom);
}

template <class T> void calc_pi(T &result, unsigned digits) {
  typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
  typedef typename mpl::front<typename T::float_types>::type real_type;
  //
  // 1100 digits in string form:
  //
  const char *string_val = "3."
                           "141592653589793238462643383279502884197169399375105"
                           "8209749445923078164062862089986280348253421170679"
                           "821480865132823066470938446095505822317253594081284"
                           "8111745028410270193852110555964462294895493038196"
                           "442881097566593344612847564823378678316527120190914"
                           "5648566923460348610454326648213393607260249141273"
                           "724587006606315588174881520920962829254091715364367"
                           "8925903600113305305488204665213841469519415116094"
                           "330572703657595919530921861173819326117931051185480"
                           "7446237996274956735188575272489122793818301194912"
                           "983367336244065664308602139494639522473719070217986"
                           "0943702770539217176293176752384674818467669405132"
                           "000568127145263560827785771342757789609173637178721"
                           "4684409012249534301465495853710507922796892589235"
                           "420199561121290219608640344181598136297747713099605"
                           "1870721134999999837297804995105973173281609631859"
                           "502445945534690830264252230825334468503526193118817"
                           "1010003137838752886587533208381420617177669147303"
                           "598253490428755468731159562863882353787593751957781"
                           "8577805321712268066130019278766111959092164201989"
                           "380952572010654858632788659361533818279682303019520"
                           "3530185296899577362259941389124972177528347913152";
  //
  // Check if we can just construct from string:
  //
  if (digits < 3640) // 3640 binary digits ~ 1100 decimal digits
  {
    result = string_val;
    return;
  }

  T a;
  a = ui_type(1);
  T b;
  T A(a);
  T B;
  B = real_type(0.5f);
  T D;
  D = real_type(0.25f);

  T lim;
  lim = ui_type(1);
  eval_ldexp(lim, lim, -(int)digits);

  //
  // This algorithm is from:
  // Schonhage, A., Grotefeld, A. F. W., and Vetter, E. Fast Algorithms: A
  // Multitape Turing Machine Implementation. BI Wissenschaftverlag, 1994. Also
  // described in MPFR's algorithm guide: http://www.mpfr.org/algorithms.pdf.
  //
  // Let:
  // a[0] = A[0] = 1
  // B[0] = 1/2
  // D[0] = 1/4
  // Then:
  // S[k+1] = (A[k]+B[k]) / 4
  // b[k] = sqrt(B[k])
  // a[k+1] = a[k]^2
  // B[k+1] = 2(A[k+1]-S[k+1])
  // D[k+1] = D[k] - 2^k(A[k+1]-B[k+1])
  // Stop when |A[k]-B[k]| <= 2^(k-p)
  // and PI = B[k]/D[k]

  unsigned k = 1;

  do {
    eval_add(result, A, B);
    eval_ldexp(result, result, -2);
    eval_sqrt(b, B);
    eval_add(a, b);
    eval_ldexp(a, a, -1);
    eval_multiply(A, a, a);
    eval_subtract(B, A, result);
    eval_ldexp(B, B, 1);
    eval_subtract(result, A, B);
    bool neg = eval_get_sign(result) < 0;
    if (neg)
      result.negate();
    if (result.compare(lim) <= 0)
      break;
    if (neg)
      result.negate();
    eval_ldexp(result, result, k - 1);
    eval_subtract(D, result);
    ++k;
    eval_ldexp(lim, lim, 1);
  } while (true);

  eval_divide(result, B, D);
}

template <class T, const T &(*F)(void)> struct constant_initializer {
  static void do_nothing() { init.do_nothing(); }

private:
  struct initializer {
    initializer() { F(); }
    void do_nothing() const {}
  };
  static const initializer init;
};

template <class T, const T &(*F)(void)>
typename constant_initializer<T, F>::initializer const
    constant_initializer<T, F>::init;

template <class T> const T &get_constant_ln2() {
  static BOOST_MP_THREAD_LOCAL T result;
  static BOOST_MP_THREAD_LOCAL long digits = 0;
#ifndef BOOST_MP_USING_THREAD_LOCAL
  static BOOST_MP_THREAD_LOCAL bool b = false;
  constant_initializer<T, &get_constant_ln2<T>>::do_nothing();

  if (!b ||
      (digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
    b = true;
#else
  if ((digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
#endif
    calc_log2(
        result,
        boost::multiprecision::detail::digits2<number<T, et_on>>::value());
    digits = boost::multiprecision::detail::digits2<number<T>>::value();
  }

  return result;
}

template <class T> const T &get_constant_e() {
  static BOOST_MP_THREAD_LOCAL T result;
  static BOOST_MP_THREAD_LOCAL long digits = 0;
#ifndef BOOST_MP_USING_THREAD_LOCAL
  static BOOST_MP_THREAD_LOCAL bool b = false;
  constant_initializer<T, &get_constant_e<T>>::do_nothing();

  if (!b ||
      (digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
    b = true;
#else
  if ((digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
#endif
    calc_e(result,
           boost::multiprecision::detail::digits2<number<T, et_on>>::value());
    digits = boost::multiprecision::detail::digits2<number<T>>::value();
  }

  return result;
}

template <class T> const T &get_constant_pi() {
  static BOOST_MP_THREAD_LOCAL T result;
  static BOOST_MP_THREAD_LOCAL long digits = 0;
#ifndef BOOST_MP_USING_THREAD_LOCAL
  static BOOST_MP_THREAD_LOCAL bool b = false;
  constant_initializer<T, &get_constant_pi<T>>::do_nothing();

  if (!b ||
      (digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
    b = true;
#else
  if ((digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
#endif
    calc_pi(result,
            boost::multiprecision::detail::digits2<number<T, et_on>>::value());
    digits = boost::multiprecision::detail::digits2<number<T>>::value();
  }

  return result;
}

template <class T> const T &get_constant_one_over_epsilon() {
  static BOOST_MP_THREAD_LOCAL T result;
  static BOOST_MP_THREAD_LOCAL long digits = 0;
#ifndef BOOST_MP_USING_THREAD_LOCAL
  static BOOST_MP_THREAD_LOCAL bool b = false;
  constant_initializer<T, &get_constant_one_over_epsilon<T>>::do_nothing();

  if (!b ||
      (digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
    b = true;
#else
  if ((digits != boost::multiprecision::detail::digits2<number<T>>::value())) {
#endif
    typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
    result = static_cast<ui_type>(1u);
    eval_divide(result, std::numeric_limits<number<T>>::epsilon().backend());
  }

  return result;
}
