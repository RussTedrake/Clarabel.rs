#include "solver.h"

#include <memory>

#include "ffi/algebra.h"
#include "ffi/solver.h"

namespace clarabel {

using Eigen::SparseMatrix;
using Eigen::VectorXd;

using ffi::CscMatrix;
using CscMatrixPtr =
    std::unique_ptr<ffi::CscMatrix, decltype(&ffi::CscMatrix_delete)>;
using SupportedConePtr =
    std::unique_ptr<ffi::SupportedCone, decltype(&ffi::SupportedCone_delete)>;
using DefaultSettingsPtr =
    std::unique_ptr<ffi::DefaultSettings,
                    decltype(&ffi::DefaultSettings_delete)>;
using DefaultSolverPtr = std::unique_ptr<ffi::DefaultSolver,
                                         decltype(&ffi::DefaultSolver_delete)>;
namespace {

CscMatrixPtr EigenToCscMatrix(const SparseMatrix<double> &matrix) {
  size_t m = matrix.rows();
  size_t n = matrix.cols();
  size_t nnz = matrix.nonZeros();
  std::vector<size_t> colptr(n + 1);
  std::vector<size_t> rowval(nnz);
  std::vector<double> nzval(nnz);
  size_t k = 0;
  for (size_t j = 0; j < n; j++) {
    colptr[j] = k;
    for (SparseMatrix<double>::InnerIterator it(matrix, j); it; ++it) {
      rowval[k] = it.row();
      nzval[k] = it.value();
      k++;
    }
  }
  colptr[n] = k;
  return CscMatrixPtr(ffi::CscMatrix_new(m, n, colptr.data(), rowval.data(), nzval.data()), &ffi::CscMatrix_delete);
}

} // namespace

class SupportedCone::Impl {
public:
  Impl(ffi::SupportedCone *cone_ptr)
      : cone_ptr_(cone_ptr, &ffi::SupportedCone_delete) {}

  const void *get_ptr() const { return cone_ptr_.get(); }

private:
  SupportedConePtr cone_ptr_;
};

SupportedCone::SupportedCone(ffi::SupportedCone *cone_ptr)
    : impl_(std::make_shared<SupportedCone::Impl>(cone_ptr)) {}

SupportedCone::~SupportedCone() = default;

const void *SupportedCone::get_ptr() const { return impl_->get_ptr(); }

ZeroCone::ZeroCone(size_t dim) : SupportedCone(ffi::ZeroCone(dim)), dim_(dim) {}

NonnegativeCone::NonnegativeCone(size_t dim)
    : SupportedCone(ffi::NonnegativeCone(dim)), dim_(dim) {}

SecondOrderCone::SecondOrderCone(size_t dim)
    : SupportedCone(ffi::SecondOrderCone(dim)), dim_(dim) {}

ExponentialCone::ExponentialCone(size_t dim)
    : SupportedCone(ffi::ExponentialCone(dim)), dim_(dim) {}

PowerCone::PowerCone(double power)
    : SupportedCone(ffi::PowerCone(power)), power_(power) {}

/*
PSDTriangleCone::PSDTriangleCone(size_t dim)
    : SupportedCone(ffi::PSDTriangleCone(dim)), dim_(dim) {}
*/

class DefaultSolver::Impl {
public:
  Impl(const SparseMatrix<double> &P, const Eigen::Ref<VectorXd> &q,
       const SparseMatrix<double> &A, const Eigen::Ref<VectorXd> &b,
       const std::vector<SupportedCone> &cone_specs) {
    if (P.rows() != P.cols()) {
      throw std::invalid_argument("P must be square");
    }
    if (P.rows() != q.size()) {
      throw std::invalid_argument("P and q must have the same number of rows");
    }
    if (A.cols() != P.cols()) {
      throw std::invalid_argument(
          "A and P must have the same number of columns");
    }
    if (A.rows() != b.size()) {
      throw std::invalid_argument("A and b must have the same number of rows");
    }
    P_csc_ = EigenToCscMatrix(P);
    A_csc_ = EigenToCscMatrix(A);
    settings_ = DefaultSettingsPtr(ffi::DefaultSettings_default(),
                                   &ffi::DefaultSettings_delete);
    std::vector<const ffi::SupportedCone *> cones(cone_specs.size());
    for (size_t i = 0; i < cone_specs.size(); i++) {
      cones[i] = cone_specs[i].get_ptr();
    }
    solver_ptr_ =
        DefaultSolverPtr(ffi::DefaultSolver_new(
                             P_csc_.get(), q.data(), A_csc_.get(), b.data(),
                             cones.data(), cone_specs.size(), settings_.get()),
                         &ffi::DefaultSolver_delete);
  }

  ~Impl() = default;

  void solve() { ffi::DefaultSolver_solve(solver_ptr_.get()); }

private:
  CscMatrixPtr P_csc_{nullptr, &ffi::CscMatrix_delete};
  CscMatrixPtr A_csc_{nullptr, &ffi::CscMatrix_delete};
  DefaultSolverPtr solver_ptr_{nullptr, &ffi::DefaultSolver_delete};
  DefaultSettingsPtr settings_{nullptr, &ffi::DefaultSettings_delete};
};

DefaultSolver::DefaultSolver(const SparseMatrix<double> &P,
                             const Eigen::Ref<VectorXd> &q,
                             const SparseMatrix<double> &A,
                             const Eigen::Ref<VectorXd> &b,
                             const std::vector<SupportedCone> &cone_specs) {
  impl_ = std::make_unique<Impl>(P, q, A, b, cone_specs);
}

DefaultSolver::~DefaultSolver() = default;

void DefaultSolver::solve() { impl_->solve(); }

} // namespace clarabel