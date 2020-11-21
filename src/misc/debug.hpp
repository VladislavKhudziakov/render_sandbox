


#pragma once

#include <cassert>

namespace misc
{
#ifndef NDEBUG
    #define ASSERT(expr) assert(expr)
#else
    #define ASSERT(expr)
#endif
}
