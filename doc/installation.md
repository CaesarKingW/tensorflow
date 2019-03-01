# Installation 

- You will need:
  - the tensorflow (r1.11) non-gpu installation pre-requisites
  - an OpenCL 1.2-enabled GPU, and OpenCL 1.2-enabled drivers (check that `clinfo` shows your GPU, and that is shows as a GPU device)
  - python 3
  - bazel 0.17.1
  - 

For other operating systems, please [build from source](build-from-source.md)

By default, Tensorflow-cl will run using the first GPU available on your system. You can use the environment variable `CL_GPUOFFSET` to choose others:

- `export CL_GPUOFFSET=1` chooses the second GPU (ie, index 1)
- `export CL_GPUOFFSET=2` chooses the third GPU
