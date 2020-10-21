


#pragma once

#include <cinttypes>


#ifndef NDEBUG
    #define ASSERT(expr) assert(expr)
#else
    #define ASSERT(expr)
#endif

#define COMBINE1(X, Y) X##Y
#define COMBINE(X, Y) COMBINE1(X, Y)
#define CHECK_ERR()  auto COMBINE(err, __LINE__) = glGetError(); if (COMBINE(err, __LINE__) != GL_NO_ERROR) throw std::runtime_error("invalid gl call. error code: " + std::to_string(COMBINE(err, __LINE__)))

namespace misc
{
    struct size
    {
        uint32_t width;
        uint32_t height;
    };
} // namespace misc
