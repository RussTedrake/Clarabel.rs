#![allow(non_snake_case)]
use crate::algebra::*;
use crate::solver::*;

// supportedcone.rs
#[no_mangle]
pub extern "C" fn ZeroCone(n: usize) -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::ZeroConeT(n)))
}

#[no_mangle]
pub extern "C" fn NonnegativeCone(n: usize) -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::NonnegativeConeT(n)))
}

#[no_mangle]
pub extern "C" fn SecondOrderCone(n: usize) -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::SecondOrderConeT(n)))
}

#[no_mangle]
pub extern "C" fn ExponentialCone() -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::ExponentialConeT()))
}

#[no_mangle]
pub extern "C" fn PowerCone(power: f64) -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::PowerConeT(power)))
}

#[no_mangle]
#[cfg(feature = "sdp")]
pub extern "C" fn PSDTriangleCone(n: usize) -> *mut SupportedConeT<f64> {
    Box::into_raw(Box::new(SupportedConeT::PSDTriangleConeT(n)))
}

#[no_mangle]
pub extern "C" fn SupportedCone_delete(cone_ptr: *mut SupportedConeT<f64>) {
    if !cone_ptr.is_null() {
        unsafe {
            let boxed_cone = Box::from_raw(cone_ptr);
            drop(boxed_cone);
        }
    }
}

// implementations/default/settings.rs
#[no_mangle]
pub extern "C" fn DefaultSettings_default() -> *mut DefaultSettings<f64> {
    Box::into_raw(Box::new(DefaultSettings::default()))
}

#[no_mangle]
pub extern "C" fn DefaultSettings_delete(settings_ptr: *mut DefaultSettings<f64>) {
    if !settings_ptr.is_null() {
        unsafe {
            let boxed_cone = Box::from_raw(settings_ptr);
            drop(boxed_cone);
        }
    }
}

// implementations/default/solver.rs
#[no_mangle]
pub extern "C" fn DefaultSolver_new(
    P: *const CscMatrix<f64>,
    q: *const f64,
    A: *const CscMatrix<f64>,
    b: *const f64,
    cone_specs: *const *mut SupportedConeT<f64>,
    cone_specs_len: usize,
    settings: *const DefaultSettings<f64>,
) -> *mut DefaultSolver<f64> {
    let P = unsafe { &*P };
    let q = unsafe { std::slice::from_raw_parts(q, P.ncols()).to_vec() };
    let A = unsafe { &*A };
    let b = unsafe { std::slice::from_raw_parts(b, A.nrows()).to_vec() };

    let cone_specs = unsafe { std::slice::from_raw_parts(cone_specs, cone_specs_len) }
        .iter()
        .map(|&cone| {
            // Here we take back ownership of each `SupportedConeT`
            let cone = unsafe { Box::from_raw(cone) };
            // We destructure the `Box` and copy the `SupportedConeT` (which we assume is `Copy`)
            // so that we can create a `Vec` of `SupportedConeT` instead of `Box<SupportedConeT>`.
            *cone
        })
        .collect::<Vec<_>>();
    let settings = unsafe { &*settings };

    let solver = DefaultSolver::new(P, &q, A, &b, &cone_specs, settings.clone());
    Box::into_raw(Box::new(solver))
}

#[no_mangle]
pub extern "C" fn DefaultSolver_delete(solver_ptr: *mut DefaultSolver<f64>) {
    if !solver_ptr.is_null() {
        unsafe {
            let boxed_cone = Box::from_raw(solver_ptr);
            drop(boxed_cone);
        }
    }
}

#[no_mangle]
pub extern "C" fn DefaultSolver_solve(
    solver_ptr: *mut DefaultSolver<f64>
) {
    let solver = unsafe{ &mut *solver_ptr };
    solver.solve()
}
