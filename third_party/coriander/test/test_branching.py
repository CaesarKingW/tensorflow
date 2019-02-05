# Copyright Hugh Perkins 2016
"""
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""
import numpy as np
import pyopencl as cl
from test import test_common
from test.test_common import compile_code
from test.test_common import offset_type
import pytest
import os


sourcecode = """
__global__ void testDoWhile(int *data, int N) {
    int p = threadIdx.x;
    do {
        p++;
    } while(data[p] != 0);
}

__global__ void testWhile(int *data, int N) {
    int p = threadIdx.x;
    while(data[p] != 0) {
        p++;
    }
}

__global__ void testIfElse(int *data, int N) {
    int tid = threadIdx.x;
    if(tid < N) {
        data[tid] *= 2;
        data[tid + 3] *= 2;
    } else {
        data[tid] -= 20;
        data[tid + 5] -= 20;
    }
}

"""


def test_test_if(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testIf(float *data, int N) {
    int tid = threadIdx.x;
    if(tid < N) {
        data[tid] *= 2;
    }
}
"""
    kernelName = test_common.mangle('testIf', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 2
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)
    for i in range(10):
        if i < N:
            assert float_data[i] == float_data_orig[i] * 2
        else:
            assert float_data[i] == float_data_orig[i]


def test_test_if_else(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testIfElse(float *data, int N) {
    int tid = threadIdx.x;
    if(tid < N) {
        data[tid] *= 2;
    } else {
        data[tid] += 5;
    }
}
"""
    kernelName = test_common.mangle('testIfElse', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 2
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)
    for i in range(10):
        if i < N:
            assert float_data[i] == float_data_orig[i] * 2
        else:
            assert abs(float_data[i] - float_data_orig[i] - 5) <= 1e-4


def test_test_do_while(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testIfElse(float *data, int N) {
    int tid = threadIdx.x;
    int i = 0;
    float sum = 0;
    do {
        sum += data[i];
        i++;
    } while(sum < 15);
    data[0] = sum;
}
"""
    kernelName = test_common.mangle('testIfElse', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 2
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)
    print(float_data[0])


def test_test_inlines(context, q, float_data, float_data_gpu):
    sourcecode = """
__device__ void somefunc(float *data) {
    data[0] = 3.4;
}

__global__ void testInline(float *data, int N) {
    somefunc(data);
}
"""
    kernelName = test_common.mangle('testInline', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 4
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)


def test_test_for(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testFor(float *data, int N) {
    if(threadIdx.x == 0) {
        float sum = 0.0f;
        for(int i = 0; i < N; i++) {
            sum += data[i];
        }
        data[0] = sum;
    }
}
"""
    kernelName = test_common.mangle('testFor', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 4
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)

    sum = 0
    for i in range(N):
        sum += float_data_orig[i]
    assert abs(float_data[0] - sum) <= 1e-4


def test_test_two_fors(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testTwoFors(float *data, int N) {
    if(threadIdx.x == 0) {
        float sum = 0.0f;
        for(int i = 0; i < N; i++) {
            sum += data[i];
        }
        int end = N * 3;
        float sum2 = 0;
        for(int i = 8; i < end; i++) {
            sum2 += data[i + 7];
        }
        data[0] = sum;
        data[1] = sum2;
    }
}
"""
    kernelName = test_common.mangle('testTwoFors', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 4
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)

    sum = 0
    for i in range(N):
        sum += float_data_orig[i]
    assert abs(float_data[0] - sum) <= 1e-4


@pytest.mark.skipif(os.environ.get('TRAVIS', None) == 'true', reason='this test fails on travis mac cpu for some reason')
def test_nested_for(context, q, float_data, float_data_gpu):
    source = """
__device__ void myfunc(float *data, int a, int b) {
    data[1] = data[2];
    float sum = 0;
    for(int i = 0; i < a; i++) {
        for(int j = 0; j < b; j++) {
            sum += data[i * 17 + j * 7];
        }
    }
    data[0] = sum;
}

__global__ void mykernel(float *data, int a, int b) {
    myfunc(data, a, b);
}
"""
    kernelName = test_common.mangle('mykernel', ['float *', 'int', 'int'])
    kernel = test_common.compile_code_v3(cl, context, source, kernelName, num_clmems=1)['kernel']
    float_data_orig = np.copy(float_data)

    a = 2
    b = 3
    kernel(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(a), np.int32(b), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)

    sum = 0
    for i in range(a):
        for j in range(b):
            sum += float_data_orig[i * 17 + j * 7]

    print('float_data_orig', float_data_orig[:3])
    print('float_data', float_data[:3])

    assert float_data[1] == float_data_orig[2]
    assert abs(float_data[0] - sum) <= 1e-4
    # assert abs(float_data[0] - sum(float_data_orig[0:32])) < 1e-4


def test_simpleloop(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void longKernel(float *data, int N, float value) {
    for(int i = 0; i < N; i++) {
        data[i] += value;
    }
}
"""
    kernelName = test_common.mangle('longKernel', ['float *', 'int', 'float'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 2
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), np.float32(123), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)
    print(float_data[0])


def test_break(context, q, float_data, float_data_gpu):
    sourcecode = """
__global__ void testFor(float *data, int N) {
    if(threadIdx.x == 0) {
        float sum = 0.0f;
        for(int i = 1; i < N; i++) {
            sum += data[i];
            if(sum > 5) {
                break;
            }
        }
        data[0] = sum;
    }
}
"""
    kernelName = test_common.mangle('testFor', ['float *', 'int'])
    prog = compile_code(cl, context, sourcecode, kernelName, num_clmems=1)
    float_data_orig = np.copy(float_data)

    N = 4
    prog.__getattr__(kernelName)(q, (32,), (32,), float_data_gpu, offset_type(0), offset_type(0), np.int32(N), cl.LocalMemory(4))
    cl.enqueue_copy(q, float_data, float_data_gpu)
    q.finish()
    with open('/tmp/testprog-device.cl', 'r') as f:
        cl_code = f.read()
    print('cl_code', cl_code)

    sum = 0
    for i in range(1, N):
        sum += float_data_orig[i]
        print(i, float_data_orig[i], sum)
        if sum > 5:
            break
    print('sum', sum, 'float_data[0]', float_data[0])
    assert abs(float_data[0] - sum) <= 1e-4


# def test_conditional_branch(context, q, float_data, float_data_gpu):
#     """
#     Just use normal if...else, but turn off branching transformations, and check
#     opencl works ok
#     """
#     sourcecode = """
# __global__ void testIfElse(float *data, int N) {
#     int tid = threadIdx.x;
#     if(tid < N) {
#         data[tid] *= 2;
#     } else {
#         data[tid] += 5;
#     }
# }
# """
#     prog = compile_code(cl, context, sourcecode, branching_transformations=False)
#     float_data_orig = np.copy(float_data)

#     N = 2
#     prog.__getattr__(test_common.mangle('testIfElse', ['float *', 'int']))(q, (32,), (32,), float_data_gpu, offset_type(0), np.int32(N), cl.LocalMemory(4))
#     cl.enqueue_copy(q, float_data, float_data_gpu)
#     q.finish()
#     with open('/tmp/testprog-device.cl', 'r') as f:
#         cl_code = f.read()
#     print('cl_code', cl_code)
#     for i in range(10):
#         if i < N:
#             assert float_data[i] == float_data_orig[i] * 2
#         else:
#             assert abs(float_data[i] - float_data_orig[i] - 5) <= 1e-4
