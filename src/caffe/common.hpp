// Copyright 2013 Yangqing Jia

#ifndef CAFFE_COMMON_HPP_
#define CAFFE_COMMON_HPP_

#include <boost/shared_ptr.hpp>
#include <cublas_v2.h>
#include <cuda.h>
#include <curand.h>
// cuda driver types
#include <driver_types.h>
#include <glog/logging.h>
#include <mkl_vsl.h>

#define CUDA_CHECK(condition) CHECK_EQ((condition), cudaSuccess)
#define CUBLAS_CHECK(condition) CHECK_EQ((condition), CUBLAS_STATUS_SUCCESS)
#define CURAND_CHECK(condition) CHECK_EQ((condition), CURAND_STATUS_SUCCESS)
#define VSL_CHECK(condition) CHECK_EQ((condition), VSL_STATUS_OK)

#define CUDA_POST_KERNEL_CHECK \
  if (cudaSuccess != cudaPeekAtLastError()) \
    LOG(FATAL) << "Cuda kernel failed. Error: " \
        << cudaGetErrorString(cudaPeekAtLastError())

#define DISABLE_COPY_AND_ASSIGN(classname) \
private:\
  classname(const classname&);\
  classname& operator=(const classname&)

#define INSTANTIATE_CLASS(classname) \
  template class classname<float>; \
  template class classname<double>

#define NOT_IMPLEMENTED LOG(FATAL) << "Not Implemented Yet"


namespace caffe {

// We will use the boost shared_ptr instead of the new C++11 one mainly
// because cuda does not work (at least now) well with C++11 features.
using boost::shared_ptr;

// For backward compatibility we will just use 512 threads per block
const int CAFFE_CUDA_NUM_THREADS = 512;

inline int CAFFE_GET_BLOCKS(const int N) {
  return (N + CAFFE_CUDA_NUM_THREADS - 1) / CAFFE_CUDA_NUM_THREADS;
}


// A singleton class to hold common caffe stuff, such as the handler that
// caffe is going to use for cublas.
class Caffe {
 private:
  // The private constructor to avoid duplicate instantiation.
  Caffe();

 protected:
  static shared_ptr<Caffe> singleton_;

 public:
  ~Caffe();
  inline static Caffe& Get() {
    if (!singleton_.get()) {
      singleton_.reset(new Caffe());
    }
    return *singleton_;
  }
  enum Brew { CPU, GPU };
  enum Phase { TRAIN, TEST };

  // The getters for the variables.
  // Returns the cublas handle.
  inline static cublasHandle_t cublas_handle() { return Get().cublas_handle_; }
  // Returns the curand generator.
  inline static curandGenerator_t curand_generator() {
    return Get().curand_generator_;
  }
  // Returns the MKL random stream.
  inline static VSLStreamStatePtr vsl_stream() { return Get().vsl_stream_; }
  // Returns the mode: running on CPU or GPU.
  inline static Brew mode() { return Get().mode_; }
  // Returns the phase: TRAIN or TEST.
  inline static Phase phase() { return Get().phase_; }
  // The setters for the variables
  // Sets the mode.
  inline static void set_mode(Brew mode) { Get().mode_ = mode; }
  // Sets the phase.
  inline static void set_phase(Phase phase) { Get().phase_ = phase; }
  // Sets the random seed of both MKL and curand
  static void set_random_seed(const unsigned int seed);

 protected:
  cublasHandle_t cublas_handle_;
  curandGenerator_t curand_generator_;
  VSLStreamStatePtr vsl_stream_;
  Brew mode_;
  Phase phase_;

  DISABLE_COPY_AND_ASSIGN(Caffe);
};

}  // namespace caffe

#endif  // CAFFE_COMMON_HPP_
