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

/*! @file matrix.cc
 *  @brief Unit tests for oclalgo::Matrix class.
 *  @author Dmitry Senin <d.senin@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  Tests for hblas::matrix class
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include "inc/oclalgo/matrix.h"
#include "src/gtest_main.cc"

TEST(Matrix, Copy) {
  using oclalgo::Matrix;
  Matrix<int> m1(128, 512);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j;

  Matrix<int> m2(m1);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      ASSERT_EQ(m1(i, j), m2(i, j));
}

TEST(Matrix, MoveCopy) {
  using oclalgo::Matrix;
  int rows = 512, cols = 256;
  Matrix<int> m1(rows, cols);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j;

  Matrix<int> m2(std::move(m1));
  EXPECT_EQ(0, m1.rows());
  EXPECT_EQ(0, m1.cols());
  for (int i = 0; i < m2.rows(); ++i)
    for (int j = 0; j < m2.cols(); ++j)
      ASSERT_EQ(i * cols + j, m2(i, j));
}

TEST(Matrix, Assignment) {
  using oclalgo::Matrix;
  Matrix<int> m1(512, 256);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j;

  Matrix<int> m2;
  m2 = m1;
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      ASSERT_EQ(m1(i, j), m2(i, j));
}

TEST(Matrix, MoveAssignment) {
  using oclalgo::Matrix;
  int rows = 512, cols = 256;
  Matrix<int> m1(rows, cols);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j;

  Matrix<int> m2;
  m2 = std::move(m1);
  EXPECT_EQ(0, m1.rows());
  EXPECT_EQ(0, m1.cols());
  for (int i = 0; i < m2.rows(); ++i)
    for (int j = 0; j < m2.cols(); ++j)
      ASSERT_EQ(i * cols + j, m2(i, j));
}

TEST(Matrix, Add) {
  using oclalgo::Matrix;
  Matrix<int> m1(512, 256), m2(512, 256);
  for (int i = 0; i < m1.rows(); ++i) {
    for (int j = 0; j < m1.cols(); ++j) {
      m1(i, j) = i * m1.cols() + j;
      m2(i, j) = m2.rows() * m2.cols() - i * m2.cols() - j;
    }
  }

  Matrix<int> res = m1 + m2;
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      ASSERT_EQ(m2.cols() * m2.rows(), res(i, j));
}

TEST(Matrix, Sub) {
  using oclalgo::Matrix;
  Matrix<int> m1(512, 128), m2(512, 128);
  for (int i = 0; i < m1.rows(); ++i) {
    for (int j = 0; j < m1.cols(); ++j) {
      m1(i, j) = i * m1.cols() + j + m1.cols() * m1.rows();
      m2(i, j) = i * m2.cols() + j;
    }
  }

  Matrix<int> res = m1 - m2;
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      ASSERT_EQ(m1.cols() * m1.rows(), res(i, j));
}

TEST(Matrix, MulIdentity) {
  using oclalgo::Matrix;
  Matrix<int> m1(200, 200), I(200, 200);
  for (int i = 0; i < m1.rows(); ++i) {
    for (int j = 0; j < m1.cols(); ++j) {
      m1(i, j) = i * m1.cols() + j;
      I(i, j) = i == j ? 1 : 0;
    }
  }

  Matrix<int> I2 = I * I;
  EXPECT_EQ(I.rows(), I2.rows());
  EXPECT_EQ(I.cols(), I2.cols());
  for (int i = 0; i < I2.rows(); ++i)
    for (int j = 0; j < I2.cols(); ++j)
      ASSERT_EQ(I(i, j), I2(i, j));

  Matrix<int> m1_I = m1 * I;
  for (int i = 0; i < m1_I.rows(); ++i)
    for (int j = 0; j < m1_I.cols(); ++j)
      ASSERT_EQ(m1(i, j), m1_I(i, j));
}

TEST(Matrix, MulInt) {
  using oclalgo::Matrix;
  Matrix<int> m1(4, 4), m2(4, 8);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j + 1;
  for (int i = 0; i < m2.rows(); ++i)
    for (int j = 0; j < m2.cols(); ++j)
      m2(i, j) = i * m2.cols() + j + 1;

  Matrix<int> res = m1 * m2;
  int gold_res[] = { 170, 180, 190, 200,  210,  220,  230,  240,
                     378, 404, 430, 456,  482,  508,  534,  560,
                     586, 628, 670, 712,  754,  796,  838,  880,
                     794, 852, 910, 968, 1026, 1084, 1142, 1200 };
  for (int i = 0; i < res.rows(); ++i)
    for (int j = 0; j < res.cols(); ++j)
      ASSERT_EQ(gold_res[i * res.cols() + j], res(i, j));
}

TEST(Matrix, MulFloat) {
  using oclalgo::Matrix;
  Matrix<float> m1(2, 2), m2(2, 2);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j + 1.5F;
  for (int i = 0; i < m2.rows(); ++i)
    for (int j = 0; j < m2.cols(); ++j)
      m2(i, j) = i * m2.cols() + j + 1.5F + m1.cols() * m2.rows();

  Matrix<float> res = m1 * m2;
  float gold_res[] = { 27.F, 31.F,
                       53.F, 61.F };
  for (int i = 0; i < res.rows(); ++i)
    for (int j = 0; j < res.cols(); ++j)
      ASSERT_EQ(gold_res[i * res.cols() + j], res(i, j));
}

TEST(Matrix, Transpose) {
  using oclalgo::Matrix;
  Matrix<int> m1(512, 256), m2(256, 512);
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      m1(i, j) = i * m1.cols() + j;
  for (int i = 0; i < m2.rows(); ++i)
    for (int j = 0; j < m2.cols(); ++j)
      m2(i, j) = j * m2.rows() + i;

  m1.transpose();
  for (int i = 0; i < m1.rows(); ++i)
    for (int j = 0; j < m1.cols(); ++j)
      ASSERT_EQ(m2(i, j), m1(i, j));
}
