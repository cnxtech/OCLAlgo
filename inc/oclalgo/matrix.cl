/*!
 * Copyright (c) 2014, Samsung Electronics Co.,Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of Samsung Electronics Co.,Ltd..
 *
 * OCLAlgo - Framework based on C++11 and OpenCL API to provide simple access
 *           to OpenCL devices for asynchronous calculations.
 * URL:      https://github.com/seninds/OCLAlgo
 */

// hack for highlighting syntax in OpenCL *.cl files without errors
#ifndef __OPENCL_VERSION__
#define __kernel
#define __global
#define __local
#endif  // __OPENCL_VERSION__

#ifndef VAR_TYPE
#define VAR_TYPE int
#endif  // VAR_TYPE

__kernel void matrix_add(__global const VAR_TYPE *A, __global const VAR_TYPE *B,
                         __global VAR_TYPE *C) {
  int i = get_global_id(0);
  int j = get_global_id(1);
  int cols = get_global_size(1);
  int idx = i * cols + j;
  C[idx] = A[idx] + B[idx];
}

__kernel void matrix_sub(__global const VAR_TYPE *A, __global const VAR_TYPE *B,
                         __global VAR_TYPE *C) {
  int i = get_global_id(0);
  int j = get_global_id(1);
  int cols = get_global_size(1);
  int idx = i * cols + j;
  C[idx] = A[idx] - B[idx];
}

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 32
#endif  // BLOCK_SIZE

typedef enum { ROW, COL } PackingType;

typedef struct tag_matrix_param_t {
  int rows;
  int cols;
  PackingType packing;
} matrix_param_t;

inline VAR_TYPE get_element(__global const VAR_TYPE* m,
                           __global const matrix_param_t* param, int i, int j) {
  return param->packing == ROW ? m[i * param->cols + j] :
                                 m[j * param->rows + i];
}

__kernel __attribute__((reqd_work_group_size(BLOCK_SIZE, BLOCK_SIZE, 1)))
void matrix_mul(__global const VAR_TYPE *A,
                __global const matrix_param_t *A_param,
                __global const VAR_TYPE *B,
                __global const matrix_param_t *B_param,
                __global VAR_TYPE *C) {
  int gx = get_group_id(0);
  int lx = get_local_id(0);
  int gy = get_group_id(1);
  int ly = get_local_id(1);

  __local VAR_TYPE AS[BLOCK_SIZE][BLOCK_SIZE];
  __local VAR_TYPE BS[BLOCK_SIZE][BLOCK_SIZE];

  int i_A, j_A, i_B, j_B;
  VAR_TYPE sum = 0;
  for (int j = 0, i = 0; j < A_param->cols; j += BLOCK_SIZE, i += BLOCK_SIZE) {
    j_A = j + lx;
    i_A = BLOCK_SIZE * gy + ly;
    j_B = BLOCK_SIZE * gx + lx;
    i_B = i + ly;

    // if current positions in shared matrices AS and BS are
    // out of range then set 0
    AS[ly][lx] = (j_A < A_param->cols && i_A < A_param->rows) ?
        get_element(A, A_param, i_A, j_A) : 0;
    BS[ly][lx] = (j_B < B_param->cols && i_B < B_param->rows) ?
        get_element(B, B_param, i_B, j_B) : 0;

    barrier(CLK_LOCAL_MEM_FENCE);

    #pragma unroll
    for (int k = 0; k < BLOCK_SIZE; ++k) {
      sum += AS[ly][k] * BS[k][lx];
    }

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  if (get_global_id(1) < A_param->rows && get_global_id(0) < B_param->cols) {
    C[get_global_id(1) * B_param->cols + get_global_id(0)] = sum;
  }
}

#undef BLOCK_SIZE
#undef VAR_TYPE
