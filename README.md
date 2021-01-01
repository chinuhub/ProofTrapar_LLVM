# llvm-verify-pass
# Development Environment
## Platform Support and Requirements
This project is currently incomplete. Currently, this project has been tested
on **Ubuntu 20.04**. In order to build this project you will need:
  * LLVM 11 or higher
  * Z3prover
  * C++ compiler with C++ 14 support
  * CMake 3.13.4 or higher

In order to run the pass you will need:
  * **clang** (to generate input LLVM files)
  * **opt** (to load and execute the pass)

## Building LLVM from source
Building from sources can be slow and tricky but following steps will work on
Linux:

```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build
cd build
cmake <llvm-project/root/dir>/llvm/
make
make install
```
For further details read the [official documentation](https://llvm.org/docs/CMake.html).
## Building Z3 from source
The following steps will work on Linux:

```bash
git clone https://github.com/Z3Prover/z3
cd z3
mkdir build
cd build
cmake -G "Unix Makefiles" ../
make
make install
```
For further details read the [official documentation](https://github.com/Z3Prover/z3/blob/master/README-CMake.md).

# Building the project
## Building
You can build **llvm-verify-pass** as follows:
```bash
mkdir <build/dir>
cd <build/dir>
cmake ..
make
```

# Running and testing
## Test Input Format
Test inputs are c programs.
Input programs must not use reference or pointers. Only primitive types are
allowed. It is, however, recommended that you only use integer type for
variables in your program. All programs must begin with following code:

```
void __ASSERT() {}

#define assert(e) if (!(e)) __ASSERT()
```

This should be followed by global variable definitions. It is recommended that
you initialize your variables when you declare them. Consider the following:

```
int flag1 = 0, flag2 = 0;
int turn;
int x;
```

Function definitions should follow afterwards. Every function with a declaration
must also be defined as well. All functions must return void and must not take
any arguements. Only declare the functions that you want to verify as part of
you code.

Use ```assert(<cond>)``` to insert assertions at some point in your thread.
An example function definition follows:

```
void thr1() {
  flag1 = 1;
  turn = 1;
  while (flag2==1 && turn==1) {};
  // begin: critical section
  x = 0;
  assert(x<=0);
  // end: critical section
  flag1 = 0;
}
```

As a whole, the input .c file for peterson algorithm looks like:
```
void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

#define assert(e) if (!(e)) __ASSERT()

int flag1 = 0, flag2 = 0;
int turn = 0;
int x = 0;

void thr1() {
  flag1 = 1;
  turn = 1;
  while (flag2==1 && turn==1) {};
  // begin: critical section
  x = 0;
  assert(x<=0);
  // end: critical section
  flag1 = 0;
}

void thr2() {
  flag2 = 1;
  turn = 0;
  while (flag1==1 && turn==0) {};
  // begin: critical section
  x = 1;
  assert(x>=1);
  // end: critical section
  flag2 = 0;
}
```

## Run the SafetyVerificationPass pass
There are two ways to run the pass. First, you need to decide which pass manager
you want to use (see [here](#llvm-pass-managers) for more details).

### Generating .bc and .ll from .c files]

```bash
# Generate an LLVM IR file for New PM interface
clang -emit-llvm -S input_file.c > output_file.ll
# Generate an LLVM bitcode file Legacy PM interface
clang -emit-llvm -o input_file.c > output_file.bc
```

### Running the pass through opt
```bash
# Run the pass through opt - Legacy PM
opt -disable-output -load <build_dir>/lib/libSafetyVerificationPass.so -legacy-verify-module-safety input_for_legacy_pm.bc
# Run the pass through opt - New PM
opt -disable-output -load-pass-plugin <build_dir>/lib/libSafetyVerificationPass.so -passes="verify-module-safety" input_for_new_pm.ll
```


# LLVM Pass Managers

## An Overview of LLVM Pass Managers
There are two pass managers for llvm:
* **Legacy Pass Manager** which currently is the default pass manager in LLVM:
  * It is implemented in the _legacy_ namespace.
  * It is documented very well. You can read about writing and registering
    pass [here](https://llvm.org/docs/WritingAnLLVMPass.html).
* **New Pass Manager**:
  * LLVM is slowly transitioning to New Pass Manager and this will eventually
    become the default pass manager in LLVM.
  * There is no official documentation for New Pass Manager right now. However,
    a short tutorial is given [here](https://llvm.org/docs/WritingAnLLVMNewPMPass.html).

## Runnig New PM vs Legacy PM using opt

Input for Legacy PM is llvm bitcode files i.e. .bc extension. Input for New PM
is llvm IR i.e. .ll extension.

Our **SafetyVerificationPass** implements interface for both pass managers. This
is how you can run it with Legacy PM:
```bash
opt -disable-output -load <build_dir>/lib/libSafetyVerificationPass.so -legacy-verify-module-safety input_for_legacy_pm.bc
```

And this is how you run it with New PM:
```bash
opt -disable-output -load-pass-plugin <build_dir>/lib/libSafetyVerificationPass.so -passes="verify-module-safety" input_for_new_pm.ll
```
