#include <iostream>
using namespace std;

__device__ float bar(float a, float b) {
    return a + b;
}

__device__ void incrval(float *a) {
    *a += 3;
}

__global__ void foo(float *data) {
    data[0] = 123.0f;
}

__global__ void use_tid(float *data) {
    int tid = threadIdx.x;
    data[tid] = 123.0f;
}

__global__ void use_tid2(int *data) {
    int tid = threadIdx.x;
    data[tid] = data[tid] + tid;
}

__global__ void copy_float(float *a) {
    a[0] = a[1];
}

__global__ void use_blockid(float *data) {
    int blkid = blockIdx.x;
    data[blkid] = 123.0f;
}

__global__ void use_griddim(float *data) {
    int blkid = gridDim.x;
    data[blkid] = 123.0f;
}

__global__ void use_blockdim(float *data) {
    int blkid = blockDim.x;
    data[blkid] = 123.0f;
}

__host__ float someHostFunction(float input) {
    cout << "You called: someHostFunction()" << endl;
    return input * 100.0f;
}

__global__ void someops_float(float *data) {
    data[0] = data[1] - data[2];
    data[0] += data[1] / data[2];
    data[0] += data[1] * data[2];
    data[0] += log(data[1]);
    data[0] += exp(data[1]);
    data[0] += tanh(data[1]);
    data[0] -= sqrt(data[1]);
}

__global__ void someops_int(int *data) {
    data[0] = data[1] - data[2];
    data[0] += data[1] / data[2];
    data[0] += data[1] + data[2];
    data[0] += data[1] * data[2];
    data[0] += data[1] << data[2];
    data[0] += data[1] >> data[2];
}

__global__ void testbooleanops(int *data) {
    bool a = data[0] > 0;
    bool b = data[1] < 0;
    data[2] = (int)(a && b);
    data[3] = (int)(a || b);
    data[4] = (int)(!a);
}

__global__ void testcomparisons_int_signed(int *data) {
    data[5] = (int)(data[0] >= data[1]);
    data[6] = (int)(data[0] <= data[1]);
    data[7] = (int)(data[0] > data[1]);
    data[8] = (int)(data[0] < data[1]);
    data[9] = (int)(data[0] == data[1]);
    data[10] = (int)(data[0] != data[1]);
}

__global__ void testcomparisons_float(float *data) {
    data[5] = (data[0] >= data[1]);
    data[6] = (data[0] <= data[1]);
    data[7] = (data[0] > data[1]);
    data[8] = (data[0] < data[1]);
    data[9] = (data[0] == data[1]);
    data[10] = (data[0] != data[1]);
}

// // __global__ void testcomparisons_int_unsigned(unsigned int *data) {
// //     data[5] = (unsigned int)(data[0] >= data[1]);
// //     data[6] = (unsigned int)(data[0] <= data[1]);
// //     data[7] = (unsigned int)(data[0] > data[1]);
// //     data[8] = (unsigned int)(data[0] < data[1]);
// //     data[9] = (unsigned int)(data[0] == data[1]);
// //     data[10] = (unsigned int)(data[0] != data[1]);
// // }

__global__ void testsyncthreads(float *data) {
    int tid = threadIdx.x;
    data[tid] *= 2;
    syncthreads();
    data[tid + 1] += 2;
}

void myprintint(int value) {
    cout << "myprintint " << value << endl;
}

void myprintfloat(float value) {
    cout << "myprintfloat " << value << endl;
}

void myprintvoidstar(void *value) {
    cout << "myprintvoid* " << value << endl;
}

void mynop() {

}

__global__ void setValue(float *data, int idx, float value) {
    if(threadIdx.x == 0) {
        data[idx] = value;
    }
}

__host__ void launchSetValue(float *data, int idx, float value) {
    setValue<<<dim3(32, 1, 1), dim3(32, 1, 1)>>>(data, idx, value);
}

struct MyStruct {
    int x;
    float y;
};

__global__ void testStructs(MyStruct *structs, float *float_data, int *int_data) {
    int_data[0] = structs[0].x;
    float_data[0] = structs[0].y;
    float_data[1] = structs[1].y;
}

__global__ void testFloat4(float4 *data) {
    float4 myregister4 = data[1];
    float *myregisterfloat = (float *)&myregister4;
    myregisterfloat[1] = myregisterfloat[2] * myregisterfloat[3];
    data[0] = myregister4;
}

__global__ void testFloat4_test2(float4 *data) {
    data[0] = data[1];
}

// __global__ void testFloat4_test3(float4 *data) {
//     float4 privateFloats[32];
//     for(int i = 0; i < 32; i++) {
//         privateFloats[i] = data[i];
//     }
//     for(int i = 0; i < 32; i++) {
//         data[i + 1] = privateFloats[i];
//     }
// }

__global__ void testLocal(float *data) {
    __shared__ float myshared[32];
    int tid = threadIdx.x;
    myshared[tid] = data[tid];
    data[0] = myshared[tid + 1];
}

__global__ void testLocal2(float *data) {
    __shared__ float myshared[64];
    int tid = threadIdx.x;
    myshared[tid] = data[tid];
    data[0] = myshared[tid + 1];
    myshared[tid + 1] = data[tid];
    data[1] = myshared[tid];
}

__global__ void testArray(float *data) {
    float privateFloats[32];
    for(int i = 0; i < 32; i++) {
        privateFloats[i] = data[i * 3];
    }
    for(int i = 0; i < 32; i+= 2) {
        data[i + 1] = privateFloats[i];
    }
}

__global__ void testmemcpy(float *data) {
    float privateFloats[32];
    for(int i = 0; i < 32; i++) {
        privateFloats[i] = data[i];
    }
    for(int i = 0; i < 32; i+= 2) {
        data[i] = privateFloats[i];
    }
}

__device__ float4 getfloat4(float a) {
    float4 res;
    res.x = a;
    res.y = a + 1;
    res.z = a + 2.5f;
    return res;
}

// attempting to generate an extractvalue instruciton, but failing :-P
__device__ float getfloat4ElementSum(float a, int e0, int e1) {
    float4 res = getfloat4(a);
    float sum = 0;
    sum += ((float *)&res)[e0];
    sum += ((float *)&res)[e1];
    // float4 res;
    // res.x = a;
    // res.y = a + 1;
    // res.z = a + 2.5f;
    // return res;
    return sum;
}

// __global__ void testFloat4_insertvalue(float4 *data, float *data2, int N) {
//     float4 res = getfloat4(data2[0]);
//     data[0] = res;
// }

struct hasArray {
    int foo[4];
};

// __global__ void useHasArray(hasArray *data) {
//     data[0].foo[0] = data[1].foo[2];
// }

__device__ float declaredAfterUse(float val1, float val2);

__global__ void usesForwardDeclaration(float *data) {
    data[0] = declaredAfterUse(data[1], data[2]);
}

__device__ float declaredAfterUse(float val1, float val2) {
    return val1 * val2;
}
