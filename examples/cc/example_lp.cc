#include <vector>

#include "clarabel.h"

using std::size_t;

int main(int argc, char* argv[]) {
  CscMatrix* P = CscMatrix_zeros(2, 2);
  std::vector<double> q{1., -1.};

  // a 2-d box constraint, separated into 4 inequalities.
  // A = [I; -I]
  std::vector<size_t> colptr = {0, 2, 4};
  std::vector<size_t> rowval = {0, 2, 1, 3};
  std::vector<double> nzval = {1., -1., 1., -1.};
  CscMatrix* A = CscMatrix_new(4, 2, colptr.data(), rowval.data(),
                               nzval.data());
  std::vector<double> b{1., 1., 1., 1.};

  std::vector<SupportedCone*> cones{NonnegativeCone(4)};

  DefaultSettings* settings = DefaultSettings_default();
  // TODO(russt): the rust example sets some non-default values.

  DefaultSolver* solver = DefaultSolver_new(P, q.data(), A, b.data(),
                                               cones.data(), 1, settings);

  DefaultSolver_solve(solver);

  DefaultSolver_delete(solver);
  DefaultSettings_delete(settings);
  SupportedCone_delete(cones[0]);
  CscMatrix_delete(A);
  CscMatrix_delete(P);
}
