# llvm-verify-pass
# Development Environment
## Platform Support and Requirements
This project is currently incomplete. Currently, this project has been tested
on **Ubuntu 20.04** and **Arch Linux 2021.01.01**. In order to build this project you will need:
  * LLVM 11 or higher
  * Z3prover
  * C++ compiler with C++ 14 support
  * CMake 3.13.4 or higher

In order to run the pass you will need:
  * **clang** (to generate input LLVM files)
  * **opt** (to load and execute the pass)

## Install LLVM on Ubuntu
On Ubuntu, you can install LLVM using apt-get command.
```bash
sudo apt-get install llvm-11-dev
sudo apt-get install llvm-11
sudo apt-get install llvm
sudo apt-get install clang-11
sudo apt-get install opt
```

If it doesn't work then you can get it from official [repository](https://apt.llvm.org/)

```bash
# Install the latest stable version
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# Install LLVM 11
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 11
```

## Building Z3 from source
The following steps will work on Linux:

```bash
    * download the source code of 4.8.7 to some directory, say <z3dir> from https://github.com/Z3Prover/z3/releases/tag/z3-4.8.7
    * cd <z3dir>
    * Create a directory called build inside <z3dir>
    * Change to build directory
    * run 
        * cmake ../
        * make -j4 (j4 is for forking 4 threads to speed up make build. It will take around 15 minutes to build)
        * sudo make install

```
For further details read the [official documentation](https://github.com/Z3Prover/z3/blob/master/README-CMake.md).

## Installing Boost library

For Ubuntu 20.04 install version 1.71 of Boost library using the following commands.
```bash
sudo apt-get install libboost1.71-all-dev
sudo apt-get install libboost-program-options-dev
```

On Ubuntu 18.04 the available version is 1.65 which can be installed using
```bash
sudo apt-get install libboost1.65-all-dev
sudo apt-get install libboost-program-options-dev
```

If you installed Boost version 1.65 then you need to change the following line in CMakeLists.txt file. This file is present in the root directory of the project.
 ```bash
find_package(Boost 1.71 COMPONENTS program_options REQUIRED)
```
 to
  ```bash
find_package(Boost 1.65 COMPONENTS program_options REQUIRED)
```

 

# Building the project
## Building
You can build **llvm-verify-pass** as follows:
```bash
# Clone the repository to a directory, say <projectdir>
git clone https://github.com/chinuhub/ProofTrapar_LLVM.git 
# Change to the project directory
cd <projectdir>
# Currently you are inside the cloned repository.
# Create a build directory inside it.
mkdir build
# change to build directory
cd build
# run cmake
cmake ..
# It will creata a makefile inside build directory. Now run make command.
make -j4 # (j4 is for forking 4 threads to perform make build)
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

**Note**: You may have to use ```opt-11``` instead of ```opt``` and similarly,
```clang-11``` instead of ```clang```.

### For New PM interface
```bash
# Generate an LLVM IR file for New PM interface
clang -emit-llvm -S input_file.c
# It will create a file input_file.ll in the current directory
# Run the pass through opt - New PM
opt -disable-output -load-pass-plugin <build_dir>/lib/libSafetyVerificationPass.so -passes="verify-module-safety" input_file.ll
```


### For Legacy PM interface

```bash
# Generate an LLVM bitcode file for Legacy PM interface
clang -emit-llvm -o input_file.c
# It will create a file input_file.bc in the current directory
# Run the pass through opt - Legacy PM
opt -disable-output -load <build_dir>/lib/libSafetyVerificationPass.so -legacy-verify-module-safety input_file.bc
```

# LLVM Pass Managers

## An Overview of LLVM Pass Managers
There are two pass managers for llvm:
* **New Pass Manager**:
  * LLVM is slowly transitioning to New Pass Manager and this will eventually
    become the default pass manager in LLVM.
  * There is no official documentation for New Pass Manager right now. However,
    a short tutorial is given [here](https://llvm.org/docs/WritingAnLLVMNewPMPass.html).
* **Legacy Pass Manager** which currently is the default pass manager in LLVM:
  * It is implemented in the _legacy_ namespace.
  * It is documented very well. You can read about writing and registering
    pass [here](https://llvm.org/docs/WritingAnLLVMPass.html).



# Graphviz
## Installing Graphviz
### On Ubuntu:
```bash
sudo apt install graphviz
```
### On Mac OS X:
```bash
brew install graphviz
```
### On Arch Linux:
```bash
sudo pacman -S graphviz
```
## Generating .png files from dot files
It is recommended that you use neato instead of dot to obtain more human readable graphs. Use the following command to generate png
image file from a dot file:
```bash
neato -Tpng <input_dot_file> -o <output_png_file>
```
