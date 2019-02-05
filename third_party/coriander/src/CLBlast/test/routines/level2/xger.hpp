
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// This file implements a class with static methods to describe the Xger routine. Examples of
// such 'descriptions' are how to calculate the size a of buffer or how to run the routine. These
// static methods are used by the correctness tester and the performance tester.
//
// =================================================================================================

#ifndef CLBLAST_TEST_ROUTINES_XGER_H_
#define CLBLAST_TEST_ROUTINES_XGER_H_

#include <vector>
#include <string>

#ifdef CLBLAST_REF_CLBLAS
  #include "test/wrapper_clblas.hpp"
#endif
#ifdef CLBLAST_REF_CBLAS
  #include "test/wrapper_cblas.hpp"
#endif

namespace clblast {
// =================================================================================================

// See comment at top of file for a description of the class
template <typename T>
class TestXger {
 public:

  // The BLAS level: 1, 2, or 3
  static size_t BLASLevel() { return 2; }

  // The list of arguments relevant for this routine
  static std::vector<std::string> GetOptions() {
    return {kArgM, kArgN,
            kArgLayout,
            kArgALeadDim, kArgXInc, kArgYInc,
            kArgAOffset, kArgXOffset, kArgYOffset,
            kArgAlpha};
  }

  // Describes how to obtain the sizes of the buffers
  static size_t GetSizeX(const Arguments<T> &args) {
    return args.m * args.x_inc + args.x_offset;
  }
  static size_t GetSizeY(const Arguments<T> &args) {
    return args.n * args.y_inc + args.y_offset;
  }
  static size_t GetSizeA(const Arguments<T> &args) {
    auto a_rotated = (args.layout == Layout::kRowMajor);
    auto a_two = (a_rotated) ? args.m : args.n;
    return a_two * args.a_ld + args.a_offset;
  }

  // Describes how to set the sizes of all the buffers
  static void SetSizes(Arguments<T> &args) {
    args.a_size = GetSizeA(args);
    args.x_size = GetSizeX(args);
    args.y_size = GetSizeY(args);
  }

  // Describes what the default values of the leading dimensions of the matrices are
  static size_t DefaultLDA(const Arguments<T> &args) { return args.n; }
  static size_t DefaultLDB(const Arguments<T> &) { return 1; } // N/A for this routine
  static size_t DefaultLDC(const Arguments<T> &) { return 1; } // N/A for this routine

  // Describes which transpose options are relevant for this routine
  using Transposes = std::vector<Transpose>;
  static Transposes GetATransposes(const Transposes &) { return {}; } // N/A for this routine
  static Transposes GetBTransposes(const Transposes &) { return {}; } // N/A for this routine

  // Describes how to run the CLBlast routine
  static StatusCode RunRoutine(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
    auto queue_plain = queue();
    auto event = cl_event{};
    auto status = Ger(args.layout,
                      args.m, args.n, args.alpha,
                      buffers.x_vec(), args.x_offset, args.x_inc,
                      buffers.y_vec(), args.y_offset, args.y_inc,
                      buffers.a_mat(), args.a_offset, args.a_ld,
                      &queue_plain, &event);
    clWaitForEvents(1, &event);
    return status;
  }

  // Describes how to run the clBLAS routine (for correctness/performance comparison)
  #ifdef CLBLAST_REF_CLBLAS
    static StatusCode RunReference1(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
      auto queue_plain = queue();
      auto event = cl_event{};
      auto status = clblasXger(convertToCLBLAS(args.layout),
                               args.m, args.n, args.alpha,
                               buffers.x_vec, args.x_offset, args.x_inc,
                               buffers.y_vec, args.y_offset, args.y_inc,
                               buffers.a_mat, args.a_offset, args.a_ld,
                               1, &queue_plain, 0, nullptr, &event);
      clWaitForEvents(1, &event);
      return static_cast<StatusCode>(status);
    }
  #endif

  // Describes how to run the CPU BLAS routine (for correctness/performance comparison)
  #ifdef CLBLAST_REF_CBLAS
    static StatusCode RunReference2(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
      std::vector<T> a_mat_cpu(args.a_size, static_cast<T>(0));
      std::vector<T> x_vec_cpu(args.x_size, static_cast<T>(0));
      std::vector<T> y_vec_cpu(args.y_size, static_cast<T>(0));
      buffers.a_mat.Read(queue, args.a_size, a_mat_cpu);
      buffers.x_vec.Read(queue, args.x_size, x_vec_cpu);
      buffers.y_vec.Read(queue, args.y_size, y_vec_cpu);
      cblasXger(convertToCBLAS(args.layout),
                args.m, args.n, args.alpha,
                x_vec_cpu, args.x_offset, args.x_inc,
                y_vec_cpu, args.y_offset, args.y_inc,
                a_mat_cpu, args.a_offset, args.a_ld);
      buffers.a_mat.Write(queue, args.a_size, a_mat_cpu);
      return StatusCode::kSuccess;
    }
  #endif

  // Describes how to download the results of the computation (more importantly: which buffer)
  static std::vector<T> DownloadResult(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
    std::vector<T> result(args.a_size, static_cast<T>(0));
    buffers.a_mat.Read(queue, args.a_size, result);
    return result;
  }

  // Describes how to compute the indices of the result buffer
  static size_t ResultID1(const Arguments<T> &args) { return args.m; }
  static size_t ResultID2(const Arguments<T> &args) { return args.n; }
  static size_t GetResultIndex(const Arguments<T> &args, const size_t id1, const size_t id2) {
    return (args.layout == Layout::kRowMajor) ?
           id1*args.a_ld + id2 + args.a_offset:
           id2*args.a_ld + id1 + args.a_offset;
  }

  // Describes how to compute performance metrics
  static size_t GetFlops(const Arguments<T> &args) {
    return 2 * args.m * args.n;
  }
  static size_t GetBytes(const Arguments<T> &args) {
    return (2*args.m*args.n + args.m + args.n) * sizeof(T);
  }
};

// =================================================================================================
} // namespace clblast

// CLBLAST_TEST_ROUTINES_XGER_H_
#endif
