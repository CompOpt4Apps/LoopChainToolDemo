#include <iostream>
#include <cstdlib>
#include <getopt.h>

using namespace std;

#define intDiv(x,y) ((((x)%(y))>=0) ? ((x)/(y)) : (((x)/(y))) -1 )
#define intMod(x,y) ((((x)%(y))>=0) ? ((x)%(y)) : (((x)%(y)) +y))

#define ceild(n, d) intDiv((n), (d)) + ((intMod((n),(d))>0)?1:0)
#define floord(n, d) intDiv((n), (d))

double** alloc_array( int N ){
  double** array = (double**) calloc( sizeof(double), N );
  for( int i = 0; i < N; ++i ){
    array[i] = (double*) calloc( sizeof(double), N );
  }

  return array;
}

double** test( int N, int T, double start ){

  double** flip = alloc_array( N + 2 );
  double** flop = alloc_array( N + 2 );

  flip[N/2+1][N/2 + 1] = start;

  for( int t = 0; t < T/2; ++t ){

    #pragma omplc loopchain schedule( /* schedule directives go here */ )
    {
      /* Denotes loop nest of the chain */
      #pragma omplc for \
        /* indicates the domain of the nest */ \
        domain(1:N,1:N) \
        /* symbolic iterators in the domain */ \
        with (i,j) \
          /* read accesses into the array 'flip' */ \
          read flip {(i-1,j), (i,j+1),(i,j),(i,j-1),(i+1,j)}, \
          /* write access into the array 'flop' */ \
          write flop {(i,j)}
      for( int i = 1; i <= N; i += 1 ){
        for( int j = 1; j <= N; j += 1 ){
          flop[i][j] = (flip[i-1][j] + flip[i][j+1] + flip[i][j] + flip[i][j-1] + flip[i+1][j]) * (1.0/5.0);
        }
      }

      #pragma omplc for domain(1:N,1:N) \
        with (i,j) \
          read flop {(i-1,j), (i,j+1),(i,j),(i,j-1),(i+1,j)}, \
          write flip {(i,j)}
      for( int i = 1; i <= N; i += 1 ){
        for( int j = 1; j <= N; j += 1 ){
          flip[i][j] = (flop[i-1][j] + flop[i][j+1] + flop[i][j] + flop[i][j-1] + flop[i+1][j]) * (1.0/5.0);
        }
      }
    }

  }

  if( T % 2 == 1 ){
     #pragma omplc loopchain schedule()
    {
      #pragma omplc for domain(1:N,1:N) \
        with (i,j) \
          read flip {(i-1,j), (i,j+1),(i,j),(i,j-1),(i+1,j)}, \
          write flop {(i,j)}
      for( int i = 1; i <= N; i += 1 ){
        for( int j = 1; j <= N; j += 1 ){
          flop[i][j] = (flip[i-1][j] + flip[i][j+1] + flip[i][j] + flip[i][j-1] + flip[i+1][j]) * (1.0/5.0);
        }
      }
    }
  }

  return ( T % 2 == 1)?flop:flip;
}


double** control( int N, int T, double start ){
  double** flip = alloc_array( N + 2 );
  double** flop = alloc_array( N + 2 );

  flip[N/2+1][N/2 + 1] = start;
  for( int t = 0; t < T/2; ++t ){

    for( int i = 1; i <= N; i += 1 ){
      for( int j = 1; j <= N; j += 1 ){
        flop[i][j] = (flip[i-1][j] + flip[i][j+1] + flip[i][j] + flip[i][j-1] + flip[i+1][j]) * (1.0/5.0);
      }
    }

    for( int i = 1; i <= N; i += 1 ){
      for( int j = 1; j <= N; j += 1 ){
        flip[i][j] = (flop[i-1][j] + flop[i][j+1] + flop[i][j] + flop[i][j-1] + flop[i+1][j]) * (1.0/5.0);
      }
    }

  }

  if( T % 2 == 1 ){
    for( int i = 1; i <= N; i += 1 ){
      for( int j = 1; j <= N; j += 1 ){
        flop[i][j] = (flip[i-1][j] + flip[i][j+1] + flip[i][j] + flip[i][j-1] + flip[i+1][j]) * (1.0/5.0);
      }
    }
  }

  return ( T % 2 == 1)?flop:flip;
}

bool compare( int N, double** test, double** control ){
  for( int i = 1; i <= N; ++i ){
    for( int j = 1; j <= N; ++j ){
      if( test[i][j] != control[i][j] ) return false;
    }
  }
  return true;
}

int main( int argc, char** argv ){
  int N = 10;
  int T = 10;
  double start = 10.0;
  bool print = false;

  char c;
  while ((c = getopt (argc, argv, "n:t:s:p")) != -1){
    switch( c ) {
      case 's':
        start = atof( optarg );
        break;

      case 'n':
        N = atoi( optarg );
        break;

      case 't':
        T = atoi( optarg );
        break;

      case 'p':
        print = true;
        break;

      default:
        cout << "Incorrect arguments" << endl;
    }
  }

  double** test_result = test( N, T, start );
  double** control_result = control( N, T, start );
  bool correct = compare( N, test_result, control_result );

  if( correct ){
    cout << "Success!" << endl;
  } else {
    cout << "Fail!" << endl;
  }

  if( print ){
    cout << "Control:" << endl;
    for( int i = 1; i <= N; ++i ){
      for( int j = 1; j <= N; ++j ){
        cout << control_result[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl << "Test:" << endl;
    for( int i = 1; i <= N; ++i ){
      for( int j = 1; j <= N; ++j ){
        cout << test_result[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;
  }

}
