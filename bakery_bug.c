void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

//#include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()

int t1=0, t2=0; // N natural-number tickets
int x; // variable to test mutual exclusion


void thr1() {
  int tmp;
  while (1) {
    tmp = t2 + 1;
    t1 = tmp;
    while( t1 >= t2 && ( t2 > 0 ) ) {}; // condition to exit the loop is (t1<t2 \/ t2=0)
    x=0;
    assert(x <= 0);
    t1 = 0;
  }
 
}


void thr2() {
  while (1) {
    t2 = t1 + 1;
    while( t2 >= t1 && ( t1 > 0 ) ) {}; // condition to exit the loop is (t2<t1 \/ t1=0)
    x = 1;
    assert(x >= 1);
    t2 = 0;
  }
 
}

