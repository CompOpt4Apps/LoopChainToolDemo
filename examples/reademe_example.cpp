#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;

#define intDiv(x,y) ((((x)%(y))>=0) ? ((x)/(y)) : (((x)/(y))) -1 )
#define intMod(x,y) ((((x)%(y))>=0) ? ((x)%(y)) : (((x)%(y)) +y))

#define ceild(n, d) intDiv((n), (d)) + ((intMod((n),(d))>0)?1:0)
#define floord(n, d) intDiv((n), (d))

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
