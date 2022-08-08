#![allow(unused)]
#![allow(non_snake_case)]

use super::*;
use clarabel::solver::core::{cones::SupportedCones::*, IPSolver};
use clarabel::solver::implementations::default::*;
use std::slice;

// The types defined here are for exchanging CSC matrix and
// dense vector data between Rust and Julia.

#[repr(C)]
#[derive(Debug)]
pub struct VectorJLRS<T> {
    p: *const T,
    len: libc::size_t,
}

impl<T> VectorJLRS<T>
where
    T: std::clone::Clone + std::fmt::Debug,
{
    fn to_slice(&self) -> &[T] {
        unsafe { slice::from_raw_parts(self.p, self.len as usize) }
    }

    fn to_vec(&self) -> Vec<T> {
        assert!(!self.p.is_null());
        let sl;
        if (self.len > 0) {
            unsafe {
                sl = slice::from_raw_parts(self.p, self.len);
            }
        } else {
            sl = &[];
        }
        sl.to_vec()
    }
}

#[derive(Debug)]
#[repr(C)]
pub struct CscMatrixJLRS {
    m: usize,
    n: usize,
    pub colptr: VectorJLRS<i64>,
    pub rowval: VectorJLRS<i64>,
    pub nzval: VectorJLRS<f64>,
}

impl CscMatrixJLRS {
    fn to_CscMatrix(&self) -> CscMatrix {
        //println!("Getting colptr");
        let colptr = self.colptr.to_slice().iter().map(|&e| e as usize).collect();
        //println!("Getting rowval");
        let rowval = self.rowval.to_slice().iter().map(|&e| e as usize).collect();
        //println!("Getting nzval");
        let nzval = self.nzval.to_vec();

        let mut m = CscMatrix::new(self.m, self.n, colptr, rowval, nzval);

        //need to back shift all of the indices since
        //Julia stores matrices as 1-indexed
        for v in m.rowval.iter_mut() {
            *v -= 1
        }
        for v in m.colptr.iter_mut() {
            *v -= 1
        }

        m //output
    }
}

#[no_mangle]
pub extern "C" fn setup(
    P: &CscMatrixJLRS,
    q: &VectorJLRS<f64>,
    A: &CscMatrixJLRS,
    b: &VectorJLRS<f64>,
) -> f64 {
    let P = P.to_CscMatrix();
    let A = A.to_CscMatrix();
    let q = q.to_slice().to_vec();
    let b = b.to_slice().to_vec();

    let cones = [NonnegativeConeT(b.len())];

    println!("P = {:?}", P);
    println!("A = {:?}", A);
    println!("q = {:?}", q);
    println!("b = {:?}", b);

    let settings = DefaultSettingsBuilder::default()
        .equilibrate_enable(true)
        .max_iter(20)
        .verbose(true)
        .build()
        .unwrap();

    let mut solver = DefaultSolver::new(&P, &q, &A, &b, &cones, settings);
}

#[no_mangle]
pub extern "C" fn solve(
    P: &CscMatrixJLRS,
    q: &VectorJLRS<f64>,
    A: &CscMatrixJLRS,
    b: &VectorJLRS<f64>,
) -> f64 {
    let P = P.to_CscMatrix();
    let A = A.to_CscMatrix();
    let q = q.to_slice().to_vec();
    let b = b.to_slice().to_vec();

    let cones = [NonnegativeConeT(b.len())];

    println!("P = {:?}", P);
    println!("A = {:?}", A);
    println!("q = {:?}", q);
    println!("b = {:?}", b);

    let settings = DefaultSettingsBuilder::default()
        .equilibrate_enable(true)
        .max_iter(20)
        .verbose(true)
        .build()
        .unwrap();

    let mut solver = DefaultSolver::new(&P, &q, &A, &b, &cones, settings);

    solver.solve();
}
