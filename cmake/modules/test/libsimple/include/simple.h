#ifndef GUARD_SIMPLE_H
#define GUARD_SIMPLE_H

#ifndef __MINGW32__

#include <thread>

#endif

#if !defined(HAS_SIMPLE) || DEFINE_3 != 3
#error "Not configured"
#endif

inline void simple() {
#ifndef __MINGW32__
    std::thread([] {
    }).join();
#endif
}


#endif
