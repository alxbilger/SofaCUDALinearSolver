/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <SofaCudaSolver/config.h>

#include <sofa/core/behavior/LinearSolver.h>
#include <sofa/component/linearsolver/iterative/MatrixLinearSolver.h>
#include <sofa/simulation/MechanicalVisitor.h>

#include <sofa/helper/OptionsGroup.h>

#include <cusolverSp.h>
#include <cusparse.h>
#include <cuda_runtime.h>
#include <cuda.h> 

namespace sofa::component::linearsolver::direct
{

// Direct linear solver based on Sparse Cholesky factorization, implemented with the cuSOLVER library
template<class TMatrix, class TVector>
class SolverGPU : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector>
{
public:
  SOFA_CLASS(SOFA_TEMPLATE2(SolverGPU,TMatrix,TVector),SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector));

  Data<bool> f_verbose; ///< Dump system state at each iteration
  Data<sofa::helper::OptionsGroup> d_typePermutation;
  Data<sofa::helper::OptionsGroup> d_typeSolver;

  typedef TMatrix Matrix;
  typedef TVector Vector;
  typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector> Inherit;

  int rows;///< numbuer of rows
  int cols;///< number of columns
  int nnz;///< number of non zero elements

  int previous_nnz;
  int previous_n;

  bool firstStep;

  int singularity;
  double tol;

    // csr format
  int* host_RowPtr; 
  int* host_ColsInd; 
  double* host_values;

  int* device_RowPtr;
  int* device_ColsInd;
  double* device_values;

  int reorder;
  int solverType;

  cusolverSpHandle_t handle;
  cudaStream_t stream;
  cusparseHandle_t cusparseHandle;
  cusparseMatDescr_t descr; 

  double* device_x;
  double* device_b;

  void* buffer_cpu;
   
  SolverGPU();
  ~SolverGPU();
  void solve (Matrix& M, Vector& x, Vector& b) override;
  void invert(Matrix& M) override;
    
};

#if  !defined(SOFA_PLUGIN_SOLVERGPU_CPP)
extern template class SOFACUDASOLVER_API SolverGPU< sofa::linearalgebra::CompressedRowSparseMatrix<SReal>, sofa::linearalgebra::FullVector<SReal> > ;
#endif

} // namespace sofa::component::linearsolver::direct
