#ifndef ZFP_UTILS_H
#define ZFP_UTILS_H

#include "zfparray1.h"
#include "zfparray2.h"
#include "zfparray3.h"

namespace zfp {

zfp::array* construct_from_stream(const uchar* buffer, size_t bufferSize = 0)
{
  try {
    return new array1f(buffer, bufferSize);
  } catch (std::exception const & e) {}

  try {
    return new array1d(buffer, bufferSize);
  } catch (std::exception const & e) {}

  try {
    return new array2f(buffer, bufferSize);
  } catch (std::exception const & e) {}

  try {
    return new array2d(buffer, bufferSize);
  } catch (std::exception const & e) {}

  try {
    return new array3f(buffer, bufferSize);
  } catch (std::exception const & e) {}

  try {
    return new array3d(buffer, bufferSize);
  } catch (std::exception const & e) {}

  return NULL;
}

}

#endif
