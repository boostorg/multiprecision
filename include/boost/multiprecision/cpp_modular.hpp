//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//---------------------------------------------------------------------------//

#ifndef BOOST_MP_CPP_MODULAR_HPP
#define BOOST_MP_CPP_MODULAR_HPP

#include <boost/multiprecision/cpp_int/montgomery_params.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace boost {
namespace multiprecision {

//mnt params
typedef montgomery_params<cpp_int_backend<> > cpp_mod_params;

// Fixed precision unsigned types:
typedef montgomery_params<cpp_int_backend<128, 128, unsigned_magnitude, unchecked, void> >   umod_params_params128_t;
typedef montgomery_params<cpp_int_backend<256, 256, unsigned_magnitude, unchecked, void> >   umod_params256_t;
typedef montgomery_params<cpp_int_backend<512, 512, unsigned_magnitude, unchecked, void> >   umod_params512_t;
typedef montgomery_params<cpp_int_backend<1024, 1024, unsigned_magnitude, unchecked, void> > umod_params1024_t;

// Fixed precision signed types:
typedef montgomery_params<cpp_int_backend<128, 128, signed_magnitude, unchecked, void> >   mod_params128_t;
typedef montgomery_params<cpp_int_backend<256, 256, signed_magnitude, unchecked, void> >   mod_params256_t;
typedef montgomery_params<cpp_int_backend<512, 512, signed_magnitude, unchecked, void> >   mod_params512_t;
typedef montgomery_params<cpp_int_backend<1024, 1024, signed_magnitude, unchecked, void> > mod_params1024_t;

// Over again, but with checking enabled this time:
typedef montgomery_params<cpp_int_backend<0, 0, signed_magnitude, checked> > checked_cpp_mod_params;

// Fixed precision unsigned types:
typedef montgomery_params<cpp_int_backend<128, 128, unsigned_magnitude, checked, void> >   checked_umod_params128_t;
typedef montgomery_params<cpp_int_backend<256, 256, unsigned_magnitude, checked, void> >   checked_umod_params256_t;
typedef montgomery_params<cpp_int_backend<512, 512, unsigned_magnitude, checked, void> >   checked_umod_params512_t;
typedef montgomery_params<cpp_int_backend<1024, 1024, unsigned_magnitude, checked, void> > checked_umod_params1024_t;

// Fixed precision signed types:
typedef montgomery_params<cpp_int_backend<128, 128, signed_magnitude, checked, void> >   checked_mod_params128_t;
typedef montgomery_params<cpp_int_backend<256, 256, signed_magnitude, checked, void> >   checked_mod_params256_t;
typedef montgomery_params<cpp_int_backend<512, 512, signed_magnitude, checked, void> >   checked_mod_params512_t;
typedef montgomery_params<cpp_int_backend<1024, 1024, signed_magnitude, checked, void> > checked_mod_params1024_t;

//mod_paramsular_adapter
typedef number<modular_adaptor<cpp_int_backend<> > > cpp_mod;

// Fixed precision unsigned types:
typedef number<modular_adaptor<cpp_int_backend<128, 128, unsigned_magnitude, unchecked, void> > >   umod128_t;
typedef number<modular_adaptor<cpp_int_backend<256, 256, unsigned_magnitude, unchecked, void> > >   umod256_t;
typedef number<modular_adaptor<cpp_int_backend<512, 512, unsigned_magnitude, unchecked, void> > >   umod512_t;
typedef number<modular_adaptor<cpp_int_backend<1024, 1024, unsigned_magnitude, unchecked, void> > > umod1024_t;

// Fixed precision signed types:
typedef number<modular_adaptor<cpp_int_backend<128, 128, signed_magnitude, unchecked, void> > >   mod128_t;
typedef number<modular_adaptor<cpp_int_backend<256, 256, signed_magnitude, unchecked, void> > >   mod256_t;
typedef number<modular_adaptor<cpp_int_backend<512, 512, signed_magnitude, unchecked, void> > >   mod512_t;
typedef number<modular_adaptor<cpp_int_backend<1024, 1024, signed_magnitude, unchecked, void> > > mod1024_t;

// Over again, but with checking enabled this time:
typedef number<modular_adaptor<cpp_int_backend<0, 0, signed_magnitude, checked> > > checked_cpp_mod;

// Fixed precision unsigned types:
typedef number<modular_adaptor<cpp_int_backend<128, 128, unsigned_magnitude, checked, void> > >   checked_umod128_t;
typedef number<modular_adaptor<cpp_int_backend<256, 256, unsigned_magnitude, checked, void> > >   checked_umod256_t;
typedef number<modular_adaptor<cpp_int_backend<512, 512, unsigned_magnitude, checked, void> > >   checked_umod512_t;
typedef number<modular_adaptor<cpp_int_backend<1024, 1024, unsigned_magnitude, checked, void> > > checked_umod1024_t;

// Fixed precision signed types:
typedef number<modular_adaptor<cpp_int_backend<128, 128, signed_magnitude, checked, void> > >   checked_mod128_t;
typedef number<modular_adaptor<cpp_int_backend<256, 256, signed_magnitude, checked, void> > >   checked_mod256_t;
typedef number<modular_adaptor<cpp_int_backend<512, 512, signed_magnitude, checked, void> > >   checked_mod512_t;
typedef number<modular_adaptor<cpp_int_backend<1024, 1024, signed_magnitude, checked, void> > > checked_mod1024_t;

}
} // namespace boost::multiprecision

#endif
