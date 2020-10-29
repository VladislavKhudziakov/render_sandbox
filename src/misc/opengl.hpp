


#pragma once

#include <glad/glad.h>

#define COMBINE1(X, Y) X##Y
#define COMBINE(X, Y) COMBINE1(X, Y)
#define CHECK_ERR()  auto COMBINE(err, __LINE__) = glGetError(); if (COMBINE(err, __LINE__) != GL_NO_ERROR) throw std::runtime_error("invalid gl call. error code: " + std::to_string(COMBINE(err, __LINE__)))
