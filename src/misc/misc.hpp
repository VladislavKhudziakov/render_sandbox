


#pragma once

#include <cinttypes>


#ifndef NDEBUG
    #define ASSERT(expr) assert(expr)
#else
    #define ASSERT(expr)
#endif

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)
#define CHECK_ERR() auto COMBINE(err, __LINE__) = glGetError(); assert( COMBINE(err, __LINE__) == GL_NO_ERROR)

namespace misc
{
    struct size
    {
        uint32_t width;
        uint32_t height;
    };
}
