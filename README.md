<div align="center">
  <img src="https://www.tensorflow.org/images/tf_logo_transp.png"><br><br>
</div>

-----------------


| **`Documentation`** |
|-----------------|
| [![Documentation](https://img.shields.io/badge/api-reference-blue.svg)](https://www.tensorflow.org/api_docs/) |

**TensorFlow-CL** is a customized version of tensorflow (r1.11) to support OpenCL devices.

- tested on:
  - Ubuntu 16.04, using NVIDIA K520
- should work theoretically on any OpenCL 1.2 GPU

## Execution speed

- [Execution speed](doc/execution_speed.md)

## What's working

- [What's working](doc/whats_working.md)

## Installation

- [Installation](doc/installation.md)

## Tests

- [Tests](doc/testing.md)

## Design/architecture

- tensorflow code stays 100% [NVIDIA® CUDA™](https://www.nvidia.com/object/cuda_home_new.html)
- [Coriander](https://github.com/hughperkins/Coriander) compiles the NVIDIA® CUDA™ code into OpenCL
- Cedric Nugteren's [CLBlast](https://github.com/CNugteren/CLBlast) provides BLAS (matrix multiplications)

