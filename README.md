# Cohort LoopChain Transformer Demo

## Setup and Build

1. Download, build, and/or install the requisite libraries.
  + Rose and everything it needs.
  + Our [LoopChainIR](https://github.com/CompOpt4Apps/LoopChainIR)
  + Google's [OR-Tools](https://developers.google.com/optimization/).
    There are two ways of doing this:
    1. Use the [prebuilt binaries for C++](https://developers.google.com/optimization/introduction/download)
    2. Download the [source](https://github.com/google/or-tools), build and install manually.
    Installing from prebuilt is easiest, but may not be supported.

2. Run (or rerun) `configure` in the Cohort root directory.
3. In the `lctool` directory (or using `the -C option`) run `make`.  
   This will build the translator and all necessary libraries, so it is suggested that the `-j` option is used with an appropriate number of make jobs (such as number of cores).

## Using the translator

The translator is simply run:  
`./loopchaintooldemo/lctool <Input Source File>`  
It is suggested that the translator be run from a directory that **does not** contain source code, since the test generates a number of files, including `.cpp` files, which might confuse the build-process and generally be a pain to remove by hand.
Additionally, having `LoopChainToolDemo/src/` in your `PATH` environment variable, or creating a symlink to the `lctool` executable in a project directory, can make this easier.

The translator can be run in verbose mode using the `-v N` option, where N is an integer in 0..5 (lowest..highest).

The output of the translator is a new `.cpp` file from the input file, prefixing the file-name with `rose_`.
There is also a `.ti` file created by Rose for an unknown purpose (and can be summarily ignored and deleted).

### Input

Inputs are C++ files with the `omplc` annotations

For example:
```cpp
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;

void foo( double* C, double* B, int i ){
  C[i] = (1.0 / (i + 1)) * cos( B[i] );
}

int main(){
  int N = 10;
  double A[N];
  double B[N];
  double C[N];
  double e = exp( 1.0 );

  srand( 12345 );

  for( int i = 0; i <= N-1; i += 1 ){
    A[i] = (rand() % 10000) * 0.001;
    B[i] = e;
    C[i] = 0;
  }

  #pragma omplc loopchain schedule( fuse() )
  {
    #pragma omplc for domain(1:N-2) \
      with (i) \
        read A { (i-1), (i), (i+1) }, \
        write B { (i) }
    for( int i = 1; i <= N-2; i += 1 ){
      B[i] = (A[i-1] + A[i] + A[i+1]) * e;
    }

    #pragma omplc for domain(0:N-1) \
      with (i) \
        read B { (i) }, \
        write C { (i) }
    for( int i = 0; i <= N-1; i += 1 ){
      foo( C, B, i );
    }
  }

  cout.precision(4);
  cout << scientific;

  cout << "A: ";
  for( int i = 0; i <= N-1; i += 1 ){
    cout << ((A[i]<0)?"":" ") << A[i] << " ";
  }

  cout << endl << "B: ";
  for( int i = 0; i <= N-1; i += 1 ){
    cout << ((B[i]<0)?"":" ") << B[i] << " ";
  }

  cout << endl << "C: ";
  for( int i = 0; i <= N-1; i += 1 ){
    cout << ((C[i]<0)?"":" ") << C[i] << " ";
  }
  cout << endl;

}
```

## Help
+ It doesn't build, giving the error like `ld: could not find -lisl -lloopchainIR -lor-tools ...`
+ You need to set `LIBRARY_PATH` to include the directory containing the ISL, LoopChainIR, or OR-Tools library objects.
+ It doesn't build, giving the error that it couldn't find a header file (such as all_isl.h, LoopChain.hpp, SageToTransformationWalker.hpp )
+ You need to set `CPATH` to include the directories containing the ISL, LoopChainIR, and ISL To Sage header files (see the section _Setup and Build_).
+ It builds, but running give an error like `error while loading shared libraries: libisl.so: cannot open shared object file: No such file or directory`
+ You need to set `LD_LIBRARY_PATH` to include the directory containing the ISL shared object (see the section _Setup and Build_).
