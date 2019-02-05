
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// This file implements the Xamax class (see the header for information about the class).
//
// =================================================================================================

#include "routines/level1/xamax.hpp"

#include <string>
#include <vector>

namespace clblast {
// =================================================================================================

// Constructor: forwards to base class constructor
template <typename T>
Xamax<T>::Xamax(Queue &queue, EventPointer event, const std::string &name):
    Routine(queue, event, name, {"Xdot"}, PrecisionValue<T>()) {
  source_string_ =
    #include "../../kernels/level1/xamax.opencl"
  ;
}

// =================================================================================================

// The main routine
template <typename T>
StatusCode Xamax<T>::DoAmax(const size_t n,
                            const Buffer<unsigned int> &imax_buffer, const size_t imax_offset,
                            const Buffer<T> &x_buffer, const size_t x_offset, const size_t x_inc) {

  // Makes sure all dimensions are larger than zero
  if (n == 0) { return StatusCode::kInvalidDimension; }

  // Tests the vectors for validity
  auto status = TestVectorX(n, x_buffer, x_offset, x_inc);
  if (ErrorIn(status)) { return status; }
  status = TestVectorIndex(1, imax_buffer, imax_offset);
  if (ErrorIn(status)) { return status; }

  // Retrieves the Xamax kernels from the compiled binary
  try {
    const auto program = GetProgramFromCache(context_, PrecisionValue<T>(), routine_name_);
    auto kernel1 = Kernel(program, "Xamax");
    auto kernel2 = Kernel(program, "XamaxEpilogue");

    // Creates the buffer for intermediate values
    auto temp_size = 2*db_["WGS2"];
    auto temp_buffer1 = Buffer<T>(context_, temp_size);
    auto temp_buffer2 = Buffer<unsigned int>(context_, temp_size);

    // Sets the kernel arguments
    kernel1.SetArgument(0, static_cast<int>(n));
    kernel1.SetArgument(1, x_buffer());
    kernel1.SetArgument(2, static_cast<int>(x_offset));
    kernel1.SetArgument(3, static_cast<int>(x_inc));
    kernel1.SetArgument(4, temp_buffer1());
    kernel1.SetArgument(5, temp_buffer2());

    // Event waiting list
    auto eventWaitList = std::vector<Event>();

    // Launches the main kernel
    auto global1 = std::vector<size_t>{db_["WGS1"]*temp_size};
    auto local1 = std::vector<size_t>{db_["WGS1"]};
    auto kernelEvent = Event();
    status = RunKernel(kernel1, queue_, device_, global1, local1, kernelEvent.pointer());
    if (ErrorIn(status)) { return status; }
    eventWaitList.push_back(kernelEvent);

    // Sets the arguments for the epilogue kernel
    kernel2.SetArgument(0, temp_buffer1());
    kernel2.SetArgument(1, temp_buffer2());
    kernel2.SetArgument(2, imax_buffer());
    kernel2.SetArgument(3, static_cast<int>(imax_offset));

    // Launches the epilogue kernel
    auto global2 = std::vector<size_t>{db_["WGS2"]};
    auto local2 = std::vector<size_t>{db_["WGS2"]};
    status = RunKernel(kernel2, queue_, device_, global2, local2, event_, eventWaitList);
    if (ErrorIn(status)) { return status; }

    // Succesfully finished the computation
    return StatusCode::kSuccess;
  } catch (...) { return StatusCode::kInvalidKernel; }
}

// =================================================================================================

// Compiles the templated class
template class Xamax<half>;
template class Xamax<float>;
template class Xamax<double>;
template class Xamax<float2>;
template class Xamax<double2>;

// =================================================================================================
} // namespace clblast
