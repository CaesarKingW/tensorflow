
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// This file implements the Xgemm routine. The precision is implemented using a template argument.
//
// =================================================================================================

#ifndef CLBLAST_ROUTINES_XGEMM_H_
#define CLBLAST_ROUTINES_XGEMM_H_

#include "routine.hpp"

namespace clblast {
// =================================================================================================

// See comment at top of file for a description of the class
template <typename T>
class Xgemm: public Routine {
 public:

  // Constructor
  Xgemm(Queue &queue, EventPointer event, const std::string &name = "GEMM");

  // Templated-precision implementation of the routine
  StatusCode DoGemm(const Layout layout, const Transpose a_transpose, const Transpose b_transpose,
                    const size_t m, const size_t n, const size_t k,
                    const T alpha,
                    const Buffer<T> &a_buffer, const size_t a_offset, const size_t a_ld,
                    const Buffer<T> &b_buffer, const size_t b_offset, const size_t b_ld,
                    const T beta,
                    const Buffer<T> &c_buffer, const size_t c_offset, const size_t c_ld);
};

// =================================================================================================
} // namespace clblast

// CLBLAST_ROUTINES_XGEMM_H_
#endif
