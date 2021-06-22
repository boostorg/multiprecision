///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2005-2007, 2020.             //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <vector>
#include "q_float_qf.h"

struct gamma_function
{
private:

  static const std::vector<q_float>& ak_inv(void);
  static const std::vector<q_float>& ak_log(void);

public:

  static q_float                 GX(const q_float& x);
  static std::complex<q_float> GZ(const std::complex<q_float>& z);
};

q_float qf::gamma (const q_float& x) { return gamma_function::GX(x); }

std::complex<q_float> qfz::gamma(const std::complex<q_float>& z) { return gamma_function::GZ(z); }

const std::vector<q_float>& gamma_function::ak_inv(void)
{
  // Coefficients of the expansion of 1/gamma.
  // Calculated with Mathematica using:
  // N[Series[Gamma[x], {x, 0, 45}], 50]
  // and subsequently inverting the numerical result with:
  // N[Series[1/Out[1], {x, 0, 45}], 50].
  // This is the numerical extension to 50 decimal digits of precision
  // of table 6.1.34, page 256 of Abromowitz and Stegun.
  static const q_float coef[] =
  {
    qf::one(),
    q_float(" + 0.5772156649015328606065120900824024310421593359399 "),
    q_float(" - 0.6558780715202538810770195151453904812797663804786 "),
    q_float(" - 0.042002635034095235529003934875429818711394500401  "),
    q_float(" + 0.166538611382291489501700795102105235717781502247  "),
    q_float(" - 0.042197734555544336748208301289187391301652684190  "),
    q_float(" - 0.009621971527876973562114921672348198975362942252  "),
    q_float(" + 0.007218943246663099542395010340446572709904800880  "),
    q_float(" - 0.00116516759185906511211397108401838866680933380   "),
    q_float(" - 0.00021524167411495097281572996305364780647824192   "),
    q_float(" + 0.00012805028238811618615319862632816432339489210   "),
    q_float(" - 0.00002013485478078823865568939142102181838229483   "),
    q_float(" - 1.25049348214267065734535947383309224232266E-006    "),
    q_float(" + 1.13302723198169588237412962033074494332400E-006    "),
    q_float(" - 2.0563384169776071034501541300205728365126E-007     "),
    q_float(" + 6.11609510448141581786249868285534286728E-009       "),
    q_float(" + 5.0020076444692229300556650480599913030E-009        "),
    q_float(" - 1.1812745704870201445881265654365055777E-009        "),
    q_float(" + 1.043426711691100510491540332312250191E-010         "),
    q_float(" + 7.7822634399050712540499373113607772E-012           "),
    q_float(" - 3.696805618642205708187815878085766E-012            "),
    q_float(" + 5.10037028745447597901548132286323E-013             "),
    q_float(" - 2.0583260535665067832224295448552E-014              "),
    q_float(" - 5.348122539423017982370017318728E-015               "),
    q_float(" + 1.226778628238260790158893846622E-015               "),
    q_float(" - 1.18125930169745876951376458684E-016                "),
    q_float(" + 1.186692254751600332579777243E-018                  "),
    q_float(" + 1.412380655318031781555803948E-018                  "),
    q_float(" - 2.29874568443537020659247858E-019                   "),
    q_float(" + 1.7144063219273374333839634E-020                    "),
    q_float(" + 1.33735173049369311486478E-022                      "),
    q_float(" - 2.05423355176667278932503E-022                      "),
    q_float(" + 2.736030048607999844832E-023                        "),
    q_float(" - 1.73235644591051663906E-024                         "),
    q_float(" - 2.360619024499287287E-026                           "),
    q_float(" + 1.864982941717294431E-026                           "),
    q_float(" - 2.21809562420719720E-027                            "),
    q_float(" + 1.2977819749479937E-028                             "),
    q_float(" + 1.18069747496653E-030                               "),
    q_float(" - 1.12458434927709E-030                               "),
    q_float(" + 1.2770851751409E-031                                "),
    q_float(" - 7.39145116962E-033                                  "),
    q_float(" + 1.134750258E-035                                    "),
    q_float(" + 4.639134641E-035                                    "),
    q_float(" - 5.34733682E-036                                     ")
  };

  static const std::vector<q_float> Ak(coef, coef + sizeof(coef) / sizeof(coef[0]));

  return Ak;
}

