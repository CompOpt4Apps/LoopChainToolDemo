# Cohort LoopChain Transformer Demo

## How to Try
There are two ways to try this demo:
1. Build from source and run the executable.
   This can be difficult because of the dependencies.
2. Run from docker.
   This is easier an can be completed with:
   ```
   ./make_image.sh
   cat path/to/source/file.cpp | sudo docker run -i compopt4apps/loopchaintooldemo > path/to/output/file.cpp
   ```
   See the `build_assist/docker_container` Readme for details.

## Setup and Build
1. Download, build, and/or install the requisite libraries.
  + Rose and everything it needs.
  + Our [LoopChainIR](https://github.com/CompOpt4Apps/LoopChainIR)
  + Google's [OR-Tools](https://developers.google.com/optimization/).
    There are two ways of doing this:
    1. Use the [prebuilt binaries for C++](https://developers.google.com/optimization/introduction/download)
    2. Download the [source](https://github.com/google/or-tools), build and install manually.
    Installing from prebuilt is easiest, but may not be supported.

  If this is too difficult or you are running into problems (or would like to avoid running into problems) the local_build_generator tool (see `build_assist/local_build_generator`) can help.
  Alternatively, you can use the docker container (see `build_assist/docker_container`).

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

## Loop Chain Annotations

This tool works by reading OpenMP style pragma annotations.
The code that is annotated needs to be be valid C/C++ code, that runs correctly
regardless of whether or not the tool has been used.

Below is described what annotations are needed and how they can be used.
Throughout, we will use this example code:
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

  // First nest of the loop chain
  for( int i = 1; i <= N-2; i += 1 ){
    B[i] = (A[i-1] + A[i] + A[i+1]) * e;
  }

  // Second nest of the loop chain
  for( int i = 0; i <= N-1; i += 1 ){
    foo( C, B, i );
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

#### Enclosing The Loop Chain

First a loop chain must be annotated in the `loopchain` annotation.
This is done by putting the nests of the loop chain in its own scope, and
annotating this enclosing scope:

```cpp
#pragma omplc loopchain schedule( )
{
  for( int i = 1; i <= N-2; i += 1 ){
    B[i] = (A[i-1] + A[i] + A[i+1]) * e;
  }

  for( int i = 0; i <= N-1; i += 1 ){
    foo( C, B, i );
  }
}
```

Notice that the `loopchain` annotation has an `schedule` component.
This is where the schedules to be applied to the loop chain will be listed,
and this will be touched on in the subsection "Scheduling Directives".

#### Describing Nests and Access Patterns

Next we have to describe the domain (also known as the iteration space) of each
nest, and their access patterns.
This is done with the `for` annotation.
The domain is described by the `domain(...)` clause.
In the domain clause, a range for a single dimension written as `lower_bound_expression : upper_bound_expression`.
This range is inclusive on both ends, the bounds expressions may only use
numeric constants and symbolic constants which appear as variables in the code.
Multiple ranges in a comma separated list are used to create a multi-dimenisonal
domain, with the outer dimension at the beginning, and inner dimension at the
end.
As an example, the nest:
```cpp
for( i = 0; i <= M; i += 1 )
  for( j = lb + 1; j < N + 3; j +=1 )
    for( k = 1 * 3 + Z; k <= 4 )
      // body
```
would be the domain expression: `domain( 0:M, lb+1:N+2, 1*3+Z:4 )`

In our running example, the first domain is `domain( 1:N-2 )`, and the second
domain is `domain( 0:N-1 )`.

To describe the access patterns we first declare the iterator symbols that
represent a single iteration of the nest, as a comma separated tuple, where each
individual iterator iterates over the range in that dimension of the domain.
This will be used in our access patterns.
Then we say what dataspace we are accessing, the direction (read/write), and the
indexes being accessed.
A dataspace can be any named construct that is indexable in some way.
The basic example is an array.

In our example, the first loop reads the array `A` at the indexes `i-1`, `i`,
and `i+1`, and writes the array `B` at the index `i`.
The second loop reads the array `B` at the index `i` and writes array `C` at the
index `i`.
This is known because we read the source of the function `foo`.
```cpp
#pragma omplc loopchain schedule( )
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
```

Note, in this example the pragma 'statements' are split over several lines to make reading
easier.
This requires a backslash (`\`) at the end of the line to tell the compiler to
treat the lines joined with backslashes as one line.

At this point, the annotations are syntactically valid and the loop chain tool
will accept the input code.

#### Scheduling Directives
Having described the loop chain, the next step is to apply some schedules to it.

Below are the directives that tell the tool what schedules should be applied.
The directives are listed in the `schedule( ... )`

##### Fuse
Syntax: `fuse( [optional shift terms: ( <shift>, <...> ), <...> ] )`

The optional shift terms is a comma separated list of tuples ordered by the
order of the nests they shift.
With the fuse directive, all of the loops in the loop chain are fused using a
loop fusion transformation to the depth indicated by their domains.
The results is a single loop nest in the loop chain.
How much shifting each loop requires will be determined based on the data
dependences induced by the data accesses and constrained by the loop domains
unless  optional shifting information
The original order of the loops within the loop chain is the order of statements
in the fused loop body.

##### Tile
Syntax: `tile( ( <tile size>, <...> ), <outer schedule>, <inner schedule> )`

The tile directive indicates that all the loop nests in the loop chain should be
tiled.
Specifically the D outermost loops for each loop nest
should be rectangularly tiled using a tile of size `(s1, ..., sD)`, where `sd`
indicates the tile size in dimension `d` of the loop nest.
The `<outer schedule>` and `<inner schedule>` are the schedules over the tiles
and within the tiles.
Currently, only constant-sized tiles are supported.
Providing the schedule command `tile( (16), ... )` will result in tiling the
outer loop of a loop nest with a tile size of 16.
Specifying the schedule command `tile( (16,16,16), ... )` will create cubic
tiles with a volume of 4096 (16 cubed).
The dimensionality of the tiling specified cannot exceed that of the domain
provided in the for loop pragmas in the loop nest, but it can be smaller.

##### Wavefront
Syntax: `waverfront`

This schedule command when at the outermost level of the schedule annotation
indicates that all the loop nests in the loop chain should use a wavefront
parallelization strategy.
A wavefront strategy turns a loop nest into one with an outer serial loop (no
change) and then D − 1 inner parallel loops.
The D − 1 inner parallel loops will be skewed enough to make the parallelism
legal.
The wavefront command can also be used as the schedule over and/or within tiles.

##### Serial and Parallel
Syntax: `serial` and `parallel`

The serial directive is used inside the tiling directive to indicate that the
schedule over or within (or both) should not be parallelized.
Similarly, the parallel directive is used inside the tiling directive to
indicate that the schedule over or within (or both) *should* be parallelized.
Can also be used stand-alone to indicates that the outer loop of all loop nests
in the loop chain should be parallelized.


### Example Input
Below is the completely annotated version of the code with the a fused and tiled
schedule.
Additional examples can be found under `examples`.

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

  #pragma omplc loopchain schedule( fuse(), tile( (16), parallel, serial) )
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
