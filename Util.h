#pragma once

#include <algorithm>

namespace util
{

using glPtr = void*;

//==============================================================================
template <typename T>
std::size_t stride(std::size_t number)
{
  return number * sizeof(T);
}

//==============================================================================
template <typename T>
glPtr toGlPtr(std::size_t pos)
{
  return (void *)(pos * sizeof(T));
}

}
