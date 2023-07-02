#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstddef>
}
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
  typedef void SupportedCone;   // Treat SupportedCone as an opaque type.

  SupportedCone* ZeroCone(size_t n);
  SupportedCone* NonnegativeCone(size_t n);
  SupportedCone* SecondOrderCone(size_t n);
  SupportedCone* ExponentialCone(size_t n);
  SupportedCone* PowerCone(double power);
  SupportedCone* PSDTriangleCone(size_t n);
  void SupportedCone_delete(SupportedCone* cone_ptr);

  typedef void DefaultSettings; // Treat DefaultSettings as an opaque type.
  DefaultSettings* DefaultSettings_default();
  void DefaultSettings_delete(DefaultSettings* settings_ptr);

  typedef void DefaultSolver;   // Treat DefaultSolver as an opaque type.
  DefaultSolver* DefaultSolver_new(CscMatrix* P, double* q, CscMatrix* A,
                                   double* b, SupportedCone** cone_specs, size_t num_cones,
                                   DefaultSettings* settings);
  void DefaultSolver_delete(DefaultSolver* solver_ptr);
  void DefaultSolver_solve(DefaultSolver* solver_ptr);

#ifdef __cplusplus
}
#endif
