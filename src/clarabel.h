#pragma once

#include "algebra.h"
#include "solver.h"

class DefaultSolver {
 public:
  DefaultSolver(Eigen::SparseMatrix<double> P, Eigen::VectorXd q,
                Eigen::SparseMatrix<double> A, Eigen::VectorXd b,
                std::vector<SupportedCone*> cones, DefaultSettings* settings);
    
  ~DefaultSolver();

 private:
  DefaultSolver* solver_ptr_{nullptr};
};