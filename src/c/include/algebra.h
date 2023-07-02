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
  typedef void CscMatrix;   // Treat CscMatrix as an opaque type.

  // colptr must be of length `n` + 1.
  // rowval and nzval must be of length `colptr[n]`.
  CscMatrix *CscMatrix_new(size_t m, size_t n, size_t *colptr, size_t *rowval,
                           double *nzval);
  void CscMatrix_delete(CscMatrix* matrix_ptr);
  CscMatrix* CscMatrix_zeros(size_t m, size_t n);
  CscMatrix* CscMatrix_identity(size_t n);
  int CscMatrix_nnz(CscMatrix* matrix_ptr);

#ifdef __cplusplus
}
#endif
