#pragma once

#include <memory>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/Dense>

namespace clarabel {

class SupportedCone {
  public:
    virtual ~SupportedCone();

    // Internal use only.
    const void* get_ptr() const;
    
  protected:
    SupportedCone(void* cone_ptr);

  private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

class ZeroCone : public SupportedCone {
  public:
    ZeroCone(size_t dim);
    size_t dim() const { return dim_; }
  private:
    size_t dim_;
};

class NonnegativeCone : public SupportedCone {
  public:
    NonnegativeCone(size_t dim);
    size_t dim() const { return dim_; }
  private:
    size_t dim_;
};

class SecondOrderCone : public SupportedCone {
  public:
    SecondOrderCone(size_t dim);
    size_t dim() const { return dim_; }
  private:
    size_t dim_;
};

class ExponentialCone : public SupportedCone {
  public:
    ExponentialCone(size_t dim);
    size_t dim() const { return dim_; }
  private:
    size_t dim_;
};

class PowerCone : public SupportedCone {
  public:
    PowerCone(double power);
    double power() const { return power_; }
  private:
    double power_;
};

/*
class PSDTriangleCone : public SupportedCone {
  public:
    PSDTriangleCone(size_t dim);
    size_t dim() const { return dim_; }
  private:
    size_t dim_;
};
*/

class DefaultSolver {
  public:
    DefaultSolver(const Eigen::SparseMatrix<double> &P,
                  const Eigen::Ref<Eigen::VectorXd> &q,
                  const Eigen::SparseMatrix<double> &A,
                  const Eigen::Ref<Eigen::VectorXd> &b,
                  const std::vector<SupportedCone> &cone_specs);

    ~DefaultSolver();

    void solve();

  private:
    // Provides PIMPL encapsulation of FFI types.
    class Impl;
    std::unique_ptr<Impl> impl_{nullptr};
};

} // namespace clarabel