const std::vector<q_float>& gamma_function::ak_log(void)
{
  // Coefficients of Stirling's expansion of log(gamma).
  // Calculated with Mathematica using:
  // N[Series[LogGamma[x], {x, Infinity, 40}], 40].
  // These coefficients consist of Bernoulli B_2n / [2n * (2n - 1)].
  static const q_float coef[] =
  {
    q_float(" + 0.08333333333333333333333333333333333333333    "),
    q_float(" - 0.002777777777777777777777777777777777777778   "),
    q_float(" + 0.0007936507936507936507936507936507936507937  "),
    q_float(" - 0.0005952380952380952380952380952380952380952  "),
    q_float(" + 0.0008417508417508417508417508417508417508418  "),
    q_float(" - 0.001917526917526917526917526917526917526918   "),
    q_float(" + 0.006410256410256410256410256410256410256410   "),
    q_float(" - 0.02955065359477124183006535947712418300654    "),
    q_float(" + 0.1796443723688305731649384900158893966944     "),
    q_float(" - 1.392432216905901116427432216905901116427      "),
    q_float(" + 13.40286404416839199447895100069013112491      "),
    q_float(" - 156.8482846260020173063651324520889738281      "),
    q_float(" + 2193.103333333333333333333333333333333333      "),
    q_float(" - 36108.77125372498935717326521924223073648      "),
    q_float(" + 691472.2688513130671083952507756734675533      "),
    q_float(" - 1.523822153940741619228336495888678051866E+007 "),
    q_float(" + 3.829007513914141414141414141414141414141E+008 "),
    q_float(" - 1.088226603578439108901514916552510537473E+010 "),
    q_float(" + 3.473202837650022522522522522522522522523E+011 ")
  };

  static const std::vector<q_float> Ak(coef, coef + sizeof(coef) / sizeof(coef[0]));

  return Ak;
}

q_float gamma_function::GX(const q_float& x)
{
  const bool b_neg = x.is_neg();

  // Local copy of argument xx.
  q_float xx(!b_neg ? x : -x);

  // Scale the argument down to |xx| < 1, if necessary.
  // Use upward recursion on the result later.
  UINT32 n_recur = 0;
  bool b_pure_int = false;
  
  // For smaller arguments |x| < 20, scale the up argument and use downward
  // recursion later to calculate the final the result.
  const INT32 a = qf::to_int32(xx);
  if(a > 1)
  {
    b_pure_int = q_float(a) == xx;
    
    n_recur = !b_pure_int ? a : a - 1;

    xx -= n_recur;
  }

  UINT32 i;
  q_float G;

  if(b_pure_int)
  {
    G = qf::one();
  }
  else
  {
    G = qf::zero();
    q_float xxn(xx);
  
    for(i = 0; i < ak_inv().size(); i++)
    {
      G += ak_inv()[i] * xxn;
      xxn *= xx;
    }

    static_cast<void>(G.inv());
  }
  
  // Rescale the result using upward recursion if the argument
  // had previously been scaled to |xx| < 1.
  for(i = 0; i < n_recur; i++)
  {
    G  *= xx;
    xx += qf::one();
  }

  // Result.
  return !b_neg ? G : -qf::pi() / (xx * G * qf::sin(qf::pi() * xx));
}

std::complex<q_float> gamma_function::GZ(const std::complex<q_float>& z)
{
  const bool b_neg = z.is_neg();

  // Local copy of argument xx.
  std::complex<q_float> zz(!b_neg ? z : -z);

  UINT32 n_recur = 0;

  // For smaller arguments |x| < 20, scale the up argument and use downward
  // recursion later for the final result.
  const INT32 a = qf::to_int32(qfz::abs(zz));
  static const INT32 scale = 20;
  if(a < scale)
  {
    n_recur = scale - a + 1;

    zz += std::complex<q_float>(n_recur);
  }

  // Stirling's expansion of log(gamma(z)).
  std::complex<q_float> G = (zz - qfz::half()) * qfz::log(zz) - zz + qfz::half() * qfz::log(qfz::two_pi());
  std::complex<q_float> zzn = zz;
  const std::complex<q_float> zz2 = zz * zz;

  UINT32 i;
  for(i = 0; i < ak_log().size(); i++)
  {
    G += std::complex<q_float>(ak_log()[i]) / zzn;
    zzn *= zz2;
  }

  G = qfz::exp(G);

  // Rescale the result using downward recursion if the argument
  // had previously been scaled to |zz| > 20.
  for(i = 0; i < n_recur; i++)
  {
    zz -= qfz::one();
    G  /= zz;
  }

  // Result.
  return !b_neg ? G : -qfz::pi() / (zz * G * qfz::sin(qfz::pi() * zz));
}
