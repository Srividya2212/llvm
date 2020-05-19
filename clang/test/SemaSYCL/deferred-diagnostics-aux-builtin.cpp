// RUN: %clang_cc1 -triple spir64-unknown-unknown-sycldevice -fsycl -fsycl-is-device -aux-triple x86_64-unknown-linux-gnu -verify -fsyntax-only  %s
// 
//
// Ensure that the SYCL diagnostics that are typically deferred are correctly emitted.
namespace std {
class type_info;
typedef __typeof__(sizeof(int)) size_t;
} // namespace std

// testing that the deferred diagnostics work in conjunction with the SYCL namespaces.
inline namespace cl {
namespace sycl {

template <typename name, typename Func>
__attribute__((sycl_kernel)) void kernel_single_task(Func kernelFunc) {
  // expected-note@+1 {{called by 'kernel_single_task<AName, (lambda}}
  kernelFunc();
}

} // namespace sycl
} // namespace cl

void calledFromHost(void) {
  //These built-in functions are available for the x86-32 and x86-64 family of computers
  //This function returns a positive integer if the run-time CPU is of type cpuname and returns 0 otherwise.

  //__builtin_cpu_is("intel");
  __builtin_cpu_init ();
}

void calledFromHostWithInvalidBuiltinParam(void) {
  //__builtin_cpu_is("testInvalidCPU");
}

// 


int main(int argc, char **argv) {

  //This is host code. This will not be compiled for the device.
  //calledFromHost();

  //calledFromHostWithInvalidBuiltinParam();
  
  cl::sycl::kernel_single_task<class AName>([]() {
    //SYCL device compiler will compile this kernel for a device as well as any functions that the kernel calls
    __builtin_cpu_init (); // expected-error {{ AUX target specific builtins should not be present in device code }}
  });
  return 0;
}
