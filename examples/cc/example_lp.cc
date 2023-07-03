#include "src/cc/clarabel.h"

int main(int argc, char* argv[]) {
  Eigen::SparseMatrix<double> P(2, 2);
  Eigen::Vector2d q{1., -1.};

  // a 2-d box constraint, separated into 4 inequalities.
  // A = [I; -I]
  Eigen::SparseMatrix<double> A(4, 2);
  A.insert(0, 0) = 1.;
  A.insert(1, 1) = 1.;
  A.insert(2, 0) = -1.;
  A.insert(3, 1) = -1.;
  Eigen::Vector4d b{1., 1., 1., 1.};

  std::vector<clarabel::SupportedCone> cones{clarabel::NonnegativeCone(4)};

  // TODO(russt): the rust example uses some non-default settings.

  clarabel::DefaultSolver solver(P, q, A, b, cones);
  solver.solve();
}
