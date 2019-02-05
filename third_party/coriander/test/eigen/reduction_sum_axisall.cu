// This is from Eigen unsupported/test/cxx11_tensor_cuda.cu

#define EIGEN_TEST_NO_LONGDOUBLE
#define EIGEN_TEST_NO_COMPLEX
#define EIGEN_TEST_FUNC cuda_reduction
#define EIGEN_USE_GPU

#include <unsupported/Eigen/CXX11/Tensor>

#include "main.h"

#include <iostream>

using Eigen::Tensor;

void test_cuda_reduction()
{
  Tensor<float, 4> in1(72,53,97,113);
  Tensor<float, 0> out;
  in1.setRandom();

  std::size_t in1_bytes = in1.size() * sizeof(float);
  std::size_t out_bytes = out.size() * sizeof(float);

  float* d_in1;
  float* d_out;
  cudaMalloc((void**)(&d_in1), in1_bytes);
  cudaMalloc((void**)(&d_out), out_bytes);

  cudaMemcpy(d_in1, in1.data(), in1_bytes, cudaMemcpyHostToDevice);

  Eigen::CudaStreamDevice stream;
  Eigen::GpuDevice gpu_device(&stream);

  Eigen::TensorMap<Eigen::Tensor<float, 4> > gpu_in1(d_in1, 72,53,97,113);
  Eigen::TensorMap<Eigen::Tensor<float, 2> > gpu_out(d_out, 72,97);

  gpu_out.device(gpu_device) = gpu_in1.sum();
  // float out = gpu_in1.sum();

  assert(cudaMemcpyAsync(out.data(), d_out, out_bytes, cudaMemcpyDeviceToHost, gpu_device.stream()) == cudaSuccess);
  assert(cudaStreamSynchronize(gpu_device.stream()) == cudaSuccess);
  std::cout << "actual sum, from gpu: " << out << std::endl;

  float sum = 0;
  for (int i = 0; i < 72; ++i) {
    for (int j = 0; j < 97; ++j) {
      for (int k = 0; k < 53; ++k) {
        for (int l = 0; l < 113; ++l) {
          sum += in1(i, k, j, l);
        }
      }
    }
  }
  std::cout << "expected sum, from cpu: " << sum << std::endl;
  // VERIFY_IS_APPROX(out, sum);

  cudaFree(d_in1);
  cudaFree(d_out);
}
