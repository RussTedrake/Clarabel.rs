use crate::algebra::*;

// core.rs
#[no_mangle]
pub extern "C" fn CscMatrix_new(
    m: usize,
    n: usize,
    colptr_ptr: *const usize,
    rowval_ptr: *const usize,
    nzval_ptr: *const f64,
) -> *mut CscMatrix<f64> {
    let colptr = unsafe { std::slice::from_raw_parts(colptr_ptr, n + 1) };
    let len = colptr[n];
    let rowval = unsafe { std::slice::from_raw_parts(rowval_ptr, len) };
    let nzval = unsafe { std::slice::from_raw_parts(nzval_ptr, len) };

    Box::into_raw(Box::new(CscMatrix::new(
        m,
        n,
        colptr.to_vec(),
        rowval.to_vec(),
        nzval.to_vec(),
    )))
}

#[no_mangle]
pub extern "C" fn CscMatrix_delete(matrix: *mut CscMatrix<f64>) {
    if !matrix.is_null() {
        unsafe {
            let boxed_matrix = Box::from_raw(matrix);
            drop(boxed_matrix);
        }
    }
}

#[no_mangle]
pub extern "C" fn CscMatrix_zeros(m: usize, n: usize) -> *mut CscMatrix<f64> {
    Box::into_raw(Box::new(CscMatrix::zeros((m, n))))
}

#[no_mangle]
pub extern "C" fn CscMatrix_identity(n: usize) -> *mut CscMatrix<f64> {
    Box::into_raw(Box::new(CscMatrix::identity(n)))
}

#[no_mangle]
pub extern "C" fn CscMatrix_nnz(matrix_ptr: *const CscMatrix<f64>) -> usize {
    let matrix = unsafe { &*matrix_ptr };
    matrix.nnz()
}
